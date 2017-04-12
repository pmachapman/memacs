#	MicroSoft Windows NT makefile for
#		MicroEMACS 3.12
#		(C)Copyright 1992 by Daniel Lawrence
#		all rights reserved
#               By Walter Warniaha

# modify the following lines with the proper relative directories

OBJD = OBJ  			# object file directory
SRCD = ..\src          		# source file directory
HDIR = ..\h            		# header file directory
INCD = c:\mstools\h            	# system include directory

# Nmake macros for building Windows 32-Bit apps

# Win32 NMAKE definitions


!IF "$(CPU)" == "i386"

# Debug switches are default for current release
#
# These switches allow for source level debugging
# with NTSD for local and global variables.


CPUTYPE=1
cdebug = -Zi -Od
cvtomfdebug = -g

cc = cl386

#First clags is for debugging, second one is for not
#cflags	= -c -G3d -W3 -D_X86_=1 $(cdebug) -D_MT /J /I. /I$(HDIR) /I$(INCD)
cflags	= -c -G3d -W3 -D_X86_=1 -D_MT /J /I. /I$(HDIR) /I$(INCD)

#cvtobj = cvtomf $(cvtomfdebug)
cvtobj = REM MIPS only version
!ENDIF

!IF "$(CPU)" == "MIPS"
#declarations for use on self hosted MIPS box.

CPUTYPE=2
cc = cc
cflags	= -c -std -G0 -O -o $(*B).obj -EL -DMIPS=1
cvtobj = mip2coff
cvtdebug =
!ENDIF

!IFNDEF CPUTYPE
!ERROR  Must specify CPU Environment Variable (i386 or MIPS )
!ENDIF


#Universal declaration

cvars = -DWIN32
linkdebug = -debug:full -debugtype:cv
link = link32 $(linkdebug)


# link flags - must be specified after $(link)
#
# conflags : creating a character based console application
# guiflags : creating a GUI based "Windows" application

conflags =  -subsystem:console -entry:mainCRTStartup
guiflags =  -subsystem:windows -entry:WinMainCRTStartup

# Link libraries - system import and C runtime libraries
#
# conlibs : libraries to link with for a console application
# guilibs : libraries to link with for a "Windows" application
#
# note : $(LIB) is set in environment variables


conlibs = libcmt.lib ntdll.lib kernel32.lib

guilibs    = libc.lib ntdll.lib kernel32.lib \
	     user32.lib gdi32.lib winspool.lib \
	     comdlg32.lib

# This line allows NMAKE to work as well
PROGRAM = emacs

# list of header files

HFILES = $(HDIR)\estruct.h $(HDIR)\edef.h $(HDIR)\efunc.h $(HDIR)\epath.h $(HDIR)\ebind.h $(HDIR)\eproto.h

#list of source files

F1 = $(OBJD)\main.obj $(OBJD)\basic.obj $(OBJD)\bind.obj $(OBJD)\buffer.obj
F2 = $(OBJD)\char.obj $(OBJD)\crypt.obj $(OBJD)\display.obj
F3 = $(OBJD)\eval.obj $(OBJD)\exec.obj $(OBJD)\file.obj
F4 = $(OBJD)\fileio.obj $(OBJD)\nt.obj $(OBJD)\input.obj $(OBJD)\keyboard.obj
F5 = $(OBJD)\isearch.obj $(OBJD)\line.obj $(OBJD)\lock.obj $(OBJD)\dolock.obj
F6 = $(OBJD)\mouse.obj $(OBJD)\ntconio.obj $(OBJD)\screen.obj $(OBJD)\replace.obj
F7 = $(OBJD)\random.obj $(OBJD)\region.obj $(OBJD)\search.obj $(OBJD)\tags.obj
F8 = $(OBJD)\window.obj $(OBJD)\word.obj

# product list

emacs.exe: $(F1) $(F2) $(F3) $(F4) $(F5) $(F6) $(F7) $(F8)
    $(link) $(conflags) -out:$(PROGRAM).exe -stack:18000 $(F1) $(F2) $(F3) $(F4) $(F5) $(F6) $(F7) $(F8) $(conlibs)

# compilation rules

