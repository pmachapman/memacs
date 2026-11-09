#	Windows console makefile for MicroEMACS, using GCC (MinGW / TDM-GCC)
#
#	Build with:	make -C xp/gcc		(or mingw32-make)
#	h/estruct.h should be set up for WINXP and XPCON, as it is for
#	the xp/vs2017 project.

SRCD = ../../src
HDIR = ../../h
LUAD = $(SRCD)/lua
OBJD = obj

CC = gcc
CFLAGS = -O2 -w -I$(HDIR)
LUACFLAGS = -O2

EMACSOBJS = abbrev.o basic.o bind.o buffer.o char.o crypt.o display.o \
	dolock.o elua.o eval.o exec.o file.o fileio.o input.o isearch.o \
	keyboard.o line.o lock.o main.o mouse.o random.o region.o replace.o \
	screen.o search.o tags.o undo.o window.o word.o xp.o xpconio.o

LUAOBJS = lapi.o lauxlib.o lbaselib.o lcode.o lcorolib.o lctype.o ldblib.o \
	ldebug.o ldo.o ldump.o lfunc.o lgc.o linit.o liolib.o llex.o \
	lmathlib.o lmem.o loadlib.o lobject.o lopcodes.o loslib.o lparser.o \
	lstate.o lstring.o lstrlib.o ltable.o ltablib.o ltm.o lundump.o \
	lutf8lib.o lvm.o lzio.o

EOBJS = $(addprefix $(OBJD)/,$(EMACSOBJS))
LOBJS = $(addprefix $(OBJD)/lua_,$(LUAOBJS))

all: emacs.exe

emacs.exe: $(EOBJS) $(LOBJS)
	$(CC) -o $@ $(EOBJS) $(LOBJS)

$(OBJD)/%.o: $(SRCD)/%.c $(wildcard $(HDIR)/*.h) | $(OBJD)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJD)/lua_%.o: $(LUAD)/%.c | $(OBJD)
	$(CC) $(LUACFLAGS) -c -o $@ $<

$(OBJD):
	mkdir $(OBJD)

clean:
	rm -rf $(OBJD) emacs.exe

.PHONY: all clean
