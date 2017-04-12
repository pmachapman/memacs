;	translate the files from UNIX line termination to DOS

*setup

	delete-other-windows
	find-file flist
	beginning-of-file
	select-buffer main

*next
	set %cfile #flist
	!if &seq %cfile "<end>"
		!goto endit
	!endif

	find-file %cfile
	set %b $cbufname
	update-screen
	insert-string "a"
	delete-previous-character
	save-file

	select-buffer main
	delete-buffer %b
	!goto next

*endit

	print "[All Done]~n"
	exit-emacs
