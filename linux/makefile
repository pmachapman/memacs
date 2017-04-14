#		Linux makefile for
#		MicroEMACS 4.00
#		(C)Copyright 1995 by Daniel Lawrence
#		all rights reserved

# modify the following lines with the proper relative directories

# object file directory
OBJD = .
# source file directory
SRCD = ../src
# header file directory
HDIR = ../h

# options and arguments to the C compiler

#CFLAGS += -I. -I$(HDIR)
CFLAGS = -O -I. -I$(HDIR)

#list of object files

F1 = $(OBJD)/abbrev.o $(OBJD)/basic.o $(OBJD)/bind.o $(OBJD)/buffer.o
F2 = $(OBJD)/char.o $(OBJD)/crypt.o $(OBJD)/display.o $(OBJD)/dolock.o
F3 = $(OBJD)/eval.o $(OBJD)/exec.o $(OBJD)/file.o $(OBJD)/fileio.o
F4 = $(OBJD)/input.o $(OBJD)/isearch.o $(OBJD)/keyboard.o $(OBJD)/line.o
F5 = $(OBJD)/lock.o $(OBJD)/main.o $(OBJD)/mouse.o $(OBJD)/screen.o
F6 = $(OBJD)/random.o $(OBJD)/region.o $(OBJD)/search.o $(OBJD)/replace.o
F7 = $(OBJD)/tags.o $(OBJD)/undo.o $(OBJD)/unix.o $(OBJD)/window.o $(OBJD)/word.o

# product list

all:	emacs

OFILES =	$(F1) $(F2) $(F3) $(F4) $(F5) $(F6) $(F7)

emacs:	$(OFILES)
		$(CC) $(CFLAGS) $(OFILES) -lncurses -lc -o emacs

$(OBJD)/abbrev.o: $(SRCD)/abbrev.c
	$(CC) $(CFLAGS) -c -o $(OBJD)/abbrev.o $(SRCD)/abbrev.c
$(OBJD)/basic.o: $(SRCD)/basic.c
	$(CC) $(CFLAGS) -c -o $(OBJD)/basic.o $(SRCD)/basic.c
$(OBJD)/bind.o: $(SRCD)/bind.c
	$(CC) $(CFLAGS) -c -o $(OBJD)/bind.o $(SRCD)/bind.c
$(OBJD)/buffer.o: $(SRCD)/buffer.c
	$(CC) $(CFLAGS) -c -o $(OBJD)/buffer.o $(SRCD)/buffer.c
$(OBJD)/char.o: $(SRCD)/char.c
	$(CC) $(CFLAGS) -c -o $(OBJD)/char.o $(SRCD)/char.c
$(OBJD)/crypt.o: $(SRCD)/crypt.c
	$(CC) $(CFLAGS) -c -o $(OBJD)/crypt.o $(SRCD)/crypt.c
$(OBJD)/cmdfix.o: $(SRCD)/cmdfix.c
	$(CC) $(CFLAGS) -c -o $(OBJD)/cmdfix.o $(SRCD)/cmdfix.c
$(OBJD)/display.o: $(SRCD)/display.c
	$(CC) $(CFLAGS) -c -o $(OBJD)/display.o $(SRCD)/display.c
$(OBJD)/dolock.o: $(SRCD)/dolock.c
	$(CC) $(CFLAGS) -c -o $(OBJD)/dolock.o $(SRCD)/dolock.c
$(OBJD)/eval.o: $(SRCD)/eval.c $(HDIR)/evar.h
	$(CC) $(CFLAGS) -c -o $(OBJD)/eval.o $(SRCD)/eval.c
$(OBJD)/exec.o: $(SRCD)/exec.c
	$(CC) $(CFLAGS) -c -o $(OBJD)/exec.o $(SRCD)/exec.c
$(OBJD)/file.o: $(SRCD)/file.c
	$(CC) $(CFLAGS) -c -o $(OBJD)/file.o $(SRCD)/file.c
$(OBJD)/fileio.o: $(SRCD)/fileio.c
	$(CC) $(CFLAGS) -c -o $(OBJD)/fileio.o $(SRCD)/fileio.c
$(OBJD)/input.o: $(SRCD)/input.c
	$(CC) $(CFLAGS) -c -o $(OBJD)/input.o $(SRCD)/input.c
$(OBJD)/isearch.o: $(SRCD)/isearch.c
	$(CC) $(CFLAGS) -c -o $(OBJD)/isearch.o $(SRCD)/isearch.c
$(OBJD)/keyboard.o: $(SRCD)/keyboard.c
	$(CC) $(CFLAGS) -c -o $(OBJD)/keyboard.o $(SRCD)/keyboard.c
$(OBJD)/line.o: $(SRCD)/line.c
	$(CC) $(CFLAGS) -c -o $(OBJD)/line.o $(SRCD)/line.c
$(OBJD)/lock.o: $(SRCD)/lock.c
	$(CC) $(CFLAGS) -c -o $(OBJD)/lock.o $(SRCD)/lock.c
$(OBJD)/main.o: $(SRCD)/main.c
	$(CC) $(CFLAGS) -c -o $(OBJD)/main.o $(SRCD)/main.c
$(OBJD)/mouse.o: $(SRCD)/mouse.c
	$(CC) $(CFLAGS) -c -o $(OBJD)/mouse.o $(SRCD)/mouse.c
$(OBJD)/unix.o: $(SRCD)/unix.c
	$(CC) $(CFLAGS) -c -o $(OBJD)/unix.o $(SRCD)/unix.c
$(OBJD)/random.o: $(SRCD)/random.c
	$(CC) $(CFLAGS) -c -o $(OBJD)/random.o $(SRCD)/random.c
$(OBJD)/region.o: $(SRCD)/region.c
	$(CC) $(CFLAGS) -c -o $(OBJD)/region.o $(SRCD)/region.c
$(OBJD)/screen.o: $(SRCD)/screen.c
	$(CC) $(CFLAGS) -c -o $(OBJD)/screen.o $(SRCD)/screen.c
$(OBJD)/search.o: $(SRCD)/search.c
	$(CC) $(CFLAGS) -c -o $(OBJD)/search.o $(SRCD)/search.c
$(OBJD)/replace.o: $(SRCD)/replace.c
	$(CC) $(CFLAGS) -c -o $(OBJD)/replace.o $(SRCD)/replace.c
$(OBJD)/tags.o: $(SRCD)/tags.c
	$(CC) $(CFLAGS) -c -o $(OBJD)/tags.o $(SRCD)/tags.c
$(OBJD)/undo.o: $(SRCD)/undo.c
	$(CC) $(CFLAGS) -c -o $(OBJD)/undo.o $(SRCD)/undo.c
$(OBJD)/window.o: $(SRCD)/window.c
	$(CC) $(CFLAGS) -c -o $(OBJD)/window.o $(SRCD)/window.c
$(OBJD)/word.o: $(SRCD)/word.c
	$(CC) $(CFLAGS) -c -o $(OBJD)/word.o $(SRCD)/word.c
