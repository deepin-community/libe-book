/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <string>

#include "libebook_utils.h"
#include "EBOOKCharsetConverter.h"
#include "EBOOKMemoryStream.h"
#include "EBOOKZlibStream.h"
#include "PDBLZ77Stream.h"
#include "PluckerParser.h"

using std::unique_ptr;
using std::shared_ptr;

using std::vector;

namespace libebook
{

namespace
{

static const uint32_t PLUCKER_TYPE = PDB_CODE("Data");
static const uint32_t PLUCKER_CREATOR = PDB_CODE("Plkr");

static const uint32_t APPINFO_SIGNATURE = 0x6c6e6368;

enum Compression
{
  COMPRESSION_UNKNOWN,
  COMPRESSION_LZ77,
  COMPRESSION_ZLIB
};

enum DataType
{
  DATA_TYPE_PHTML,
  DATA_TYPE_PHTML_COMPRESSED,
  DATA_TYPE_TBMP,
  DATA_TYPE_TBMP_COMPRESSED,
  DATA_TYPE_MAILTO,
  DATA_TYPE_LINK_INDEX,
  DATA_TYPE_LINKS,
  DATA_TYPE_LINKS_COMPRESSED,
  DATA_TYPE_BOOKMARKS,
  DATA_TYPE_CATEGORY,
  DATA_TYPE_METADATA,
  DATA_TYPE_LAST = DATA_TYPE_METADATA,
  DATA_TYPE_UNKNOWN = 0xff
};

enum FunctionCode
{
  LINK_END = 0x8,
  PAGE_LINK_BEGIN = 0xa,
  PARAGRAPH_LINK_BEGIN = 0xc,
  SET_FONT = 0x11,
  EMBEDDED_IMAGE = 0x1a,
  SET_MARGIN = 0x22,
  TEXT_ALIGNMENT = 0x29,
  HORIZONTAL_RULE = 0x33,
  NEW_LINE = 0x38,
  ITALIC_BEGIN = 0x40,
  ITALIC_END = 0x48,
  UNKNOWN_53 = 0x53,
  MULTIPLE_EMBEDDED_IMAGE = 0x5c,
  UNDERLINE_BEGIN = 0x60,
  UNDERLINE_END = 0x68,
  STRIKETHROUGH_BEGIN = 0x70,
  STRIKETHROUGH_END = 0x78
};

enum TextAlignment
{
  TEXT_ALIGNMENT_LEFT,
  TEXT_ALIGNMENT_RIGHT,
  TEXT_ALIGNMENT_CENTER,
  TEXT_ALIGNMENT_LAST = TEXT_ALIGNMENT_CENTER
};

enum Font
{
  FONT_REGULAR,
  FONT_H1,
  FONT_H2,
  FONT_H3,
  FONT_H4,
  FONT_H5,
  FONT_H6,
  FONT_BOLD,
  FONT_FIXED_WIDTH,
  FONT_LAST = FONT_FIXED_WIDTH
};

typedef std::map<unsigned, unsigned> ExceptionalCharsetMap_t;

// source: http://www.iana.org/assignments/character-sets/character-sets.xhtml (2013)
static const char *const IANA_CHARSETS[] =
{
  /* 0 */
  nullptr, nullptr, nullptr,
  "US-ASCII",
  "ISO_8859-1:1987",
  "ISO_8859-2:1987",
  "ISO_8859-3:1988",
  "ISO_8859-4:1988",
  "ISO_8859-5:1988",
  "ISO_8859-6:1987",
  /* 10 */
  "ISO_8859-7:1987",
  "ISO_8859-8:1988",
  "ISO_8859-9:1989",
  "ISO-8859-10",
  "ISO_6937-2-add",
  "JIS_X0201",
  "JIS_Encoding",
  "Shift_JIS",
  "Extended_UNIX_Code_Packed_Format_for_Japanese",
  "Extended_UNIX_Code_Fixed_Width_for_Japanese",
  /* 20 */
  "BS_4730",
  "SEN_850200_C",
  "IT",
  "ES",
  "DIN_66003",
  "NS_4551-1",
  "NF_Z_62-010",
  "ISO-10646-UTF-1",
  "ISO_646.basic:1983",
  "INVARIANT",
  /* 30 */
  "ISO_646.irv:1983",
  "NATS-SEFI",
  "NATS-SEFI-ADD",
  "NATS-DANO",
  "NATS-DANO-ADD",
  "SEN_850200_B",
  "KS_C_5601-1987",
  "ISO-2022-KR",
  "EUC-KR",
  "ISO-2022-JP",
  /* 40 */
  "ISO-2022-JP-2",
  "JIS_C6220-1969-jp",
  "JIS_C6220-1969-ro",
  "PT",
  "greek7-old",
  "latin-greek",
  "NF_Z_62-010_(1973)",
  "Latin-greek-1",
  "ISO_5427",
  "JIS_C6226-1978",
  /* 50 */
  "BS_viewdata",
  "INIS",
  "INIS-8",
  "INIS-cyrillic",
  "ISO_5427:1981",
  "ISO_5428:1980",
  "GB_1988-80",
  "GB_2312-80",
  "NS_4551-2",
  "videotex-suppl",
  /* 60 */
  "PT2",
  "ES2",
  "MSZ_7795.3",
  "JIS_C6226-1983",
  "greek7",
  "ASMO_449",
  "iso-ir-90",
  "JIS_C6229-1984-a",
  "JIS_C6229-1984-b",
  "JIS_C6229-1984-b-add",
  /* 70 */
  "JIS_C6229-1984-hand",
  "JIS_C6229-1984-hand-add",
  "JIS_C6229-1984-kana",
  "ISO_2033-1983",
  "ANSI_X3.110-1983",
  "T.61-7bit",
  "T.61-8bit",
  "ECMA-cyrillic",
  "CSA_Z243.4-1985-1",
  "CSA_Z243.4-1985-2",
  /* 80 */
  "CSA_Z243.4-1985-gr",
  "ISO_8859-6-E",
  "ISO_8859-6-I",
  "T.101-G2",
  "ISO_8859-8-E",
  "ISO_8859-8-I",
  "CSN_369103",
  "JUS_I.B1.002",
  "IEC_P27-1",
  "JUS_I.B1.003-serb",
  /* 90 */
  "JUS_I.B1.003-mac",
  "greek-ccitt",
  "NC_NC00-10:81",
  "ISO_6937-2-25",
  "GOST_19768-74",
  "ISO_8859-supp",
  "ISO_10367-box",
  "latin-lap",
  "JIS_X0212-1990",
  "DS_2089",
  /* 100 */
  "us-dk",
  "dk-us",
  "KSC5636",
  "UNICODE-1-1-UTF-7",
  "ISO-2022-CN",
  "ISO-2022-CN-EXT",
  "UTF-8",
  nullptr, nullptr,
  "ISO-8859-13",
  /* 110 */
  "ISO-8859-14",
  "ISO-8859-15",
  "ISO-8859-16",
  "GBK",
  "GB18030",
  "OSD_EBCDIC_DF04_15",
  "OSD_EBCDIC_DF03_IRV",
  "OSD_EBCDIC_DF04_1",
  "ISO-11548-1",
  "KZ-1048",
  /* 120 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 130 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 140 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 150 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 160 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 170 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 180 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 190 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 200 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 210 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 220 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 230 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 240 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 250 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 260 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 270 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 280 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 290 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 300 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 310 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 320 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 330 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 340 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 350 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 360 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 370 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 380 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 390 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 400 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 410 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 420 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 430 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 440 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 450 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 460 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 470 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 480 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 490 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 500 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 510 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 520 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 530 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 540 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 550 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 560 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 570 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 580 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 590 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 600 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 610 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 620 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 630 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 640 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 650 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 660 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 670 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 680 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 690 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 700 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 710 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 720 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 730 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 740 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 750 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 760 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 770 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 780 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 790 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 800 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 810 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 820 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 830 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 840 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 850 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 860 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 870 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 880 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 890 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 900 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 910 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 920 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 930 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 940 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 950 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 960 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 970 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 980 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 990 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1000 */
  "ISO-10646-UCS-2",
  "ISO-10646-UCS-4",
  "ISO-10646-UCS-Basic",
  "ISO-10646-Unicode-Latin1",
  "ISO-10646-J-1",
  "ISO-Unicode-IBM-1261",
  "ISO-Unicode-IBM-1268",
  "ISO-Unicode-IBM-1276",
  "ISO-Unicode-IBM-1264",
  "ISO-Unicode-IBM-1265",
  /* 1010 */
  "UNICODE-1-1",
  "SCSU",
  "UTF-7",
  "UTF-16BE",
  "UTF-16LE",
  "UTF-16",
  "CESU-8",
  "UTF-32",
  "UTF-32BE",
  "UTF-32LE",
  /* 1020 */
  "BOCU-1",
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1030 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1040 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1050 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1060 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1070 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1080 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1090 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1100 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1110 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1120 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1130 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1140 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1150 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1160 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1170 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1180 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1190 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1200 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1210 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1220 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1230 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1240 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1250 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1260 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1270 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1280 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1290 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1300 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1310 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1320 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1330 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1340 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1350 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1360 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1370 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1380 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1390 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1400 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1410 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1420 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1430 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1440 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1450 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1460 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1470 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1480 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1490 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1500 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1510 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1520 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1530 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1540 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1550 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1560 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1570 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1580 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1590 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1600 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1610 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1620 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1630 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1640 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1650 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1660 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1670 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1680 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1690 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1700 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1710 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1720 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1730 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1740 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1750 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1760 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1770 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1780 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1790 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1800 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1810 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1820 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1830 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1840 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1850 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1860 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1870 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1880 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1890 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1900 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1910 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1920 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1930 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1940 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1950 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1960 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1970 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1980 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1990 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 2000 */
  "ISO-8859-1-Windows-3.0-Latin-1",
  "ISO-8859-1-Windows-3.1-Latin-1",
  "ISO-8859-2-Windows-Latin-2",
  "ISO-8859-9-Windows-Latin-5",
  "hp-roman8",
  "Adobe-Standard-Encoding",
  "Ventura-US",
  "Ventura-International",
  "DEC-MCS",
  "IBM850",
  /* 2010 */
  "IBM852",
  "IBM437",
  "PC8-Danish-Norwegian",
  "IBM862",
  "PC8-Turkish",
  "IBM-Symbols",
  "IBM-Thai",
  "HP-Legal",
  "HP-Pi-font",
  "HP-Math8",
  /* 2020 */
  "Adobe-Symbol-Encoding",
  "HP-DeskTop",
  "Ventura-Math",
  "Microsoft-Publishing",
  "Windows-31J",
  "GB2312",
  "Big5",
  "macintosh",
  "IBM037",
  "IBM038",
  /* 2030 */
  "IBM273",
  "IBM274",
  "IBM275",
  "IBM277",
  "IBM278",
  "IBM280",
  "IBM281",
  "IBM284",
  "IBM285",
  "IBM290",
  /* 2040 */
  "IBM297",
  "IBM420",
  "IBM423",
  "IBM424",
  "IBM500",
  "IBM851",
  "IBM855",
  "IBM857",
  "IBM860",
  "IBM861",
  /* 2050 */
  "IBM863",
  "IBM864",
  "IBM865",
  "IBM868",
  "IBM869",
  "IBM870",
  "IBM871",
  "IBM880",
  "IBM891",
  "IBM903",
  /* 2060 */
  "IBM904",
  "IBM905",
  "IBM918",
  "IBM1026",
  "EBCDIC-AT-DE",
  "EBCDIC-AT-DE-A",
  "EBCDIC-CA-FR",
  "EBCDIC-DK-NO",
  "EBCDIC-DK-NO-A",
  "EBCDIC-FI-SE",
  /* 2070 */
  "EBCDIC-FI-SE-A",
  "EBCDIC-FR",
  "EBCDIC-IT",
  "EBCDIC-PT",
  "EBCDIC-ES",
  "EBCDIC-ES-A",
  "EBCDIC-ES-S",
  "EBCDIC-UK",
  "EBCDIC-US",
  "UNKNOWN-8BIT",
  /* 2080 */
  "MNEMONIC",
  "MNEM",
  "VISCII",
  "VIQR",
  "KOI8-R",
  "HZ-GB-2312",
  "IBM866",
  "IBM775",
  "KOI8-U",
  "IBM00858",
  /* 2090 */
  "IBM00924",
  "IBM01140",
  "IBM01141",
  "IBM01142",
  "IBM01143",
  "IBM01144",
  "IBM01145",
  "IBM01146",
  "IBM01147",
  "IBM01148",
  /* 2100 */
  "IBM01149",
  "Big5-HKSCS",
  "IBM1047",
  "PTCP154",
  "Amiga-1251",
  "KOI7-switched",
  "BRF",
  "TSCII",
  "CP51932",
  "windows-874",
  /* 2110 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 2120 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 2130 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 2140 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 2150 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 2160 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 2170 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 2180 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 2190 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 2200 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 2210 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 2220 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 2230 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 2240 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 2250 */
  "windows-1250",
  "windows-1251",
  "windows-1252",
  "windows-1253",
  "windows-1254",
  "windows-1255",
  "windows-1256",
  "windows-1257",
  "windows-1258",
  "TIS-620",
  /* 2260 */
  "CP50220",
};

}

}

