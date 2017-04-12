#		MicroSoft C makefile for
#		MicroEMACS 3.12
#		written by Pierre Perret
#
# for Microsoft C compiler (6.0 or 7.0) on i386 or MIPS or ALPHA CPU
#
# define TARGET as C6, C7 or NT (or some other short suffix)
# for Windows/NT, use TARGET=NT
# other suffixes compile for Windows 3.1
#
# define DEBUG as 1 if Codeview/WinDebug symbolic information is desired
# if DEBUG is not defined, no debugging information is generated
#
# This makefile assumes the following directory hierarchy:
#     ---
#        |-- SRC    <-- source files (*.c)
#        |
#        |-- H      <-- include files (*.h)
#        |
#        |-- MSWIN --   <-- resource files (.dlg, .rc, .ico ...)
#        |           |
#        |           |-- MSC --   <-- where this makefile is located
#        |           |         |
#        |           |         |-- BIN  <-- where the executable ends up
#        |           |         |
#        |           |         |-- OBJ.C7
#        |           |         |
#        |           |         |-- OBJ.NT
#
# if TEMPOBJ is defined, the object files are stored under a directory
# (MEWIN.NT, MEWIN.C7 ...) under $TEMP. Otherwise, they are stored under
# the corresponding object directory (OBJ.NT, OBJ.C7 ...) in the
# MSWIN\MSC hierarchy


!IFNDEF DEBUG
DEBUG = 0
!ENDIF

!IFNDEF TARGET
TARGET = C7
!ENDIF

!IFNDEF CPU
!IF "$(TARGET)"=="NT"
CPU = i386
!ENDIF
!ENDIF

SRC = ..\..\SRC
INC = ..\..\H
RES = ..
BIN = BIN
!IFNDEF TEMPOBJ
OBJ = OBJ.$(TARGET)
!ELSE
OBJ = $(TEMP)\MEWIN.$(TARGET)
!ENDIF
PROJ = MEWIN$(TARGET)
PROG = $(BIN)\$(PROJ).exe

DEF_FILE = $(RES)\MEWIN.DEF
RES_FILE = $(OBJ)\MEWIN.RES
RC_FILE = $(RES)\MEWIN.RC
DLG_FILES = $(RES)\mswmodes.dlg $(RES)\mswmlh.dlg $(RES)\mswfonts.dlg\
	$(RES)\mswprg.dlg $(RES)\mswabout.dlg $(RES)\mswfile.dlg
ICO_FILES = $(RES)\mswapp.ico $(RES)\mswscr.ico $(RES)\mswwait.ico
CUR_FILES = $(RES)\mswcur.cur $(RES)\mswnot.cur $(RES)\mswgrin1.cur\
	$(RES)\mswgrin2.cur $(RES)\mswgrin3.cur $(RES)\mswgrin4.cur\
	$(RES)\mswgrin5.cur $(RES)\mswgrin6.cur $(RES)\mswgrin7.cur\
	$(RES)\mswgrin8.cur
BMP_FILES =
MNU_FILES = $(RES)\mswmenu.rc

!IF "$(TARGET)"=="NT"
!IF "$(CPU)"=="MIPS"
# MIPS specific build stuff
CPUTYPE=2
CC = cc
cvtobj = mip2coff
CFLAGS_G  = -std -G0 -O -EL -I. -I$(INC) -DMIPS=1 -DWIN32 -DWINNT=1 -D__cdecl= -D__export= $(C_FLAGS)
CFLAGS_D  =
CFLAGS_R  =
LFLAGS_G  = /SUBSYSTEM:windows /ENTRY:WinMainCRTStartup\
        /OUT:$(PROG) $(L_FLAGS)
