#	Unix makefile for
#		MicroEMACS 3.12
#		(C)Copyright 1993 by Daniel Lawrence
#		all rights reserved

# modify the following lines with the proper relative directories

# object file directory
OBJD = obj
# source file directory
SRCD = ../../src
# header file directory
HDIR = ../../h

# options and arguments to the C compiler

CFLAGS= -I. -I$(HDIR) -O -J

# list of header files

HFILES = estruct.h $(HDIR)/edef.h $(HDIR)/eproto.h $(HDIR)/elang.h

#list of object files

F1 = $(OBJD)/abbrev.o $(OBJD)/basic.o $(OBJD)/bind.o $(OBJD)/buffer.o
F2 = $(OBJD)/char.o $(OBJD)/crypt.o $(OBJD)/display.o $(OBJD)/dolock.o
F3 = $(OBJD)/eval.o $(OBJD)/exec.o $(OBJD)/file.o $(OBJD)/keyboard.o
F4 = $(OBJD)/fileio.o $(OBJD)/unix.o $(OBJD)/input.o
F5 = $(OBJD)/isearch.o $(OBJD)/libhelp.o $(OBJD)/line.o $(OBJD)/lock.o
F6 = $(OBJD)/main.o $(OBJD)/message.o $(OBJD)/mouse.o $(OBJD)/screen.o
F7 = $(OBJD)/random.o $(OBJD)/region.o $(OBJD)/search.o $(OBJD)/replace.o
F8 = $(OBJD)/tags.o $(OBJD)/undo.o $(OBJD)/window.o $(OBJD)/word.o

# product list

all:	bin/emacs

OFILES =	$(F1) $(F2) $(F3) $(F4) $(F5) $(F6) $(F7) $(F8)

bin/emacs:	$(OFILES)
		$(CC) $(CFLAGS) $(OFILES) -s -ltermcap -lc -ltinfo -o bin/emacs

$(OBJD)/abbrev.o: $(SRCD)/abbrev.c $(HFILES)
	$(CC) $(CFLAGS) -c -Fo$(OBJD)/abbrev.o $(SRCD)/abbrev.c
$(OBJD)/basic.o: $(SRCD)/basic.c $(HFILES)
	$(CC) $(CFLAGS) -c -Fo$(OBJD)/basic.o $(SRCD)/basic.c
$(OBJD)/bind.o: $(SRCD)/bind.c $(HFILES) $(HDIR)/epath.h
	$(CC) $(CFLAGS) -c -Fo$(OBJD)/bind.o $(SRCD)/bind.c
$(OBJD)/buffer.o: $(SRCD)/buffer.c $(HFILES)
	$(CC) $(CFLAGS) -c -Fo$(OBJD)/buffer.o $(SRCD)/buffer.c
$(OBJD)/char.o: $(SRCD)/char.c $(HFILES)
	$(CC) $(CFLAGS) -c -Fo$(OBJD)/char.o $(SRCD)/char.c
$(OBJD)/crypt.o: $(SRCD)/crypt.c $(HFILES)
	$(CC) $(CFLAGS) -c -Fo$(OBJD)/crypt.o $(SRCD)/crypt.c
$(OBJD)/cmdfix.o: $(SRCD)/cmdfix.c $(HFILES)
	$(CC) $(CFLAGS) -c -Fo$(OBJD)/cmdfix.o $(SRCD)/cmdfix.c
$(OBJD)/display.o: $(SRCD)/display.c $(HFILES)
	$(CC) $(CFLAGS) -c -Fo$(OBJD)/display.o $(SRCD)/display.c
$(OBJD)/dolock.o: $(SRCD)/dolock.c $(HFILES)
	$(CC) $(CFLAGS) -c -Fo$(OBJD)/dolock.o $(SRCD)/dolock.c
$(OBJD)/eval.o: $(SRCD)/eval.c $(HFILES) $(HDIR)/evar.h
	$(CC) $(CFLAGS) -c -Fo$(OBJD)/eval.o $(SRCD)/eval.c
$(OBJD)/exec.o: $(SRCD)/exec.c $(HFILES)
	$(CC) $(CFLAGS) -c -Fo$(OBJD)/exec.o $(SRCD)/exec.c