namespace libebook
{
namespace
{

struct PluckerAttributes
{
  PluckerAttributes();

  Font font;
  unsigned leftMargin;
  unsigned rightMargin;
  TextAlignment textAlignment;
  bool italic;
  bool underline;
  bool strikethrough;
};

PluckerAttributes::PluckerAttributes()
  : font(FONT_REGULAR)
  , leftMargin(0)
  , rightMargin(0)
  , textAlignment(TEXT_ALIGNMENT_LEFT)
  , italic(false)
  , underline(false)
  , strikethrough(false)
{
}

librevenge::RVNGPropertyList makeParagraphProperties(const PluckerAttributes &attributes)
{
  librevenge::RVNGPropertyList props;

  switch (attributes.textAlignment)
  {
  case TEXT_ALIGNMENT_LEFT :
    props.insert("fo:text-align", "left");
    break;
  case TEXT_ALIGNMENT_RIGHT :
    props.insert("fo:text-align", "end");
    break;
  case TEXT_ALIGNMENT_CENTER :
    props.insert("fo:text-align", "center");
    break;
  default:
    break;
  }

  return props;
}

librevenge::RVNGPropertyList makeCharacterProperties(const PluckerAttributes &attributes)
{
  librevenge::RVNGPropertyList props;

  if (attributes.italic)
    props.insert("fo:font-style", "italic");

  if (attributes.underline)
    props.insert("style:text-underline-type", "single");

  if (attributes.strikethrough)
    props.insert("style:text-line-through-type", "single");

  if (((FONT_H1 <= attributes.font) && (FONT_H6 >= attributes.font)) || (FONT_BOLD == attributes.font))
    props.insert("fo:font-weight", "bold");

  return props;
}

class MarkupParser
{
  // -Weffc++
  MarkupParser(const MarkupParser &other);
  MarkupParser &operator=(const MarkupParser &other);

public:
  MarkupParser(librevenge::RVNGTextInterface *document, const PluckerImageMap_t &imageMap);
  ~MarkupParser();