LFLAGS_D  = /DEBUG:full
LFLAGS_R  =
LLIBS_G   = $(OBJ)\$(PROJ).lib $(LIB)\libcmt.lib $(LIB)\*.lib
LLIBS_R  =
LLIBS_D  =
!ELSE
!IF "$(CPU)"=="ALPHA"
# Alpha AXP build stuff
CPUTYPE=2
CC = acc
cvtobj = a2coff
CFLAGS_G  = -std -G0 -O -EL -I$(INC) -DALPHA=1 -DWIN32 -DWINNT=1 -D__cdecl=-D__export= $(C_FLAGS)
CFLAGS_D  =
CFLAGS_R  =
LFLAGS_G  = /SUBSYSTEM:windows /ENTRY:WinMainCRTStartup\
        /OUT:$(PROG) $(L_FLAGS)
LFLAGS_D  = /DEBUG:full
LFLAGS_R  =
LLIBS_G   = $(OBJ)\$(PROJ).lib $(LIB)\libcmt.lib $(LIB)\*.lib
LLIBS_R  =
LLIBS_D  =
!ELSE
# x86 build stuff
!if "$(CPU)"=="i386"
CC  = cl386
CFLAGS_G  = /I . /I $(INC) /G3 /Gd /BATCH /Di386=1 /DWIN32 /DWINNT=1 $(C_FLAGS)
CFLAGS_D  = /Od /Zi
CFLAGS_R  = /Os /Og
LFLAGS_G  = /SUBSYSTEM:windows /ENTRY:WinMainCRTStartup\
        /OUT:$(PROG) $(L_FLAGS)
LFLAGS_D  = /DEBUG:full /DEBUGTYPE:cv
LFLAGS_R  =
LLIBS_G   = $(OBJ)\$(PROJ).lib libc.lib ntdll.lib \
        kernel32.lib user32.lib gdi32.lib shell32.lib
LLIBS_R  =
LLIBS_D  =
!ELSE
!ERROR  Must specify CPU Environment Variable
!ERROR  (set CPU=i386 or set CPU=MIPS or set CPU=ALPHA)
!ENDIF
!ENDIF
!ENDIF
!ELSE
CC  = cl
CFLAGS_G  = /I . /I $(INC) /AL /G2 /Gx /GA /DWINVER=0x030a /Zp /BATCH
CFLAGS_D  = /Od /Zi
CFLAGS_R  = /Os /Og /Gs
LFLAGS_G  = /BATCH /ONERROR:NOEXE
LFLAGS_D  = /CO /NOF
LFLAGS_R  = /F
LLIBS_G  = LIBW.LIB SHELL.LIB
LLIBS_R  = /NOD:LLIBCE LLIBCEW.LIB
LLIBS_D  = /NOD:LLIBCE LLIBCEW.LIB
!ENDIF
MAPFILE_D  = $(OBJ)\$(PROJ).map
MAPFILE_R  = NUL
LINKER	= link
LRF  = echo > NUL
RC  = rc
RCFLAGS2  = /30 /t
CVFLAGS  = /25

H_FILES = estruct.h $(INC)\eproto.h $(INC)\edef.h $(INC)\elang.h\
	$(INC)\english.h

OBJ_FILES = $(OBJ)\BASIC.obj $(OBJ)\BIND.obj $(OBJ)\BUFFER.obj $(OBJ)\CHAR.obj\
        $(OBJ)\CRYPT.obj $(OBJ)\DISPLAY.obj $(OBJ)\DOLOCK.obj $(OBJ)\EVAL.obj\
        $(OBJ)\EXEC.obj $(OBJ)\FILE.obj $(OBJ)\FILEIO.obj $(OBJ)\INPUT.obj\
        $(OBJ)\ISEARCH.obj $(OBJ)\LINE.obj $(OBJ)\LOCK.obj $(OBJ)\MAIN.obj\
        $(OBJ)\MOUSE.obj $(OBJ)\MSWDISP.obj $(OBJ)\MSWDRV.obj\
        $(OBJ)\MSWEMACS.obj $(OBJ)\MSWEXEC.obj $(OBJ)\MSWFILE.obj\
        $(OBJ)\MSWFONT.obj $(OBJ)\MSWINPUT.obj $(OBJ)\MSWMEM.obj\
        $(OBJ)\MSWMENU.obj $(OBJ)\MSWSYS.obj $(OBJ)\RANDOM.obj\
        $(OBJ)\REGION.obj $(OBJ)\REPLACE.obj $(OBJ)\SCREEN.obj\
        $(OBJ)\SEARCH.obj $(OBJ)\WINDOW.obj $(OBJ)\WORD.obj

