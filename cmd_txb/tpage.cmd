;	TPAGE.CMD:	TxBasic language MENU Page
;			for MicroEMACS 3.9e and above
;

; set up the "clean" procedure

store-procedure clean
	delete-buffer "[b-indent]"
	delete-buffer "[b-indent-brace]"
	delete-buffer "[b-check]"
	delete-buffer "[b-compile]"
	delete-buffer "[b-compile-release]"
	delete-buffer "[b-compile-speed]"
	delete-buffer "[b-compile-debug]"
	delete-buffer "[b-compile-gdb]"
	delete-buffer "[b-run]"
	delete-buffer "[b-debug]"
	delete-buffer "[b-gdb]"
	delete-buffer "[b-new-if]"
	delete-buffer "[b-new-switch]"
	delete-buffer "[b-new-for]"
	delete-buffer "[b-new-while]"
	set $cmode %oldmode
	set $gmode %oldgmode
!endm

store-procedure b-indent
;	set $debug TRUE
	unmark-buffer
	filter-buffer &cat "txf --unbrace --upper -i" $hardtab
	!force set $debug FALSE
!endm

store-procedure b-indent-brace
;	set $debug TRUE
	unmark-buffer
	filter-buffer &cat "txf --brace --upper -i" $hardtab
	!force set $debug FALSE
!endm

store-procedure b-compile
;	set $debug TRUE
	!if %rcfkeys
		!force toggle-fkeys
	!endif
	delete-other-windows
	delete-buffer "[S-F9 prev error  S-F10 next error]"
	save-file
	set %cfname $cfname
	set %cmode &band $cmode 16384
	!if &equ %cmode 0
		!force pipe-command &cat &cat %command " --utf8 " $cfname
	!else
		!force pipe-command &cat %command $cfname
	!endif
	name-buffer "[S-F9 prev error  S-F10 next error]"
	end-of-file
	!if &equ $curline 1
		!force delete-mode "VIEW"
		insert-string &cat &cat %cfname " successfully " %work
		unmark-buffer
		!force 1 resize-window
		add-mode "VIEW"
		!force delete-mode "CMODE"
		next-window
	!else
		beginning-of-file
		!force 5 resize-window
		add-mode "VIEW"
		!force delete-mode "CMODE"
		next-window
		beginning-of-file
		execute-procedure "b-next-error"
	!endif
	!force set $debug FALSE
!endm

store-procedure b-check
;	set $debug TRUE
	set %command "mepipe txlint "
	set %work "checked"
	execute-procedure "b-compile"
	!force set $debug FALSE
!endm

store-procedure b-compile-release
;	set $debug TRUE
	set %command "mepipe txb -T -C"
	set %work "compiled"
	execute-procedure "b-compile"
	!force set $debug FALSE
!endm

store-procedure b-compile-speed
;	set $debug TRUE
	set %command "mepipe txb"
	set %work "compiled"
	execute-procedure "b-compile"
	!force set $debug FALSE
!endm

store-procedure b-compile-debug
;	set $debug TRUE
	set %command "mepipe txb -d -T"
	set %work "compiled"
	execute-procedure "b-compile"
	!force set $debug FALSE
!endm

store-procedure b-compile-gdb
;	set $debug TRUE
	set %command "mepipe txb -g -T -C"
	set %work "compiled"
	execute-procedure "b-compile"
	!force set $debug FALSE
!endm

store-procedure b-run
;	set $debug TRUE
	set %rctmp &sri $cfname "."
	set %cfname &mid $cfname 1 &sub %rctmp 1
	shell-command &cat "merun ./" %cfname
	!force set $debug FALSE
!endm

store-procedure b-gdb
;	set $debug TRUE
	set %rctmp &sri $cfname "."
	set %cfname &mid $cfname 1 &sub %rctmp 1
	shell-command &cat "gdb -tui ./" %cfname
	!force set $debug FALSE
!endm

store-procedure b-prev-error
;	set $debug TRUE
	delete-other-windows
	split-current-window
	select-buffer "[S-F9 prev error  S-F10 next error]"
	add-mode "VIEW"
	!force 5 resize-window
	beginning-of-line
	!force search-reverse ", line "
	!if &seq $status FALSE
		write-message "[No more error]"
		previous-window
		!return
	!endif
	7 forward-character
	set-mark
	end-of-word
	set %line $region
	previous-window
	!force %line goto-line
	!force set $debug FALSE
!endm