$(OBJD)\basic.obj: $(SRCD)\basic.c $(HFILES)
    $(cc) $(cflags) $(cvars) /Fo$(OBJD)\basic.obj $(SRCD)\basic.c
    $(cvtobj) $(OBJD)\basic.obj
$(OBJD)\bind.obj: $(SRCD)\bind.c $(HFILES)
    $(cc) $(cflags) $(cvars) /Fo$(OBJD)\bind.obj $(SRCD)\bind.c
    $(cvtobj) $(OBJD)\bind.obj
$(OBJD)\buffer.obj: $(SRCD)\buffer.c $(HFILES)
    $(cc) $(cflags) $(cvars) /Fo$(OBJD)\buffer.obj $(SRCD)\buffer.c
    $(cvtobj) $(OBJD)\buffer.obj
$(OBJD)\char.obj: $(SRCD)\char.c $(HFILES)
    $(cc) $(cflags) $(cvars) /Fo$(OBJD)\char.obj $(SRCD)\char.c
    $(cvtobj) $(OBJD)\char.obj
$(OBJD)\crypt.obj: $(SRCD)\crypt.c $(HFILES)
    $(cc) $(cflags) $(cvars) /Fo$(OBJD)\crypt.obj $(SRCD)\crypt.c
    $(cvtobj) $(OBJD)\crypt.obj
$(OBJD)\cmdfix.obj: $(SRCD)\cmdfix.c $(HFILES)
    $(cc) $(cflags) $(cvars) /Fo$(OBJD)\cmdfix.obj $(SRCD)\cmdfix.c
    $(cvtobj) $(OBJD)\cmdfix.obj
$(OBJD)\display.obj: $(SRCD)\display.c $(HFILES)
    $(cc) $(cflags) $(cvars) /Fo$(OBJD)\display.obj $(SRCD)\display.c
    $(cvtobj) $(OBJD)\display.obj
$(OBJD)\dolock.obj: $(SRCD)\dolock.c $(HFILES)
    $(cc) $(cflags) $(cvars) /Fo$(OBJD)\dolock.obj $(SRCD)\dolock.c
    $(cvtobj) $(OBJD)\dolock.obj
$(OBJD)\eval.obj: $(SRCD)\eval.c $(HFILES) $(HDIR)\evar.h
    $(cc) $(cflags) $(cvars) /Fo$(OBJD)\eval.obj $(SRCD)\eval.c
    $(cvtobj) $(OBJD)\eval.obj
$(OBJD)\exec.obj: $(SRCD)\exec.c $(HFILES)
    $(cc) $(cflags) $(cvars) /Fo$(OBJD)\exec.obj $(SRCD)\exec.c
    $(cvtobj) $(OBJD)\exec.obj
$(OBJD)\file.obj: $(SRCD)\file.c $(HFILES)
    $(cc) $(cflags) $(cvars) /Fo$(OBJD)\file.obj $(SRCD)\file.c
    $(cvtobj) $(OBJD)\file.obj
$(OBJD)\fileio.obj: $(SRCD)\fileio.c $(HFILES)
    $(cc) $(cflags) $(cvars) /Fo$(OBJD)\fileio.obj $(SRCD)\fileio.c
    $(cvtobj) $(OBJD)\fileio.obj
$(OBJD)\hp150.obj: $(SRCD)\hp150.c $(HFILES)
    $(cc) $(cflags) $(cvars) /Fo$(OBJD)\hp150.obj $(SRCD)\hp150.c
    $(cvtobj) $(OBJD)\hp150.obj
$(OBJD)\ibmpc.obj: $(SRCD)\ibmpc.c $(HFILES)
    $(cc) $(cflags) $(cvars) /Fo$(OBJD)\ibmpc.obj $(SRCD)\ibmpc.c
    $(cvtobj) $(OBJD)\ibmpc.obj
$(OBJD)\input.obj: $(SRCD)\input.c $(HFILES)
    $(cc) $(cflags) $(cvars) /Fo$(OBJD)\input.obj $(SRCD)\input.c
    $(cvtobj) $(OBJD)\input.obj
$(OBJD)\isearch.obj: $(SRCD)\isearch.c $(HFILES)
    $(cc) $(cflags) $(cvars) /Fo$(OBJD)\isearch.obj $(SRCD)\isearch.c
    $(cvtobj) $(OBJD)\isearch.obj
