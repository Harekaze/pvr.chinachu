noinst_LTLIBRARIES = libchinachu.la

libchinachu_la_SOURCES = \
	api.cpp \
	api.h \
	chinachu.h \
	recorded.cpp \
	recorded.h \
	reserves.cpp \
	reserves.h \
	schedule.cpp \
	schedule.h

libchinachu_la_LDFLAGS = \
	-avoid-version

AM_CPPFLAGS = \
	$(AM_CPPFLAGS_EXT) \
	-I. \
	-I$(top_srcdir)/include -I$(top_srcdir)/src

EXTRA_DIST = \
	include

all: libchinachu.la
