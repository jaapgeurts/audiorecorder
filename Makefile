TARGET := audiorecorder
MIPSTARGET := mips/audiorecorder
SRCS := $(wildcard *.c)
HDRS := $(wildcard *.h)
OBJS := $(SRCS:.c=.o)
MIPSOBJS := $(SRCS:%.c=mips/%.o)
CC := gcc
AR := ar
RANLIB := ranlib
MIPSCC := mipsel-linux-gcc

CFLAGS := -Wall -std=gnu99 
DEBUGFLAGS := -O0 -DDEBUG -ggdb
RELEASEFLAGS := -O2 -DRELEASE 
LDFLAGS := -lm -lasound

SYSROOT := $(shell $(CC) --print-sysroot)
GCWSYSROOT := $(shell $(MIPSCC) --print-sysroot)
X86CFLAGS := $(CFLAGS) $(shell $(SYSROOT)/usr/bin/sdl2-config --cflags)
X86LDFLAGS := $(LDFLAGS) $(shell $(SYSROOT)/usr/bin/sdl2-config --libs)
GCWCFLAGS := $(CFLAGS) $(shell $(GCWSYSROOT)/usr/bin/sdl2-config --cflags)
GCWLDFLAGS := $(LDFLAGS) $(shell $(GCWSYSROOT)/usr/bin/sdl2-config --libs)


.PHONY: clean all

all: $(TARGET) $(MIPSTARGET)

%.o : %.c
	$(CC) -c $(X86CFLAGS) -o $@ $<

mips/%.o : %.c
	$(MIPSCC) -DGCW0 -c $(GCWCFLAGS) -o $@ $<

$(TARGET): $(OBJS)
	$(CC) $(X86LDFLAGS) $^ -o $@

$(MIPSTARGET): $(MIPSOBJS)
	$(MIPSCC) $(GCWLDFLAGS) $^ -o $@

clean:
	rm -f $(TARGET) $(OBJS)
	rm -f mips/$(TARGET) $(MIPSOBJS)