  /** Parse an input stream.
    *
    * The function can be called more than once. In that case, the
    * parsing continues with the old state.
    *
    * @arg[in] input input stream
    * @arg[in] paragraphLengths lengths of paragraphs in the text block
    */
  void parse(librevenge::RVNGInputStream *input, const vector<unsigned> &paragraphLengths);

private:
  void flushText(bool endOfParagraph = false);

  void closeParagraph();

  /** Insert a line break.
    *
    * Only line breaks in the middle of a paragraph (e.g., separators of
    * verses in a poem) are really inserted. Line breaks at the
    * beginning of a paragraph (before any text) and at the end of a
    * paragraph (after all text) are ignored.
    */
  void insertLineBreak();

  void insertImage(unsigned id);

private:
  librevenge::RVNGTextInterface *m_document;
  const PluckerImageMap_t &m_imageMap;

  librevenge::RVNGInputStream *m_input;

  PluckerAttributes m_attributes;

  std::string m_text;

  unsigned m_lineBreaks;

  bool m_paragraphOpened;
};

MarkupParser::MarkupParser(librevenge::RVNGTextInterface *const document, const PluckerImageMap_t &imageMap)
  : m_document(document)
  , m_imageMap(imageMap)
  , m_input(nullptr)
  , m_attributes()
  , m_text()
  , m_lineBreaks(0)
  , m_paragraphOpened(false)
{
}

MarkupParser::~MarkupParser()
{
  closeParagraph();
}

void MarkupParser::parse(librevenge::RVNGInputStream *const input, const vector<unsigned> &paragraphLengths)
{
  unsigned para = 0;
  unsigned chars = 0;

  while (!input->isEnd())
  {
    const unsigned char c = readU8(input);
    ++chars;

    if (0 == c)
    {
      const unsigned char function = readU8(input);
      ++chars;

      switch (function)
      {
      case PAGE_LINK_BEGIN :
        skip(input, 2);
        chars += 2;
        break;
      case PARAGRAPH_LINK_BEGIN :
        skip(input, 4);
        chars += 4;
        break;
      case LINK_END :
        break;
      case SET_FONT :
      {
        const unsigned char font = readU8(input);
        ++chars;
        if (FONT_LAST >= font)
          m_attributes.font = static_cast<Font>(font);
        else
        {
          EBOOK_DEBUG_MSG(("unknown font specifier %d\n", font));
        }
        break;
      }
      case EMBEDDED_IMAGE :
      {
        const unsigned imageID = readU16(input, true);
        chars += 2;
        insertImage(imageID);
        break;
      }
      case SET_MARGIN :
        m_attributes.leftMargin = readU8(input);
        m_attributes.rightMargin = readU8(input);
        chars += 2;
        break;
      case TEXT_ALIGNMENT :
      {
        const unsigned alignmnent = readU8(input);
        ++chars;
        if (TEXT_ALIGNMENT_LAST >= alignmnent)
          m_attributes.textAlignment = static_cast<TextAlignment>(alignmnent);
        else
        {
          EBOOK_DEBUG_MSG(("unknown text alignment %d\n", (int) alignmnent));
        }
        break;
      }
      case HORIZONTAL_RULE :
        // ignore
        skip(input, 3);
        chars += 3;
        break;
      case NEW_LINE :
        flushText();
        insertLineBreak();
        break;
      case ITALIC_BEGIN :
        flushText();
        m_attributes.italic = true;
        break;
      case ITALIC_END :
        flushText();
        m_attributes.italic = false;
        break;
      case UNKNOWN_53 :
        // TODO: find what this is
        skip(input, 3);
        chars += 3;
        break;
      case MULTIPLE_EMBEDDED_IMAGE :
        // TODO: implement me
        skip(input, 4);
        chars += 4;
        break;
      case UNDERLINE_BEGIN :
        flushText();
        m_attributes.underline = true;
        break;
      case UNDERLINE_END :
        flushText();
        m_attributes.underline = false;
        break;
      case STRIKETHROUGH_BEGIN :
        flushText();
        m_attributes.strikethrough = true;
        break;
      case STRIKETHROUGH_END :
        flushText();
        m_attributes.strikethrough = false;
        break;
      default :
        EBOOK_DEBUG_MSG(("unknown function code %x\n", function));
        break;
      }
    }
    else
    {
      m_text.push_back((char) c);
    }

    if ((paragraphLengths.size() > para) && (paragraphLengths[para] <= chars))
    {
      closeParagraph();
      ++para;
      chars = 0;
    }
  }
}

void MarkupParser::flushText(bool endOfParagraph)
{
  if (!m_paragraphOpened)
  {
    m_document->openParagraph(makeParagraphProperties(m_attributes));
    m_paragraphOpened = true;
    m_lineBreaks = 0;
  }

  // Many files I have seen have a line break followed by a space at the
  // end of paragraphs. IMHO that might be safely thrown away.
  if (!m_text.empty() && (!endOfParagraph || (std::string::npos != m_text.find_first_not_of(" "))))
  {
    // pending line break(s)
    if (0 != m_lineBreaks)
    {
      for (unsigned i = 0; i != m_lineBreaks; ++i)
        m_document->insertLineBreak();
      m_lineBreaks = 0;
    }

    m_document->openSpan(makeCharacterProperties(m_attributes));
    m_document->insertText(librevenge::RVNGString(m_text.c_str()));
    m_text.clear();
    m_document->closeSpan();
  }
}

void MarkupParser::closeParagraph()
{
  flushText(true);

  m_document->closeParagraph();
  m_paragraphOpened = false;
}

void MarkupParser::insertLineBreak()
{
  ++m_lineBreaks;
}

void MarkupParser::insertImage(const unsigned id)
{
  const PluckerImageMap_t::const_iterator it = m_imageMap.find(id);
  if (m_imageMap.end() != it)
  {
    librevenge::RVNGPropertyList props;
    const librevenge::RVNGBinaryData data(&(it->second)[0], it->second.size());
    props.insert("office:binary-data", data);
    m_document->insertBinaryObject(props);
  }
}

}
}

