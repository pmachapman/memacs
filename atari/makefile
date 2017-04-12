#	ST/MWC makefile for
#		MicroEMACS 3.12
#		(C)Copyright 1993 by Daniel Lawrence
#		all rights reserved

# modify the following lines with the proper relative directories

# object file directory
OBJD = obj
# source file directory
SRCD = ..\src
# header file directory
HDIR = ..\h

# options and arguments to the C compiler
# PeepHole optimization and don't report unused variables!
CFLAGS=-I$(HDIR) -VPEEP -VSUVAR

# list of header files

HFILES = $(HDIR)\estruct.h $(HDIR)\edef.h $(HDIR)\efunc.h $(HDIR)\epath.h $(HDIR)\ebind.h $(HDIR)\eproto.h

#list of object files

F1 = $(OBJD)\basic.o $(OBJD)\bind.o $(OBJD)\buffer.o
F2 = $(OBJD)\char.o $(OBJD)\crypt.o $(OBJD)\display.o
F3 = $(OBJD)\eval.o $(OBJD)\exec.o $(OBJD)\file.o
F4 = $(OBJD)\fileio.o $(OBJD)\input.o $(OBJD)\isearch.o $(OBJD)\line.o
F5 = $(OBJD)\main.o $(OBJD)\mouse.o
#F5 = $(OBJD)\lock.o $(OBJD)\dolock.o $(OBJD)\main.o $(OBJD)\mouse.o
F6 = $(OBJD)\random.o $(OBJD)\region.o $(OBJD)\replace.o $(OBJD)\screen.o
F7 = $(OBJD)\search.o $(OBJD)\st52.o $(OBJD)\stmouse.o
F8 = $(OBJD)\tags.o $(OBJD)\tos.o $(OBJD)\window.o $(OBJD)\word.o

OFILES =	$(F1) $(F2) $(F3) $(F4) $(F5) $(F6) $(F7) $(F8)

# product list

emacs.prg:	$(OFILES)
	$(CC) $(CFLAGS) -o emacs.prg $(OFILES) -laes -lvdi
	strip emacs.prg

$(OBJD)\basic.o: $(SRCD)\basic.c $(HFILES)
	$(CC) $(CFLAGS) -o $(OBJD)\basic.o -c $(SRCD)\basic.c
$(OBJD)\bind.o: $(SRCD)\bind.c $(HFILES)
	$(CC) $(CFLAGS) -o $(OBJD)\bind.o -c $(SRCD)\bind.c
$(OBJD)\buffer.o: $(SRCD)\buffer.c $(HFILES)
	$(CC) $(CFLAGS) -o $(OBJD)\buffer.o -c $(SRCD)\buffer.c
$(OBJD)\char.o: $(SRCD)\char.c $(HFILES)
	$(CC) $(CFLAGS) -o $(OBJD)\char.o -c $(SRCD)\char.c
$(OBJD)\crypt.o: $(SRCD)\crypt.c $(HFILES)
	$(CC) $(CFLAGS) -o $(OBJD)\crypt.o -c $(SRCD)\crypt.c
$(OBJD)\display.o: $(SRCD)\display.c $(HFILES)
	$(CC) $(CFLAGS) -o $(OBJD)\display.o -c $(SRCD)\display.c
$(OBJD)\eval.o: $(SRCD)\eval.c $(HFILES) $(HDIR)\evar.h
	$(CC) $(CFLAGS) -o $(OBJD)\eval.o -c $(SRCD)\eval.c
$(OBJD)\exec.o: $(SRCD)\exec.c $(HFILES)
	$(CC) $(CFLAGS) -o $(OBJD)\exec.o -c $(SRCD)\exec.c
$(OBJD)\file.o: $(SRCD)\file.c $(HFILES)
	$(CC) $(CFLAGS) -o $(OBJD)\file.o -c $(SRCD)\file.c
$(OBJD)\fileio.o: $(SRCD)\fileio.c $(HFILES)
	$(CC) $(CFLAGS) -o $(OBJD)\fileio.o -c $(SRCD)\fileio.c
$(OBJD)\input.o: $(SRCD)\input.c $(HFILES)
	$(CC) $(CFLAGS) -o $(OBJD)\input.o -c $(SRCD)\input.c
$(OBJD)\isearch.o: $(SRCD)\isearch.c $(HFILES)
	$(CC) $(CFLAGS) -o $(OBJD)\isearch.o -c $(SRCD)\isearch.c
$(OBJD)\line.o: $(SRCD)\line.c $(HFILES)
	$(CC) $(CFLAGS) -o $(OBJD)\line.o -c $(SRCD)\line.c
$(OBJD)\lock.o: $(SRCD)\lock.c $(HFILES)
	$(CC) $(CFLAGS) -o $(OBJD)\lock.o -c $(SRCD)\lock.c
$(OBJD)\dolock.o: $(SRCD)\dolock.c $(HFILES)
	$(CC) $(CFLAGS) -o $(OBJD)\dolock.o -c $(SRCD)\dolock.c
$(OBJD)\main.o: $(SRCD)\main.c $(HFILES)
	$(CC) $(CFLAGS) -o $(OBJD)\main.o -c $(SRCD)\main.c
$(OBJD)\mouse.o: $(SRCD)\mouse.c $(HFILES)
	$(CC) $(CFLAGS) -o $(OBJD)\mouse.o -c $(SRCD)\mouse.c
$(OBJD)\random.o: $(SRCD)\random.c $(HFILES)
	$(CC) $(CFLAGS) -o $(OBJD)\random.o -c $(SRCD)\random.c
$(OBJD)\region.o: $(SRCD)\region.c $(HFILES)
	$(CC) $(CFLAGS) -o $(OBJD)\region.o -c $(SRCD)\region.c
$(OBJD)\replace.o: $(SRCD)\replace.c $(HFILES)
	$(CC) $(CFLAGS) -o $(OBJD)\replace.o -c $(SRCD)\replace.c
$(OBJD)\screen.o: $(SRCD)\screen.c $(HFILES)
	$(CC) $(CFLAGS) -o $(OBJD)\screen.o -c $(SRCD)\screen.c
$(OBJD)\search.o: $(SRCD)\search.c $(HFILES)
	$(CC) $(CFLAGS) -o $(OBJD)\search.o -c $(SRCD)\search.c
$(OBJD)\st52.o: $(SRCD)\st52.c $(HFILES)
	$(CC) $(CFLAGS) -o $(OBJD)\st52.o -c $(SRCD)\st52.c
$(OBJD)\stmouse.o: $(SRCD)\stmouse.s
	$(AS) $(ASFLAGS) -o $(OBJD)\stmouse.o $(SRCD)\stmouse.s
$(OBJD)\tags.o: $(SRCD)\tags.c $(HFILES)
	$(CC) $(CFLAGS) -o $(OBJD)\tags.o -c $(SRCD)\tags.c
$(OBJD)\tos.o: $(SRCD)\tos.c $(HFILES)
	$(CC) $(CFLAGS) -o $(OBJD)\tos.o -c $(SRCD)\tos.c
$(OBJD)\window.o: $(SRCD)\window.c $(HFILES)
	$(CC) $(CFLAGS) -o $(OBJD)\window.o -c $(SRCD)\window.c
$(OBJD)\word.o: $(SRCD)\word.c $(HFILES)
	$(CC) $(CFLAGS) -o $(OBJD)\word.o -c $(SRCD)\word.c


