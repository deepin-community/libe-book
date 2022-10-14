## -*- Mode: make; tab-width: 4; indent-tabs-mode: tabs -*-

SUBDIRS = lib

if WITH_LIBEBOOK_TOOLS
SUBDIRS += conv
endif

if BUILD_FUZZERS
SUBDIRS += fuzz
endif

if WITH_LIBEBOOK_TESTS
SUBDIRS += test
endif

## vim:set shiftwidth=4 tabstop=4 noexpandtab:
