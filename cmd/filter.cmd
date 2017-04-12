;	FILTER.CMD:	Filter a change through a lot of files
;			written for MicroEMACS 4.0
;
;	This is one incredably usefull command file for filtering changes
;	in a large collection of files (Like all the MicroEMACS source files)
;
;	To use this,
;
;	1) modify the marked section to make what changes to the files
;	   you want to happen.
;	2) start EMACS and type M-X source <ret> filter to start the filter
;	3) step back. Get a cup of coffee (if there are a lot of files).
;	   Enjoy.

	;Make the file list first
	set %filemask @"What files to be filtered? "
	show-files %filemask
	select-buffer "File List"
	beginning-of-file
	!while &equ $curchar 46
		1 kill-to-end-of-line
	!endwhile
	end-of-file
	set-mark
	previous-line
	kill-region
	write-file "flist"
	unmark-buffer

	;Set up to run through the files
	delete-other-windows
	find-file flist
	beginning-of-file
	select-buffer main
	add-global-mode magic

	set %cfile #flist
	!while &not &seq %cfile "<END>"

		find-file %cfile
		set %b $cbufname
		update-screen
	
		;**** Here is where to make changes to specify the ****
		;**** commands to run on each file                 ****
		;**** Below is an such an example                  ****
		;query-replace-string is good to use for comfirmations
		;replace-string lets it all run automatically!!
	
		query-replace-string "3\.[0-9][0-9]" "4.0"
		beginning-of-file
		replace-string "nsearch" "search"
		beginning-of-file
	
		;Save the Results!
		update-screen
		save-file
	
		;Dump the already saved buffer
		select-buffer main
		delete-buffer %b

		;on to the next file in the file list
		set %cfile #flist

	!endwhile

*endit

	print "[All Done]~n"
	exit-emacs
