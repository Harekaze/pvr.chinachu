noinst_LTLIBRARIES = libchinachu.la

libchinachu_la_SOURCES = \
	api.cpp \
	api.h \
	chinachu.h \
	recorded.cpp \
	recorded.h \
	recording.cpp \
	recording.h \
	reserves.cpp \
	reserves.h \
	rules.cpp \
	rules.h \
	schedule.cpp \
	schedule.h

libchinachu_la_LDFLAGS = \
	-avoid-version

AM_CPPFLAGS = \
	$(AM_CPPFLAGS_EXT) \
	-std=c++11 \
	-I. \
	-I$(top_srcdir)/include -I$(top_srcdir)/src

EXTRA_DIST = \
	include

all: libchinachu.la
