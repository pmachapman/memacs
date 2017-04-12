;	CPAGE.CMD:	C language MENU Page
;			for MicroEMACS 3.9e and above
;			(C)opyright 1988 by Daniel Lawrence
;

; set up the "clean" procedure
store-procedure clean
	delete-buffer "[c-new-proc]"
	delete-buffer "[c-new-func]"
	delete-buffer "[c-new-if]"
	delete-buffer "[c-new-while]"
	delete-buffer "[c-new-repeat]"
	delete-buffer "[c-new-switch]"
	delete-buffer "[c-value]"
	delete-buffer "[c-keys-up]"
	delete-buffer "[c-find-com]
	delete-buffer "[c-comment-block]"
	delete-buffer "[drawbox]"
	delete-buffer "[setpoints]"
	set $cmode %oldmode
	set $gmode %oldgmode
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
	overwrite-string " F1 PROCEDURE             F2 function           "
	next-line
	set $curcol 25
	overwrite-string " F3 if then else          F4 while              "
	next-line
	set $curcol 25
	overwrite-string " F5 REPEAT                F6 switch             "
	next-line
	set $curcol 18
	overwrite-string "C   "
	set $curcol 25
	overwrite-string " F7 VALUE                 F8 Reserved words UP  "
	next-line
	set $curcol 25
	overwrite-string " F9 chk comment matching F10 Comment block      "
	unmark-buffer
	beginning-of-file
	!force restore-window
	update-screen

;

store-procedure c-new-proc
;	set $debug TRUE
        set %proc @"Procedure Name: "
        insert-string &cat &cat "Procedure " %proc "~n"
        insert-string "   ( VAR ~n"
        insert-string "   );~n"
        insert-string "~n"
        insert-string &cat &cat "{ (* " %proc " *)~n"
        insert-string &cat &cat "};  (* " %proc " *)~n"
	previous-line
        open-line
        insert-string "   "
	!force set $debug FALSE
!endm

;	Function
store-procedure c-new-func
        set %func @"Function Name: "
        set %type @"Return type: "
	insert-string %type
	!if &not &sequal &right %type 1 "*"
		insert-string " "
	!endif                            
        insert-string &cat %func "()~n~n{~n	~n}~n"
        3 backward-character
	!force set $debug FALSE
!endm

;

store-procedure	c-new-if
        set %cond @"Condition Expr: "
        insert-string &cat &cat "if (" %cond ") {"
	newline-and-indent
	newline-and-indent
        insert-string "} else {"
	newline-and-indent
	newline-and-indent
       	insert-string &cat &cat "}  /* if (" %cond ") */"
	1 previous-line
        end-of-line
        handle-tab
	2 previous-line
        end-of-line
        handle-tab
!endm

;

store-procedure	c-new-while
        set %cond @"Condition Expr: "
	;set $debug TRUE
        insert-string &cat &cat "while (" %cond ") {"
	newline-and-indent
	newline-and-indent
        insert-string &cat &cat "}  /* while (" %cond ") */"
	1 previous-line
        end-of-line
	handle-tab
!endm

store-procedure c-new-repeat
        set %cond @"Condition Expr: "
	;set $debug TRUE
        set %curcol $curcol
        insert-string &cat &cat "REPEAT~n"
        %curcol insert-string " "
        set $curcol %curcol
        !if &EQUal  %curcol 3
	        insert-string &cat &cat "UNTIL " %cond ";"
	!else
	        insert-string &cat      "UNTIL " %cond 
	!endif
        beginning-of-line
        open-line
        %curcol insert-string " "
        insert-string "   "
	!force set $debug FALSE
!endm

store-procedure c-new-switch
        set %expr @"Switch Expr: "
	;set $debug TRUE
        set %curcol $curcol
        insert-string &cat &cat "switch (" %expr ") {"
	newline-and-indent
	insert-string "}"
	previous-line
	end-of-line
	newline-and-indent
	handle-tab
	insert-string "case "
	!force set $debug FALSE
!endm

store-procedure c-value
;	set $debug TRUE
        set %curcol $curcol
        set %value @"Value: "
        insert-string &cat %value " : {~n"
        %curcol insert-string " "
        insert-string "    };~n"
        %curcol insert-string " "
	previous-line
        beginning-of-line
        open-line
        %curcol insert-string " "
        insert-string "       "
	!force set $debug FALSE