namespace libebook
{

struct PluckerHeader
{
  PluckerHeader();

  bool isValid() const;

  Compression compression;
  bool valid;
  bool validAppInfo;
};

struct PluckerParserState
{
  PluckerParserState();

  PluckerImageMap_t m_imageMap;
  shared_ptr<MarkupParser> markupParser;
  shared_ptr<EBOOKCharsetConverter> charsetConverter;
  bool knownEncoding;
  ExceptionalCharsetMap_t exceptionalCharsetMap;
};

struct PluckerRecordHeader
{
  PluckerRecordHeader();

  unsigned number;
  unsigned uid;
  unsigned paragraphs;
  unsigned size;
  DataType type;
};

PluckerHeader::PluckerHeader()
  : compression(COMPRESSION_UNKNOWN)
  , valid(false)
  , validAppInfo(true)
{
}

bool PluckerHeader::isValid() const
{
  return (COMPRESSION_UNKNOWN != compression) && valid && validAppInfo;
}

PluckerParserState::PluckerParserState()
  : m_imageMap()
  , markupParser()
  , charsetConverter()
  , knownEncoding(false)
  , exceptionalCharsetMap()
{
}

PluckerRecordHeader::PluckerRecordHeader()
  : number(0)
  , uid(0)
  , paragraphs(0)
  , size(0)
  , type(DATA_TYPE_UNKNOWN)
{
}

}

namespace libebook
{

PluckerParser::PluckerParser(librevenge::RVNGInputStream *const input, librevenge::RVNGTextInterface *const document)
  : PDBParser(input, document, PLUCKER_TYPE, PLUCKER_CREATOR)
  , m_header()
  , m_state(new PluckerParserState())
{
  if (!m_header)
    m_header.reset(new PluckerHeader());

  const std::unique_ptr<librevenge::RVNGInputStream> record(getIndexRecord());
  readIndexRecord(record.get());

  if (!m_header->isValid())
    throw UnsupportedFormat();
}

bool PluckerParser::checkType(const unsigned type, const unsigned creator)
{
  return (PLUCKER_TYPE == type) && (PLUCKER_CREATOR == creator);
}

void PluckerParser::readAppInfoRecord(librevenge::RVNGInputStream *const record)
{
  const uint32_t signature = readU32(record, true);
  const unsigned version = readU16(record, true);
  const unsigned encoding = readU16(record, true);

  m_header->validAppInfo = (APPINFO_SIGNATURE == signature) && (3 == version) && (0 == encoding);
}

void PluckerParser::readSortInfoRecord(librevenge::RVNGInputStream *)
{
  // there is no sortInfo record in Plucker
}

void PluckerParser::readIndexRecord(librevenge::RVNGInputStream *const record)
{
  if (!m_header)
    m_header.reset(new PluckerHeader());

  m_header->valid = 1 == readU16(record, true);

  const unsigned version = readU16(record, true);
  switch (version)
  {
  case 1 :
    m_header->compression = COMPRESSION_LZ77;
    break;
  case 2 :
    m_header->compression = COMPRESSION_ZLIB;
    break;
  default :
    EBOOK_DEBUG_MSG(("unknown compression %d\n", (int) version));
    break;
  }
}

void PluckerParser::readDataRecord(librevenge::RVNGInputStream *const record, bool)
{
  // TODO: implement me
  (void) record;
}

void PluckerParser::readDataRecords()
{
  vector<PluckerRecordHeader> textRecords;

  // Process in two phases:

  // 1. save images, process metadata and (since we are reading the
  // record headers anyway) save data about text records
  for (unsigned i = 0; i < getDataRecordCount(); ++i)
  {
    const unique_ptr<librevenge::RVNGInputStream> record(getDataRecord(i));

    PluckerRecordHeader header;
    header.number = i;
    header.uid = readU16(record.get(), true);
    header.paragraphs = readU16(record.get(), true);
    header.size = readU16(record.get(), true);
    const unsigned typeNum = readU8(record.get(), true);
    header.type = DATA_TYPE_UNKNOWN;

    if (DATA_TYPE_LAST >= typeNum)
      header.type = static_cast<DataType>(typeNum);

    switch (header.type)
    {
    case DATA_TYPE_PHTML :
    case DATA_TYPE_PHTML_COMPRESSED :
      textRecords.push_back(header);
      break;
    case DATA_TYPE_TBMP :
    case DATA_TYPE_TBMP_COMPRESSED :
    {
      librevenge::RVNGInputStream *input = record.get();

      std::shared_ptr<librevenge::RVNGInputStream> uncompressed;
      if (DATA_TYPE_TBMP_COMPRESSED == header.type)
      {
        uncompressed = getUncompressedStream(input);
        input = uncompressed.get();
      }

      readImage(input, header);

      break;
    }
    case DATA_TYPE_METADATA :
      readMetadata(record.get(), header);
      break;
    case DATA_TYPE_MAILTO:
    case DATA_TYPE_LINK_INDEX:
    case DATA_TYPE_LINKS:
    case DATA_TYPE_LINKS_COMPRESSED:
    case DATA_TYPE_BOOKMARKS:
    case DATA_TYPE_CATEGORY:
    case DATA_TYPE_UNKNOWN:
    default :
      // not interesting
      break;
    }
  }

  // 2. process text records and generate output
  getDocument()->startDocument(librevenge::RVNGPropertyList());
  getDocument()->setDocumentMetaData(librevenge::RVNGPropertyList());
  getDocument()->openPageSpan(getDefaultPageSpanPropList());

  // create markup parser
  m_state->markupParser.reset(new MarkupParser(getDocument(), m_state->m_imageMap));

  for (vector<PluckerRecordHeader>::const_iterator it = textRecords.begin(); it != textRecords.end(); ++it)
  {
    const unique_ptr<librevenge::RVNGInputStream> record(getDataRecord(it->number));

    if (it->type==DATA_TYPE_PHTML || it->type==DATA_TYPE_PHTML_COMPRESSED)
    {
      librevenge::RVNGInputStream *input = record.get();

      skip(input, 8);

      vector<unsigned> paraLengths;
      for (unsigned i = 0; i != it->paragraphs; ++i)
      {
        paraLengths.push_back(readU16(input, true));
        skip(input, 2);
      }

      shared_ptr<librevenge::RVNGInputStream> uncompressed;
      if (DATA_TYPE_PHTML_COMPRESSED == it->type)
      {
        uncompressed = getUncompressedStream(input);
        input = uncompressed.get();
      }

      readText(input, *it, paraLengths);

      break;
    }
    else
    {
      // how comes?
      EBOOK_DEBUG_MSG(("unknown data type %d for text record\n", it->type));
    }
  }

  m_state->markupParser.reset();

  getDocument()->closePageSpan();
  getDocument()->endDocument();
}

void PluckerParser::readMetadata(librevenge::RVNGInputStream *const input, const PluckerRecordHeader &)
{
  const unsigned count = readU16(input, true);
  for (unsigned i = 0; count != i; ++i)
  {
    const unsigned typeCode = readU16(input, true);
    const unsigned length = readU16(input, true);

    switch (typeCode)
    {
    case 1 : // charset
      if (1 == length)
      {
        const unsigned mib = readU16(input, true);
        const char *charset = nullptr;
        if (EBOOK_NUM_ELEMENTS(IANA_CHARSETS) > mib)
          charset = IANA_CHARSETS[mib];
        m_state->charsetConverter.reset(new EBOOKCharsetConverter(charset));
        m_state->knownEncoding = nullptr != charset;
      }
      else
      {
        EBOOK_DEBUG_MSG(("invalid record length %d for charset record\n", (int) length));
      }
      break;
    case 2 : // exceptional charsets
      for (unsigned j = 0; length != j; j += 2)
      {
        const unsigned id = readU16(input, true);
        const unsigned mib = readU16(input, true);
        m_state->exceptionalCharsetMap[id] = mib;
      }
      break;
    default :
      EBOOK_DEBUG_MSG(("unknown type code %d in metadata record\n", (int) typeCode));
    }
  }
}

void PluckerParser::readImage(librevenge::RVNGInputStream *const input, const PluckerRecordHeader &header)
{
  vector<unsigned char> data;
  while (!input->isEnd())
    data.push_back(readU8(input));

  m_state->m_imageMap.insert(PluckerImageMap_t::value_type(header.uid, data));
}

void PluckerParser::readText(librevenge::RVNGInputStream *const input, const PluckerRecordHeader &, const std::vector<unsigned> &paragraphLengths)
{
  m_state->markupParser->parse(input, paragraphLengths);
}

std::shared_ptr<librevenge::RVNGInputStream> PluckerParser::getUncompressedStream(librevenge::RVNGInputStream *const input) const
{
  const auto pos = (unsigned long) input->tell();
  input->seek(0, librevenge::RVNG_SEEK_END);
  const unsigned long length = (unsigned long) input->tell() - pos;
  input->seek((long) pos, librevenge::RVNG_SEEK_SET);
  const unsigned char *bytes = readNBytes(input, length);

  EBOOKMemoryStream data(bytes, static_cast<unsigned>(length));

  shared_ptr<librevenge::RVNGInputStream> uncompressed;
  switch (m_header->compression)
  {
  case COMPRESSION_LZ77 :
    uncompressed.reset(new PDBLZ77Stream(&data));
    break;
  case COMPRESSION_ZLIB :
    uncompressed.reset(new EBOOKZlibStream(&data));
    break;
  case COMPRESSION_UNKNOWN:
  default :
    // not possible
    break;
  }

  return uncompressed;
}

} // namespace libebook

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
