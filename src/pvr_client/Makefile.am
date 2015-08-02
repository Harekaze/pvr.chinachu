LIB = ../$(ADDONNAME)

lib_LTLIBRARIES = libpvrchinachu.la

libpvrchinachu_la_SOURCES = \
	channel.cpp \
	demux.cpp \
	information.cpp \
	initialize.cpp \
	live.cpp \
	recorded.cpp \
	settings.cpp \
	stream.cpp \
	timer.cpp

libpvrchinachu_la_LIBADD = \
	../chinachu/libchinachu.la

libpvrchinachu_la_LDFLAGS = \
	-avoid-version

AM_CPPFLAGS = \
	$(AM_CPPFLAGS_EXT) \
	-I. \
	-I$(top_srcdir)/include -I$(top_srcdir)/src

EXTRA_DIST = \
	include

$(ADDONNAME): libpvrchinachu.la
	cp -f .libs/libpvrchinachu.$(LIBEXT) $(ADDONNAME)

$(LIB): $(ADDONNAME)
	cp -f $(ADDONNAME) $(LIB)

all: $(LIB)

clean-local:
	rm -f $(ADDONNAME)
