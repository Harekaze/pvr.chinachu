CXX = g++
CXXFLAGS = -g -O2
INCLUDES = -I. -I../include -I../src
CXXCOMPILE = $(CXX) $(INCLUDES) $(CXXFLAGS)
ADDONNAME = PVRChinachu.pvr
LIBNAME = libpvrchinachu.dylib
OBJS = ./chinachu/api.o \
./chinachu/schedule.o \
./pvr_client/channel.o \
./pvr_client/demux.o \
./pvr_client/information.o \
./pvr_client/initialize.o \
./pvr_client/live.o \
./pvr_client/recorded.o \
./pvr_client/settings.o \
./pvr_client/stream.o \
./pvr_client/timer.o

.SUFFIXES: .cpp .o

.PHONY: all
all: $(ADDONNAME)

$(ADDONNAME): $(OBJS)
	$(CXX) -Wl,-undefined -Wl,dynamic_lookup -o $(ADDONNAME) $^ -lz -O2 -dynamiclib -Wl,-single_module -install_name /lib/$(LIBNAME)

.cpp.o:
	$(CXXCOMPILE) -MT $@ -MP -c -o $@ $<

.PHONY: clean
clean:
	$(RM) -rf $(ADDONLIBRARY) $(ADDONNAME) $(OBJS) $(LIBNAME).dSYM
