;	ERROR.CMD:	Parse error files
;			using MicroEMACS 4.0
;
;	(C)opyright 1993 by Daniel M. Lawrence
;

store-procedure disp-error
	write-message %error
	set $cmdhook nop
!endm
	set $cmdhook disp-error
	source "emacs.rc"
