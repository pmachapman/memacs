;	WPAGE.CMD:	Wordprocessing Macro Page
;			for MicroEMACS 3.13 and above
;			(C)opyright 1987 by Daniel M Lawrence

; set the clean procedure up
store-procedure clean
	delete-buffer "[w-center]"
	delete-buffer "[w-scheck]
	delete-buffer "[w-indformat]"
	delete-buffer "[w-notready]"
	delete-buffer "[w-swapword]"
	unbind-key S-FN3
	unbind-key S-FN9

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
	overwrite-string " F1 center line                                     "
	next-line
	set $curcol 25
	overwrite-string " F3 reformat paragraph     F4 spell check file      "
	next-line
	set $curcol 25
	overwrite-string " F5 ref indented paragraph F6 PREVIEW output        "
	next-line
	set $curcol 18
	overwrite-string "WORD"
	set $curcol 25
	overwrite-string " F7 toggle words           F8 output DRAFT          "
	next-line
	set $curcol 25
	overwrite-string " F9 count words           F10 output FINAL          "
	unmark-buffer
	beginning-of-file
	!force restore-window
	update-screen

; Center the current line

store-procedure w-center
	end-of-line
	set %rctmp $curcol
	beginning-of-line
	&div &sub $fillcol %rctmp 2 insert-space
	beginning-of-line
!endm

;	Spell check the current buffer

store-procedure w-scheck
	set %rctmp @"User dictionary(<ret> if none): "
	!if &seq %rctmp "ERROR"
		!return
	!endif
	!if &not &seq %rctmp ""
		set %rctmp &cat &cat "-u" %rctmp " "
	!endif
	save-file
	set %rcspell TRUE
	write-message ~n
	shell-command &cat &cat "spell " %rctmp $cfname
	execute-file "scan.cmd"
	delete-other-windows
	!if &seq %rcfkeys TRUE
		set %rcfkeys FALSE
		toggle-fkeys
	!endif
	update-screen
	write-message "[Spell Check complete]"
!endm

;	reformat indented paragraph

store-procedure w-indformat
	write-message "                    [Fixing paragraph]"

	;remember where we are..and set up
	set %endpos $cwline
	set $discmd "FALSE"

	;snarf the paragraph and put it in its own buffer
	kill-region
	2 split-current-window
	select-buffer "[temp]"
	yank

	;get rid of the blank lines at the end
	end-of-file
	!while &and &seq $line "" &not &equ $curline 1
		delete-previous-character
	!endwhile
	forward-character

	;get rid of the indents
	set %endline $curline
	beginning-of-file
	!while &gre %endline $curline
		!while &or &equ $curchar 32 &equ $curchar 9
			delete-next-character
		!endwhile
		next-line
	!endwhile

	;re-format it
	&sub $fillcol 8 set $fillcol
	1 goto-line
	fill-paragraph
	&add $fillcol 8 set $fillcol

	;insert new indents
	end-of-file
	set %endline $curline
	beginning-of-file
	!while &less $curline &sub %endline 1
		beginning-of-line
		handle-tab
		next-line
	!endwhile

	;move it back into the original buffer
	beginning-of-file
	set-mark
	end-of-file
	kill-region
	unmark-buffer
	delete-window
	yank

	;and clean up
	delete-buffer "[temp]"
	set $cwline %endpos
	set $discmd TRUE
	write-message "                    [Fixed paragraph]"
!endm

store-procedure w-notready
	write-message "[MicroSCRIBE not ready]"
!endm

;	swap the word the cursor is on and it's predisesor

store-procedure w-swapword
	set %rctmp $search
	forward-character
	previous-word
	0 delete-next-word
	set %second $kill
	insert-string "~~"
	!force previous-word
	!if &seq $status FALSE
		set %first %second
	!else
		0 delete-next-word
		set %first $kill
		insert-string %second
	!endif
	search-forward "~~"
	delete-previous-character
	insert-string %first
	previous-word
	set $search %rctmp
!endm

macro-to-key w-center S-FN1
bind-to-key fill-paragraph S-FN3
macro-to-key w-scheck S-FN4
macro-to-key w-indformat S-FN5
macro-to-key w-notready S-FN6
macro-to-key w-swapword S-FN7
macro-to-key w-notready S-FN8
bind-to-key count-words S-FN9
macro-to-key w-notready S-FN8

set %oldmode $cmode
set %oldgmode $gmode
add-mode wrap
add-global-mode wrap
write-message "[Wordprocessing mode loaded]"