all: $(PROG)

SUFFIXES:
SUFFIXES: .obj .c
SUFFIXES: .obj .c

$(OBJ)\BASIC.obj : $(SRC)\BASIC.C $(H_FILES)

$(OBJ)\BIND.obj : $(SRC)\BIND.C  $(H_FILES) $(INC)\epath.h

$(OBJ)\BUFFER.obj : $(SRC)\BUFFER.C $(H_FILES)

$(OBJ)\CHAR.obj : $(SRC)\CHAR.C $(H_FILES)

$(OBJ)\CRYPT.obj : $(SRC)\CRYPT.C $(H_FILES)

$(OBJ)\DISPLAY.obj : $(SRC)\DISPLAY.C $(H_FILES)

$(OBJ)\DOLOCK.obj : $(SRC)\DOLOCK.C $(H_FILES)

$(OBJ)\EVAL.obj : $(SRC)\EVAL.C $(H_FILES) $(INC)\evar.h

$(OBJ)\EXEC.obj : $(SRC)\EXEC.C $(H_FILES)

$(OBJ)\FILE.obj : $(SRC)\FILE.C $(H_FILES)

$(OBJ)\FILEIO.obj : $(SRC)\FILEIO.C $(H_FILES)

$(OBJ)\INPUT.obj : $(SRC)\INPUT.C $(H_FILES)

$(OBJ)\ISEARCH.obj : $(SRC)\ISEARCH.C $(H_FILES)

$(OBJ)\LINE.obj : $(SRC)\LINE.C $(H_FILES)

$(OBJ)\LOCK.obj : $(SRC)\LOCK.C $(H_FILES)

$(OBJ)\MAIN.obj : $(SRC)\MAIN.C $(H_FILES) $(INC)\efunc.h $(INC)\ebind.h

$(OBJ)\MOUSE.obj : $(SRC)\MOUSE.C $(H_FILES)

$(OBJ)\MSWDISP.obj : $(SRC)\MSWDISP.C $(H_FILES) $(INC)\mswin.h $(INC)\mswrid.h

$(OBJ)\MSWDRV.obj : $(SRC)\MSWDRV.C $(H_FILES) $(INC)\mswin.h $(INC)\mswrid.h

$(OBJ)\MSWEMACS.obj : $(SRC)\MSWEMACS.C $(H_FILES) $(INC)\mswin.h\
	$(INC)\mswmenu.h $(INC)\mswrid.h

$(OBJ)\MSWEXEC.obj : $(SRC)\MSWEXEC.C $(H_FILES) $(INC)\mswin.h $(INC)\mswrid.h

$(OBJ)\MSWFILE.obj : $(SRC)\MSWFILE.C $(H_FILES) $(INC)\mswin.h $(INC)\mswrid.h

$(OBJ)\MSWFONT.obj : $(SRC)\MSWFONT.C $(H_FILES) $(INC)\mswin.h $(INC)\mswrid.h

$(OBJ)\MSWINPUT.obj : $(SRC)\MSWINPUT.C $(H_FILES) $(INC)\mswin.h\
	$(INC)\mswrid.h

$(OBJ)\MSWMEM.obj : $(SRC)\MSWMEM.C $(H_FILES) $(INC)\mswin.h $(INC)\mswrid.h

$(OBJ)\MSWMENU.obj : $(SRC)\MSWMENU.C $(H_FILES) $(INC)\mswin.h\
	$(INC)\mswmenu.h $(INC)\mswhelp.h $(INC)\mswrid.h

$(OBJ)\MSWSYS.obj : $(SRC)\MSWSYS.C $(H_FILES) $(INC)\mswin.h $(INC)\mswrid.h

$(OBJ)\RANDOM.obj : $(SRC)\RANDOM.C $(H_FILES)

$(OBJ)\REGION.obj : $(SRC)\REGION.C $(H_FILES)

$(OBJ)\REPLACE.obj : $(SRC)\REPLACE.C $(H_FILES)

$(OBJ)\SCREEN.obj : $(SRC)\SCREEN.C $(H_FILES)

$(OBJ)\SEARCH.obj : $(SRC)\SEARCH.C $(H_FILES)

$(OBJ)\WINDOW.obj : $(SRC)\WINDOW.C $(H_FILES)

$(OBJ)\WORD.obj : $(SRC)\WORD.C $(H_FILES)

$(RES_FILE) : $(RC_FILE) $(INC)\mswin.h $(INC)\mswrid.h $(INC)\mswmenu.h\
        $(DLG_FILES) $(ICO_FILES) $(CUR_FILES) $(BMP_FILES) $(MENU_FILES)
        $(RC) /i $(INC) /i $(RES) /r /fo $(RES_FILE) $(RC_FILE)

$(PROG) : $(OBJ) $(BIN) $(OBJ_FILES) $(RES_FILE)
!IF $(DEBUG)
!IF "$(TARGET)"=="NT"
	$(LRF) @<<$(OBJ)\$(PROJ).lrf
$(OBJ_FILES)
$(LIBS) $(LLIBS_G) $(LLIBS_D)
$(LFLAGS_G) $(LFLAGS_D);
<<
!ELSE
	$(LRF) @<<$(OBJ)\$(PROJ).lrf
$(RT_OBJS: = +^
) $(OBJ_FILES: = +^
)
$@
$(MAPFILE_D)
$(LIBS: = +^
) +
$(LLIBS_G: = +^
) +
$(LLIBS_D: = +^
)
$(DEF_FILE) $(LFLAGS_G) $(LFLAGS_D);
<<
!ENDIF
!ELSE
!IF "$(TARGET)"=="NT"
	$(LRF) @<<$(OBJ)\$(PROJ).lrf
$(OBJ_FILES)
$(LIBS) $(LLIBS_G) $(LLIBS_R)
$(LFLAGS_G) $(LFLAGS_R);
<<
!ELSE
	$(LRF) @<<$(OBJ)\$(PROJ).lrf
$(RT_OBJS: = +^
) $(OBJ_FILES: = +^
)
$@
$(MAPFILE_R)
$(LIBS: = +^
) +
$(LLIBS_G: = +^
) +
$(LLIBS_R: = +^
)
$(DEF_FILE) $(LFLAGS_G) $(LFLAGS_R);
<<
!ENDIF
!ENDIF
!IF "$(TARGET)"=="NT"
        cvtres -$(CPU) $(RES_FILE)
        LIB /OUT:$(OBJ)\$(PROJ).lib /DEF:$(DEF_FILE) /MACHINE:$(CPU)
	$(LINKER) $(OBJ)\MEWIN.OBJ @$(OBJ)\$(PROJ).lrf
!ELSE
	$(LINKER) @$(OBJ)\$(PROJ).lrf
        $(RC) $(RCFLAGS2) $(RES_FILE) $(PROG)
!ENDIF

$(OBJ):
        mkdir $(OBJ)

$(BIN):
        mkdir $(BIN)

{$(SRC)}.c{$(OBJ)}.obj :
!IF "$(CPU)"=="MIPS" || "$(CPU)"=="ALPHA"
!IF $(DEBUG)
	@$(CC) -c $(CFLAGS_G) $(CFLAGS_D) -o $@ $<
!ELSE
	@$(CC) -c $(CFLAGS_G) $(CFLAGS_R) -o $@ $<
!ENDIF
        @$(cvtobj) $@
!ELSE
!IF $(DEBUG)
	@$(CC) /c $(CFLAGS_G) $(CFLAGS_D) /Fo$@ $<
!ELSE
	@$(CC) /c $(CFLAGS_G) $(CFLAGS_R) /Fo$@ $<
!ENDIF
!ENDIF
