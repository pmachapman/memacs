;	BPAGE.CMD:	Box Macro and rectangualr region page
;			for MicroEMACS 3.13 and above
;			(C)opyright 1987,1993 by Suresh Konda/Daniel M Lawrence
;			Last Update: 11/13/93

; make sure the function key window is up
	set %rcfkeys FALSE
	toggle-fkeys
	write-message "Loading..."

; set the clean procedure up
store-procedure clean
	delete-buffer "[b-change-line]"
	delete-buffer "[b-del-block]"
	delete-buffer "[b-draw-box]"
	delete-buffer "[b-copy-block]"
	delete-buffer "[b-yank-block]"
	delete-buffer "[b-yank-block]"
	delete-buffer "[getblock]"
	delete-buffer "[putblock]"
!endm

; Write out the page instructions
	save-window
	1 next-window
	beginning-of-file
	delete-mode "VIEW"
	set $curcol 25
	overwrite-string " F1 single line box       F2 kill block        "
	next-line
	set $curcol 25
	overwrite-string " F3 double line box       F4 copy block        "
	next-line
	set $curcol 25
	overwrite-string "                          F6 yank block        "
	next-line
	set $curcol 18
	overwrite-string "BOX "
	set $curcol 25
	overwrite-string "                          F8 insert block      "
	next-line
	set $curcol 25
	overwrite-string "                                               "
	unmark-buffer
	beginning-of-file
	add-mode "VIEW"
	!force restore-window
	update-screen

; this sets overwrite mode to off.  to change it, set rcinsert to 1
set %rcinsert 0

;	change line type

store-procedure b-change-line
	!if &equ %rcltype 1
		set %rcltype 2
		set %rctmp "DOUBLE"
	!else
		set %rcltype 1
		set %rctmp "SINGLE"
	!endif
	set %cbuf $cbufname
	set %cline $cwline
	select-buffer "[Function Keys]"
	beginning-of-file
	1 goto-line
	40 forward-character
	delete-mode "VIEW"
	6 delete-next-character
	insert-string %rctmp
	unmark-buffer
	add-mode "VIEW"
	select-buffer %cbuf
	%cline redraw-display
	!return
!endm

;	delete a rectangular block of text

store-procedure b-del-block
	set %bkcopy FALSE
	run getblock
	write-message "[Block deleted]"
!endm

;	copy a rectangular region

store-procedure b-copy-block
	set %bkcopy TRUE
	run getblock
	write-message "[Block copied]"
!endm

;	yank a rectangular region

store-procedure b-yank-block
	set %bkcopy TRUE
	run putblock
!endm

;	insert a rectangular region

store-procedure b-ins-block
	set %bkcopy FALSE
	run putblock
!endm

store-procedure getblock
	;set up needed variables
	set $discmd FALSE
	delete-buffer "[block]"
	set %rcbuf $cbufname
	set %cline $cwline

	;save block boundaries
	set %endpos $curcol
	set %endline $curline
	detab-region
	exchange-point-and-mark
	set %begpos $curcol
	set %begline $curline
	set %blwidth &sub %endpos %begpos

	;scan through the block
	set $curline %begline
	!while &less $curline &add %endline 1
		;grab the part of this line needed
		!force set $curcol %begpos
		set-mark
		!force set $curcol %endpos
		kill-region

		;bring it back if this is just a copy
		!if %bkcopy
			yank
		!endif

		;put the line in the block buffer
		select-buffer "[block]"
		yank

		;and pad it if needed
		!if &less $curcol %blwidth
			&sub %blwidth $curcol insert-space
			end-of-line
		!endif
		forward-character

		;onward...
		select-buffer %rcbuf
		next-line
	!endwhile

        ;unmark the block
        select-buffer "[block]"
        unmark-buffer
        select-buffer %rcbuf
        previous-line
        %cline redraw-display
	set $discmd TRUE
!endm

;	insert/overlay a rectangular block of text

