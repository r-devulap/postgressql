MODULE_big = test_crc32c
OBJS = test_crc32c.o
PGFILEDESC = "test"
EXTENSION = test_crc32c
DATA = test_crc32c--1.0.sql

first: all

# test_crc32c.o:	CFLAGS+=-g

ifdef USE_PGXS
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
else
subdir = src/test/modules/test_crc32c
top_builddir = ../../../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif
