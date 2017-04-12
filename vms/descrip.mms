!
! MMS file to compile and link MicroEmacs on the VAX/VMS
! operating system.
!

!
! Pick the options you want when you make MicroEmacs.
!

!copts=		/debug/nolist/nooptim
copts=		/nolist
!lopts=		/map/cross
lopts=		/map/brief

!
! The header file epath.h is used only by
! bind.c at this writing, so it appears
! on bind.c's dependancy line only.
!
hfiles=		ebind.h edef.h-
		efunc.h evar.h-
		estruct.h-

ofiles=		basic.obj,   bind.obj,   buffer.obj,-
		char.obj,    crypt.obj,-
		display.obj,-
		eval.obj,    exec.obj,-
		file.obj,    fileio.obj,-
		input.obj,   isearch.obj,-
		line.obj,    lock.obj,-
		main.obj,    mouse.obj,-
		random.obj,  region.obj,-
		screen.obj,  search.obj,  replace.obj, smg.obj,-
		tags.obj,-
		vms.obj,-
		window.obj,  word.obj-


!
! The choice to link with sys$share is made in
! the option file me.opt (you can't do it on
! the command line).  If you don't want to
! link sharable, change
!	sys$share:vaxcrtl/share
! to
!	sys$library:vaxcrtl/lib
!
me.exe : $(ofiles) descrip.mms me.opt
	LINK $(lopts)	me/option

basic.obj : basic.c $(HFILES)
    CC $(copts) basic
bind.obj : bind.c epath.h $(HFILES)
    CC $(copts) bind
buffer.obj : buffer.c $(HFILES)
    CC $(copts) buffer
char.obj : char.c $(HFILES)
    CC $(copts) char
crypt.obj : crypt.c $(HFILES)
    CC $(copts) crypt
display.obj : display.c $(HFILES)
    CC $(copts) display
eval.obj : eval.c $(HFILES)
    CC $(copts) eval
exec.obj : exec.c $(HFILES)
    CC $(copts) exec
file.obj : file.c $(HFILES)
    CC $(copts) file
fileio.obj : fileio.c $(HFILES)
    CC $(copts) fileio
input.obj : input.c $(HFILES)
    CC $(copts) input
isearch.obj : isearch.c $(HFILES)
    CC $(copts) isearch
line.obj : line.c $(HFILES)
    CC $(copts) line
lock.obj : lock.c $(HFILES)
    CC $(copts) lock
main.obj : main.c $(HFILES)
    CC $(copts) main
mouse.obj : mouse.c $(HFILES)
    CC $(copts) mouse
random.obj : random.c $(HFILES)
    CC $(copts) random
region.obj : region.c $(HFILES)
    CC $(copts) region
screen.obj : screen.c $(HFILES)
    CC $(copts) screen
search.obj : search.c $(HFILES)
    CC $(copts) search
replace.obj : replace.c $(HFILES)
    CC $(copts) replace
vms.obj : vms.c $(HFILES)
    CC $(copts) vms
smg.obj : smg.c $(HFILES)
    CC $(copts) smg
window.obj : window.c $(HFILES)
    CC $(copts) window
word.obj : word.c $(HFILES)
    CC $(copts) word
