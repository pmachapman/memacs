;
; Screen juggling macroes for MicroEMACS 4.0 for Windows.
; Handles MDI features, including drag-and-drop.
;
; Pierre Perret - April 1993
; Updated for version 4.0 May 1995
;

global %scrwidth
global %filename
global %dropbuf

rename-screen $cbufname
restore-screen
set $newscreen TRUE	; A new screen for each new file
set %scrwidth 75
;set %scrwidth 80	; 80 is more appropriate above 640x480

%scrwidth change-screen-width

set %filename ""
set %dropbuf "Dropped files"	; Special buffer used for drag-and-drop.

;
; Reads files in an MDI manner.
; Overcomes a bug in the "new screen" mechanism by re-sizing the screen.
; Called by the drag-and-drop mechanism as well.
;
store-procedure open-file
	local %prevbuf
	set %prevbuf $cbufname

	;
	; Are we called from drop-files?
	;
	!if &seq %filename ""
		!force execute-named-command find-file
	!else
		!force find-file %filename &lower %filename
		; the second argument is there to supply an alternate buffer name
	!endif
	!if $status
		%scrwidth change-screen-width
	!else
		; find-file failed
		!if &not &seq $cbufname %prevbuf
			; buf name changed ==> it is a new file
			%scrwidth change-screen-width
		!endif
	!endif
	set %filename ""
!endm
macro-to-key	open-file	^X^F
unbind-menu	">&File>&Open..."
macro-to-menu	open-file	"&Open...@0"

;
; Reads files in an MDI manner, with VIEW mode.
; Overcomes a bug in the "new screen" mechanism by re-sizing the screen.
;
store-procedure visit-file
	local %prevbuf
	set %prevbuf $cbufname

	!force execute-named-command view-file

	!if $status
		%scrwidth change-screen-width
	!else
		; view-file failed
		!if &not &seq $cbufname %prevbuf
			; buf name changed ==> it is a new file
			%scrwidth change-screen-width
		!endif
	!endif
!endm
macro-to-key	visit-file	^X^V
unbind-menu	">&File>&View..."
macro-to-menu	visit-file	"&View...@1"

;
; Makes sure there is one screen per visible buffer.
; Does not affect screens whose name do not match an existing buffer.
;
store-procedure rebuild-screens
	local %lastbuf

	;
	; If we happen to start with an invisible buffer, skip it.
	;
	!if &equ $cbflags &band 1 1
		!force next-buffer
		!if &not $status
			!return
		!endif
	!endif
	set %lastbuf $cbufname
*BufLoop
		find-screen "Relay screen"
		next-buffer
		find-screen $cbufname
		select-buffer $scrname
		delete-other-windows
		restore-screen
		%scrwidth change-screen-width

		!if &not &seq %lastbuf $cbufname
			!goto BufLoop
		!endif

	!if &seq $scrname "Relay screen"
		cycle-screens
	!endif
	!force delete-screen "Relay screen"
	cascade-screens
!endm
macro-to-menu	rebuild-screens ">S&creen>&Rebuild@0"

;
; Close both the screen and the buffer associated with it.
;
store-procedure kill-screen
	local %prevbuf
	local %prevscr

	set %prevscr $scrname
	set %prevbuf $cbufname
	cycle-screens
	!force delete-screen %prevscr
	!if $status
		!force delete-buffer %prevbuf
	!endif
!endm
macro-to-key	kill-screen A-K
macro-to-menu	kill-screen 	">S&creen>&Kill@6"

;
; Handle a drag-and-drop.
;
store-procedure drop-files
	local %prevbuf

	; Note that we pay no attention to the location of the drop.
	set %prevbuf $cbufname
	select-buffer %dropbuf
	goto-line 2
	select-buffer %prevbuf
	set %filename #%dropbuf
	!while &not &seq %filename "<END>"
		run open-file
		set %filename #%dropbuf
	!endwhile
	set %filename ""
!endm
macro-to-key	drop-files	MS!