store-procedure putblock
;	set $debug TRUE
	;set up needed variables
	set $discmd FALSE
	set %rcbuf $cbufname
	set %cline $cwline

	;save block boundaries
	set %begpos $curcol
	set %begline $curline

	;scan through the block
	select-buffer "[block]"
	beginning-of-file
	set %endpos &add %begpos $lwidth
	!while &not &equ $lwidth 0

		;pad the destination if it is needed
		select-buffer %rcbuf
		beginning-of-line
		!if &not &equ $lwidth 0
			1 detab-region
			previous-line
		!endif
		!force set $curcol %begpos
		!if &less $curcol %begpos
			&sub %begpos $curcol insert-space
			end-of-line
		!endif

		;delete some stuff if this should overlay
		!if %bkcopy
			set-mark
			!force set $curcol %endpos
			kill-region
		!endif

		;grab the line from the block buffer
		select-buffer "[block]"
		beginning-of-line
		set-mark
		end-of-line
		copy-region
		forward-character

		;put the line in the destination position
		select-buffer %rcbuf
		yank
		next-line

		;onward...
		select-buffer "[block]"
	!endwhile

	select-buffer %rcbuf
	set $curline %begline
	set $curcol %begpos
	%cline redraw-display
	set $discmd TRUE
	set $debug FALSE
!endm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;	Draw a box

	delete-buffer "[box]"
	delete-buffer "[simple-box]"
	delete-buffer "[double-box]"

store-procedure box
;	set $debug TRUE
	set $discmd FALSE
	!if &less $curline $mline
		exchange-point-and-mark
	!endif
	set %endline $curline
	set %rcol $curcol
	set %endpos $cwline
	exchange-point-and-mark
	set %topline $curline
	set-mark
	set %lcol $curcol
	!if &les %rcol %lcol
		set %rctmp %lcol
		set %lcol %rcol
		set %rcol %rctmp
	!endif
	1 detab-region
	previous-line
	set $curcol %lcol
	!if &equ $curline %endline
		; insert horizontal line
		!while &not &gre $curcol %rcol
			overwrite-string %hor
		!endwhile
	!else
		!if &equ %lcol %rcol
			; insert vertical line
			!while &not &gre $curline %endline
				end-of-line
				!while &les $curcol %lcol
					overwrite-string " "
				!endwhile
				!force set $curcol %lcol
				overwrite-string %ver
				next-line
			!endwhile
		!else
			; insert box
			set %rc %rcol - 1
			set %lc %lcol - 1
			; insert upper left corner
			overwrite-string %ulc
			; insert line
			!while &not &equ $curcol %rc
				overwrite-string %hor
			!endwhile
			; insert upper right corner
			overwrite-string %urc
			next-line
			!while &gre %endline $curline
				1 detab-region
				previous-line
				end-of-line
				; pad line up to the left side of the box
				!while &les $curcol %lc
					overwrite-string " "
				!endwhile
				!force set $curcol %lcol
				overwrite-string %ver
				end-of-line
				!while &les $curcol %rc
					overwrite-string " "
				!endwhile
				!force set $curcol %rcol
				overwrite-string %ver
				next-line
			!endwhile
			1 detab-region
			previous-line
			end-of-line
			; pad line up to the left side of the box
			!while &les $curcol %lc
				overwrite-string " "
			!endwhile
			!force set $curcol %lcol
			overwrite-string %llc
			!while &not &equ $curcol %rc
				overwrite-string %hor
			!endwhile
			overwrite-string %lrc
		!endif
	!endif
	set $cwline %endpos
	set $discmd TRUE
!endm

;	display single box

store-procedure simple-box
	set %ulc "┌"
	set %urc "┐"
	set %lrc "┘"
	set %llc "└"
	set %hor "─"
	set %ver "│"
	run box
!endm

;	display double box

store-procedure double-box
	set %ulc "╔"
	set %urc "╗"
	set %lrc "╝"
	set %llc "╚"
	set %hor "═"
	set %ver "║"
	run box
!endm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
macro-to-key simple-box		S-FN1
macro-to-key b-del-block	S-FN2
macro-to-key double-box		S-FN3
macro-to-key b-copy-block	S-FN4
!force unbind-key      		S-FN5
macro-to-key b-yank-block	S-FN6
!force unbind-key          	S-FN7
macro-to-key b-ins-block	S-FN8
!force unbind-key          	S-FN9
!force unbind-key          	S-FN0

; and init some variables
set %rcltype 2
write-message "[Block mode loaded]"

