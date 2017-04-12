;	EPAGE.CMD:	EMACS Macro Programming Page
;			for MicroEMACS 3.9d and above
;			(C)opyright 1987 by Daniel M Lawrence

; set the clean procedure up
store-procedure clean
	delete-buffer "[display-variable]"
	delete-buffer "[eval-exp]"
	delete-buffer "[compile-buffer]"
	delete-buffer "[toggle-debugging]"
	unbind-key S-FN5
	unbind-key S-FN6
	unbind-key S-FN7
	unbind-key S-FN8
	unbind-key S-FN9
	unbind-key S-FN0
!endm

; make sure the function key window is up
	set %rcfkeys FALSE
	toggle-fkeys
	write-message "Loading..."

; Write out the page instructions
	save-window
	1 next-window
	beginning-of-file
	set $curcol 25
	overwrite-string " F1 display variable       F2 evaluate expression"
	next-line
	set $curcol 25
	overwrite-string " F3 compile buffer         F4 debug mode [OFF]"
	next-line
	set $curcol 25
	overwrite-string " F5 execute buffer         F6 execute macro"
	next-line
	set $curcol 18
	overwrite-string "EMAC"
	set $curcol 25
	overwrite-string " F7 indent region          F8 undent region"
	next-line
	set $curcol 25
	overwrite-string " F9 execute DOS command   F10 shell to DOS"
	unmark-buffer
	beginning-of-file
	!force restore-window
	update-screen

; display a variable

store-procedure	display-variable
	set %rcdebug $debug
	set $debug FALSE
	set %rctmp @&cat &cat "Variable to display[" %rcvar "]: "
	!if &not &seq %rctmp ""
		set %rcvar %rctmp
	!endif
	write-message &cat &cat &cat %rcvar " = [" &ind %rcvar "]"
	set $debug %rcdebug
!endm
macro-to-key display-variable	S-FN1

;	evaluate expresion

store-procedure eval-exp
	delete-buffer "[temp]"
	set %rcbuf $cbufname
	set %cline $cwline
	select-buffer "[temp]"
	insert-string "set %rcval "
	!force insert-string @"EXP: "
	execute-buffer "[temp]"
	unmark-buffer
	select-buffer %rcbuf
	delete-buffer "[temp]"
	%cline redraw-display
	write-message &cat &cat "Value = [" %rcval "]"
!endm
macro-to-key eval-exp S-FN2

;	compile the current buffer

store-procedure compile-buffer
;	here is where to add code to handle compiling the current buffer
	write-message "[No Compiler module loaded]"
!endm
macro-to-key compile-buffer S-FN3

;	Toggle debug mode

store-procedure toggle-debugging
	set $debug FALSE
	set %cbuf $cbufname
	set %cline $cwline
	select-buffer "Function Keys"
	beginning-of-file
	next-line
	68 forward-character
	set %rcdebug &equ $curchar 70
	delete-previous-character
	2 delete-next-character
	!if %rcdebug
		insert-string "ON "
	!else
		insert-string "OFF"
	!endif
	unmark-buffer
	select-buffer %cbuf	
	%cline redraw-display
	write-message &cat &cat "[Debug Mode " %rcdebug "]"
	set $debug %rcdebug
!endm
macro-to-key toggle-debugging S-FN4

	bind-to-key execute-buffer S-FN5
	bind-to-key execute-macro S-FN6
	bind-to-key indent-region S-FN7
	bind-to-key undent-region S-FN8
	bind-to-key shell-command S-FN9
	bind-to-key i-shell S-FN0
	set %rcvar ""
	write-message "[Program page loaded]"