!endm

store-procedure	c-keys-up
;	set $debug TRUE
        set %curcol $curcol
	;save-file
        set-mark
	beginning-of-file
        write-message "[Replacing 'begin']
	replace-string "begin" "BEGIN"

	beginning-of-file
        write-message "[Replacing 'end']
	replace-string "end" "END"

	beginning-of-file
        write-message "[Replacing ' if']
	replace-string " if" " IF"

	beginning-of-file
        write-message "[Replacing ' else']
	replace-string " else" " ELSE"

	beginning-of-file
        write-message "[Replacing ' repeat']
	replace-string " repeat" " REPEAT"

	beginning-of-file
        write-message "[Replacing ' while']
	replace-string " while" " WHILE"

        exchange-point-and-mark
	!force set $debug FALSE
!endm

store-procedure	c-find-com
;	This hunts down mismatched comment problems in C

	beginning-of-file
	!force search-forward "/*"
	!if &seq $status FALSE
		!goto nend
	!endif

*nxtopen
	;record the position of the open comment
	update-screen
	set %oline $curline
	set %opos $curcol

	;find the first close comment
	!force search-forward "*/"
	!if &seq $status FALSE
		write-message "%%No close to this open comment"
		!return
	!endif

	;record the position of the close comment
	set %cline $curline
	set %cpos $curcol

	;go back to the open and find the next open
	set $curline %oline
	set $curcol %opos

	;and now find the next open
	!force search-forward "/*"
	!if &seq $status FALSE
		write-message "No errors"
		!return
	!endif
	set %nline $curline
	set %npos $curcol

	;compare the close to the next open
	!if &less %cline %nline
		!goto getnext
	!endif
	!if &and &equ %cline %nline &less %cpos %npos
		!goto getnext
	!endif

	;report a mismatch
	set $curline %oline
	set $curcol %opos
	write-message "%%This comment does not terminate properly"
	!return

*getnext
	set $curline %nline
	set $curcol %npos
	!goto nxtopen 
!endm

store-procedure c-comment-block
	set %c1 "/"
	set %c2 "*"
	set %c3 "\"
	set %c4 "\"
	set %c5 "/"
	set %c6 "*"
	run drawbox	
!endm

store-procedure drawbox
	run setpoints
	set $curline %mline
	set $curcol %mcol
;draw top horizontal line
	insert-string %c1
	set %width &add 2 &sub %pcol %mcol
	%width insert-string %c2
 	insert-string %c3
	newline-and-indent
;draw bottom horizontal line
	%pline goto-line
	next-line
	end-of-line
   	newline
	%mcol insert-string " "
	insert-string %c4
	%width insert-string %c2
	insert-string %c5
;bump pline 
	set %pline &add %pline 1
;draw verticals -- go to top and work our way down
	%mline goto-line
	!while &less $curline %pline
		next-line
		end-of-line
		!if &less $curcol %pcol
			&sub %pcol $curcol insert-string " "
		!endif
		set $curcol %pcol
		insert-string " "
		insert-string %c6
		set $curcol %mcol
		insert-string %c6
		insert-string " "
	!endwhile
;return to point
	%pline goto-line
	next-line
	beginning-of-line
	&add 6 %width forward-character
!endm

store-procedure setpoints
; procedure will set pcol, pline, mcol and mline. currently at point
; it will also detab the region
	set %pcol $curcol
	set %pline $curline
	exchange-point-and-mark
	set %mcol $curcol
	set %mline $curline
	exchange-point-and-mark
	detab-region
	set $curline %pline
	set $curcol %pcol
!endm

macro-to-key c-new-proc		S-FN1
macro-to-key c-new-func		S-FN2
macro-to-key c-new-if		S-FN3
macro-to-key c-new-while	S-FN4
macro-to-key c-new-repeat	S-FN5
macro-to-key c-new-switch	S-FN6
macro-to-key c-value		S-FN7
macro-to-key c-keys-up		S-FN8
macro-to-key c-find-com		S-FN9
macro-to-key c-comment-block	S-FN0
macro-to-key c-comment-block	A-FN0

; Set up CMODE
set %oldmode $cmode
set %oldgmode $gmode
add-mode CMODE
add-global-mode CMODE

write-message "[C MENU page loaded]"