$(OBJD)/file.o: $(SRCD)/file.c $(HFILES)
	$(CC) $(CFLAGS) -c -Fo$(OBJD)/file.o $(SRCD)/file.c
$(OBJD)/fileio.o: $(SRCD)/fileio.c $(HFILES)
	$(CC) $(CFLAGS) -c -Fo$(OBJD)/fileio.o $(SRCD)/fileio.c
$(OBJD)/input.o: $(SRCD)/input.c $(HFILES)
	$(CC) $(CFLAGS) -c -Fo$(OBJD)/input.o $(SRCD)/input.c
$(OBJD)/isearch.o: $(SRCD)/isearch.c $(HFILES)
	$(CC) $(CFLAGS) -c -Fo$(OBJD)/isearch.o $(SRCD)/isearch.c
$(OBJD)/keyboard.o: $(SRCD)/keyboard.c $(HFILES)
	$(CC) $(CFLAGS) -c -Fo$(OBJD)/keyboard.o $(SRCD)/keyboard.c
$(OBJD)/libhelp.o: $(SRCD)/libhelp.c $(HFILES)
	$(CC) $(CFLAGS) -c -Fo$(OBJD)/libhelp.o $(SRCD)/libhelp.c
$(OBJD)/line.o: $(SRCD)/line.c $(HFILES)
	$(CC) $(CFLAGS) -c -Fo$(OBJD)/line.o $(SRCD)/line.c
$(OBJD)/lock.o: $(SRCD)/lock.c $(HFILES)
	$(CC) $(CFLAGS) -c -Fo$(OBJD)/lock.o $(SRCD)/lock.c
$(OBJD)/main.o: $(SRCD)/main.c $(HFILES) $(HDIR)/efunc.h $(HDIR)/ebind.h
	$(CC) $(CFLAGS) -c -Fo$(OBJD)/main.o $(SRCD)/main.c
$(OBJD)/message.o: $(SRCD)/message.c $(HFILES) $(HDIR)/epath.h
	$(CC) $(CFLAGS) -c -Fo$(OBJD)/message.o $(SRCD)/message.c
$(OBJD)/mouse.o: $(SRCD)/mouse.c $(HFILES)
	$(CC) $(CFLAGS) -c -Fo$(OBJD)/mouse.o $(SRCD)/mouse.c
$(OBJD)/unix.o: $(SRCD)/unix.c $(HFILES)
	$(CC) $(CFLAGS) -c -Fo$(OBJD)/unix.o $(SRCD)/unix.c
$(OBJD)/random.o: $(SRCD)/random.c $(HFILES)
	$(CC) $(CFLAGS) -c -Fo$(OBJD)/random.o $(SRCD)/random.c
$(OBJD)/region.o: $(SRCD)/region.c $(HFILES)
	$(CC) $(CFLAGS) -c -Fo$(OBJD)/region.o $(SRCD)/region.c
$(OBJD)/screen.o: $(SRCD)/screen.c $(HFILES)
	$(CC) $(CFLAGS) -c -Fo$(OBJD)/screen.o $(SRCD)/screen.c
$(OBJD)/search.o: $(SRCD)/search.c $(HFILES)
	$(CC) $(CFLAGS) -c -Fo$(OBJD)/search.o $(SRCD)/search.c
$(OBJD)/replace.o: $(SRCD)/replace.c $(HFILES)
	$(CC) $(CFLAGS) -c -Fo$(OBJD)/replace.o $(SRCD)/replace.c
$(OBJD)/tags.o: $(SRCD)/tags.c $(HFILES)
	$(CC) $(CFLAGS) -c -Fo$(OBJD)/tags.o $(SRCD)/tags.c
$(OBJD)/undo.o: $(SRCD)/undo.c $(HFILES)
	$(CC) $(CFLAGS) -c -Fo$(OBJD)/undo.o $(SRCD)/undo.c
$(OBJD)/window.o: $(SRCD)/window.c $(HFILES)
	$(CC) $(CFLAGS) -c -Fo$(OBJD)/window.o $(SRCD)/window.c
$(OBJD)/word.o: $(SRCD)/word.c $(HFILES)
	$(CC) $(CFLAGS) -c -Fo$(OBJD)/word.o $(SRCD)/word.c
