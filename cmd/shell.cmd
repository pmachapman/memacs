;	Shell.cmd:	MSDOS shell within MicroEMACS
;			written 1987 by Daniel Lawrence

	write-message "[Setting up SHELL window]"

store-procedure	getdir
	shell-command "cd > eshell"
	set %shtmp $cbufname
	!force delete-buffer eshell
	find-file eshell
	beginning-of-file
	set %shdir $line
	select-buffer %shtmp
	delete-buffer eshell
!endm

store-procedure prompt
	!if &not &equ $curcol 0
		newline
	!endif
	!if &not &seq $line ""
		open-line
	!endif
	insert-string &cat %shdir ">"
!endm

store-procedure getline
	beginning-of-line
	set %shtmp $curline
	!force search-forward ">"
	!if &not &equ $curline %shtmp
		set $curline %shtmp
	!endif
	!if &equ $curchar 10
		set %shline ""
	!else
		kill-to-end-of-line
		yank
		set %shline $kill
	!endif
	newline
!endm

store-procedure execline
	shell-command &cat %shline " > shtmp"
	!force insert-file shtmp
!endm

;	prompt and execute a command

store-procedure do-command
	run getline
	!if &not &seq %shline ""
		!if &or &seq &left %shline 2 "cd" &seq &right %shline 1 ":"
			shell-command %shline
			run getdir
		!else
			run execline
		!endif
	!endif
	run prompt
!endm

store-procedure checkmode
	!if &and %shmode &not &seq $cbufname "[I-SHELL]"
		set $discmd FALSE
		bind-to-key newline ^M
		add-mode blue
		add-mode WHITE
		write-message "[Exiting Shell window]"
		set %shmode FALSE
		set $discmd TRUE
	!endif
	!if &and &not %shmode &seq $cbufname "[I-SHELL]"
		set $discmd FALSE
		write-message "[Entering Shell window]"
		macro-to-key do-command ^M
		run getdir
		add-mode black
		add-mode GREEN
		end-of-file
		run prompt
		set %shmode TRUE
		set $discmd TRUE
	!endif
!endm

;	window movement (and deactivate us)

store-procedure do-next-window
	next-window
	run checkmode
!endm

store-procedure do-previous-window
	previous-window
	run checkmode
!endm

store-procedure do-delete-window
	delete-window
	run checkmode
!endm

store-procedure do-find-file
	find-file @"Find file:"
	run checkmode
!endm

store-procedure do-next-buffer
	next-buffer
	run checkmode
!endm

store-procedure openshell
	set $discmd FALSE
	macro-to-key do-next-window ^XO
	macro-to-key do-previous-window ^XP
	macro-to-key do-previous-window ^X0
	macro-to-key do-find-file ^X^F
	macro-to-key do-next-buffer ^XX
	select-buffer "[I-SHELL]"
	run checkmode
	set $discmd TRUE
!endm

	set %shmode FALSE
	run openshell