store-procedure b-next-error
;	set $debug TRUE
	delete-other-windows
	split-current-window
	select-buffer "[S-F9 prev error  S-F10 next error]"
	add-mode "VIEW"
	!force 5 resize-window
	!force search-forward ", line "
	!if &seq $status FALSE
		write-message "[No more error]"
		previous-window
		!return
	!endif
	set-mark
	end-of-word
	set %line $region
	previous-window
	!force %line goto-line
	!force set $debug FALSE
!endm

store-procedure b-new-if
	set %cond @"Logical expression: "
;	set $debug TRUE
	insert-string &cat "IF " %cond
	newline-and-indent
	newline-and-indent
	insert-string "ELSE"
	newline-and-indent
	handle-tab
	newline-and-indent
	insert-string "IFEND"
	newline-and-indent
	delete-previous-character
	4 previous-line
	handle-tab
	end-of-line
	!force set $debug FALSE
!endm

store-procedure b-new-select
	set %cond @"Expression: "
;	set $debug TRUE
	insert-string &cat "SELECT " %cond
	newline-and-indent
	newline-and-indent
	insert-string "CASE"
	newline-and-indent
	handle-tab
	newline-and-indent
	insert-string "CEND"
	newline-and-indent
	delete-previous-character
	6 previous-line
	end-of-line
	!force set $debug FALSE
!endm

;	FOR NEXT
store-procedure b-new-for
	set %cond @"Variable name: "
;	set $debug TRUE
	insert-string &cat &cat "FOR " %cond "="
	newline-and-indent
	handle-tab
	newline-and-indent
	insert-string &cat "NEXT " %cond
	2 previous-line
	end-of-line
	!force set $debug FALSE
!endm

;
;	WHILE WEND
store-procedure b-new-while
	set %cond @"Logical expression: "
;	set $debug TRUE
	insert-string &cat "WHILE " %cond
	newline-and-indent
	handle-tab
	newline-and-indent
	insert-string "WEND"
	previous-line
	end-of-line
	!force set $debug FALSE
!endm

store-procedure toggle-errors
	!if %rcerrors
		!goto rcfoff
	!endif

;	toggle errors window on
	save-window
	1 next-window
	!if &sequal $cbufname "emacs.hlp"
		delete-window
	!endif
	!if &not &sequal $cbufname "[S-F9 prev error  S-F10 next error]"
		1 split-current-window
		1 select-buffer "[S-F9 prev error  S-F10 next error]"
		add-mode "red"
		add-mode "WHITE"
		!force 5 resize-window
		1 goto-line
	!endif
	set %rcfkeys TRUE
	!force restore-window
	!if &sequal $cbufname "[S-F9 prev error  S-F10 next error]"
		next-window
	!endif
	write-message "[Errors window ON]"
	!return

	;Toggle the error window off
*rcfoff
	save-window
	1 next-window
	!if &sequal "[S-F9 prev error  S-F10 next error]" $cbufname
		delete-window
	!endif
	!force restore-window
	write-message "[Errors window OFF]"
	set %rcerrors FALSE
!endm

macro-to-key b-indent			S-FN1
macro-to-key b-indent-brace		S-FN2
macro-to-key b-check			S-FN3
macro-to-key b-compile-release	S-FN4
macro-to-key b-compile-speed	S-FN5
macro-to-key b-compile-debug	S-FN6
macro-to-key b-compile-gdb		S-FN7
macro-to-key b-run				S-FN8
macro-to-key b-prev-error		S-FN9
macro-to-key b-next-error		S-FN0

macro-to-menu b-indent		"TxBasic>Format b"
macro-to-menu b-indent-brace	"TxBasic>Format w"
macro-to-menu b-check		"TxBasic>Check"
macro-to-menu b-compile-release		"TxBasic>Compile r"
macro-to-menu b-compile-speed		"TxBasic>Compile for s"
macro-to-menu b-compile-debug		"TxBasic>Compile for T"
macro-to-menu b-compile-gdb		"TxBasic>Compile for g"
macro-to-menu b-run			"TxBasic>Run"
macro-to-menu b-gdb			"TxBasic>Run t"
macro-to-menu b-prev-error	"TxBasic>Prev"
macro-to-menu b-next-error	"TxBasic>Next"
macro-to-menu b-new-if		"TxBasic>IF"
macro-to-menu b-new-select	"TxBasic>SELECT"
macro-to-menu b-new-for		"TxBasic>FOR"
macro-to-menu b-new-while	"TxBasic>WHILE"

; Set up CMODE
add-mode CMODE
add-global-mode CMODE
set $hardtab 5

write-message "[TxBasic perspective loaded]"