$(OBJD)\line.obj: $(SRCD)\line.c $(HFILES)
    $(cc) $(cflags) $(cvars) /Fo$(OBJD)\line.obj $(SRCD)\line.c
    $(cvtobj) $(OBJD)\line.obj
$(OBJD)\keyboard.obj: $(SRCD)\keyboard.c $(HFILES)
    $(cc) $(cflags) $(cvars) /Fo$(OBJD)\keyboard.obj $(SRCD)\keyboard.c
    $(cvtobj) $(OBJD)\keyboard.obj
$(OBJD)\lock.obj: $(SRCD)\lock.c $(HFILES)
    $(cc) $(cflags) $(cvars) /Fo$(OBJD)\lock.obj $(SRCD)\lock.c
    $(cvtobj) $(OBJD)\lock.obj
$(OBJD)\main.obj: $(SRCD)\main.c $(HFILES)
    $(cc) $(cflags) $(cvars) /Fo$(OBJD)\main.obj $(SRCD)\main.c
    $(cvtobj) $(OBJD)\main.obj
$(OBJD)\mouse.obj: $(SRCD)\mouse.c $(HFILES)
    $(cc) $(cflags) $(cvars) /Fo$(OBJD)\mouse.obj $(SRCD)\mouse.c
    $(cvtobj) $(OBJD)\mouse.obj
$(OBJD)\msdos.obj: $(SRCD)\msdos.c $(HFILES)
    $(cc) $(cflags) $(cvars) /Fo$(OBJD)\msdos.obj $(SRCD)\msdos.c
    $(cvtobj) $(OBJD)\msdos.obj
$(OBJD)\nt.obj: $(SRCD)\nt.c $(HFILES)
    $(cc) $(cflags) $(cvars) /Fo$(OBJD)\nt.obj $(SRCD)\nt.c
    $(cvtobj) $(OBJD)\nt.obj
$(OBJD)\ntconio.obj: $(SRCD)\ntconio.c $(HFILES)
    $(cc) $(cflags) $(cvars) /Fo$(OBJD)\ntconio.obj $(SRCD)\ntconio.c
    $(cvtobj) $(OBJD)\ntconio.obj
$(OBJD)\random.obj: $(SRCD)\random.c $(HFILES)
    $(cc) $(cflags) $(cvars) /Fo$(OBJD)\random.obj $(SRCD)\random.c
    $(cvtobj) $(OBJD)\random.obj
$(OBJD)\region.obj: $(SRCD)\region.c $(HFILES)
    $(cc) $(cflags) $(cvars) /Fo$(OBJD)\region.obj $(SRCD)\region.c
    $(cvtobj) $(OBJD)\region.obj
$(OBJD)\search.obj: $(SRCD)\search.c $(HFILES)
    $(cc) $(cflags) $(cvars) /Fo$(OBJD)\search.obj $(SRCD)\search.c
    $(cvtobj) $(OBJD)\search.obj
$(OBJD)\replace.obj: $(SRCD)\replace.c $(HFILES)
    $(cc) $(cflags) $(cvars) /Fo$(OBJD)\replace.obj $(SRCD)\replace.c
    $(cvtobj) $(OBJD)\replace.obj
$(OBJD)\screen.obj: $(SRCD)\screen.c $(HFILES)
    $(cc) $(cflags) $(cvars) /Fo$(OBJD)\screen.obj $(SRCD)\screen.c
    $(cvtobj) $(OBJD)\screen.obj
$(OBJD)\tags.obj: $(SRCD)\tags.c $(HFILES)
    $(cc) $(cflags) $(cvars) /Fo$(OBJD)\tags.obj $(SRCD)\tags.c
    $(cvtobj) $(OBJD)\tags.obj
$(OBJD)\window.obj: $(SRCD)\window.c $(HFILES)
    $(cc) $(cflags) $(cvars) /Fo$(OBJD)\window.obj $(SRCD)\window.c
    $(cvtobj) $(OBJD)\window.obj
$(OBJD)\word.obj: $(SRCD)\word.c $(HFILES)
    $(cc) $(cflags) $(cvars) /Fo$(OBJD)\word.obj $(SRCD)\word.c
    $(cvtobj) $(OBJD)\word.obj

clean::
    del *.obj *.exe *.bak
