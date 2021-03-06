TARGET := audiorecorder
MIPSTARGET := mips/audiorecorder
OPKNAME := audiorecorder.opk
CSRCS := $(wildcard *.c) $(wildcard gamegui/*.c) $(wildcard containerlib/*.c)
CPPSRCS := $(wildcard *.cpp) 
HDRS := $(wildcard *.h)
OBJS := $(CSRCS:.c=.o) $(CPPSRCS:.cpp=.o)
MIPSOBJS := $(CSRCS:%.c=mips/%.o) $(CPPSRCS:%.cpp=mips/%.o)
CC := gcc
CPP := g++
AR := ar
RANLIB := ranlib
MIPSCC := mipsel-linux-gcc
MIPSCPP := mipsel-linux-g++

DEBUGFLAGS := -O0 -DDEBUG -ggdb
RELEASEFLAGS := -O2 -DRELEASE 
CFLAGS := -Wall -std=gnu99  $(DEBUGFLAGS)
CPPFLAGS := -Wall -std=c++11 
LDFLAGS := -lm -lSDL2_ttf -lSDL2_image -lasound -lsndfile

SYSROOT := $(shell $(CC) --print-sysroot)
GCWSYSROOT := $(shell $(MIPSCC) --print-sysroot)
X86CFLAGS := $(shell $(SYSROOT)/usr/bin/sdl2-config --cflags)
X86LDFLAGS := $(shell $(SYSROOT)/usr/bin/sdl2-config --libs)
GCWCFLAGS := $(shell $(GCWSYSROOT)/usr/bin/sdl2-config --cflags)
GCWLDFLAGS := $(shell $(GCWSYSROOT)/usr/bin/sdl2-config --libs)

.SUFFIXES: .c .cpp
.PHONY: clean all

all: $(TARGET) $(MIPSTARGET)

%.o : %.c
	$(CC) -c $(CFLAGS) $(X86CFLAGS) -o $@ $<

%.o : %.cpp
	$(CPP) -c $(CPPFLAGS) $(X86CPPFLAGS) -o $@ $<

mips/%.o : %.c
	$(MIPSCC) -DGCW0 -c $(CFLAGS) $(GCWCFLAGS) -o $@ $<

mips/%.o : %.cpp
	$(MIPSCPP) -DGCW0 -c $(CPPFLAGS) $(GCWCPPFLAGS) -o $@ $<

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $(X86LDFLAGS) $^ -o $@

$(MIPSTARGET): $(MIPSOBJS)
	$(MIPSCC) $(LDFLAGS) $(GCWLDFLAGS) $^ -o $@

clean:
	rm -f $(TARGET) $(OBJS)
	rm -f mips/$(TARGET) $(MIPSOBJS)
	rm -rf release/*
	
$(OPKNAME): $(MIPSTARGET)

dist: $(OPKNAME)
	mkdir -p release
	rm -rf release/*
	cp -R assets release
	cp $(MIPSTARGET) release
	cp default.gcw0.desktop release
	cp audiorecorder.png release/appicon.png
	mksquashfs release $^ -all-root -noappend -no-exports -no-xattrs
