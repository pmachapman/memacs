/*	EFUNC.H:	MicroEMACS function declarations and names

		This file list all the C code functions used by MicroEMACS
	and the names to use to bind keys to them. To add functions,
	declare it here in both the extern function list and the name
	binding table.

*/

/*	Name to function binding table

		This table gives the names of all the bindable functions
	end their C function address. These are used for the bind-to-key
	function.
*/

NOSHARE NBIND	names[] = {
	{"abort-command",		ctrlg},
	{"add-abbrev",			add_abbrev},
	{"add-global-mode",		setgmode},
#if UNIX || (VMS && SMG) || MPE
	{"add-keymap",			addkeymap},
#else
	{"add-keymap",			notavail},
#endif
	{"add-mode",			setmod},
	{"append-file",			fileapp},
	{"apropos",			apro},
#if	CTAGS
	{"back-from-tag-word",		backtagword},
#else
	{"back-from-tag-word",		notavail},
#endif
	{"backward-character",		backchar},
	{"begin-macro",			ctlxlp},
	{"beginning-of-file",		gotobob},
	{"beginning-of-line",		gotobol},
	{"bind-to-key",			bindtokey},
#if	WINDOW_MSWIN || CURSES
	{"bind-to-menu",		bindtomenu},
#else
	{"bind-to-menu",		notavail},
#endif
	{"buffer-position",		showcpos},
#if	WINDOW_MSWIN
	{"cascade-screens",		cascadescreens},
#else
	{"cascade-screens",		notavail},
#endif
	{"case-region-lower",		lowerregion},
	{"case-region-upper",		upperregion},
	{"case-word-capitalize",	capword},
	{"case-word-lower",		lowerword},
	{"case-word-upper",		upperword},
	{"change-file-name",		filename},
	{"change-screen-column",	new_col_org},
	{"change-screen-row",		new_row_org},
	{"change-screen-size",		newsize},
	{"change-screen-width",		newwidth},
	{"clear-and-redraw",		uerefresh},
	{"clear-message-line",		clrmes},
#if	WINDOW_MSWIN
	{"clip-region", 		clipregion},
#else
	{"clip-region", 		notavail},
#endif
	{"copy-region",			copyregion},
	{"count-words",			wordcount},
	{"ctlx-prefix",			cex},
#if	WINDOW_MSWIN
	{"cut-region",			cutregion},
#else
	{"cut-region",			notavail},
#endif
	{"cycle-ring",			cycle_ring},
	{"cycle-screens",		cycle_screens},
	{"define-abbrevs",		def_abbrevs},
	{"delete-abbrev",		del_abbrev},
	{"delete-blank-lines",		deblank},
	{"delete-buffer",		killbuffer},
	{"delete-global-mode",		delgmode},
	{"delete-kill-ring",		clear_ring},
	{"delete-mode",			delmode},
	{"delete-next-character",	forwdel},
	{"delete-next-word",		delfword},
	{"delete-other-windows",	onlywind},
	{"delete-previous-character",	backdel},
	{"delete-previous-word",	delbword},
	{"delete-screen",		delete_screen},
	{"delete-undos",		undo_delete},
	{"delete-window",		delwind},
	{"describe-abbrevs",		desc_abbrevs},
	{"describe-bindings",		desbind},
	{"describe-functions",		desfunc},
	{"describe-key",		deskey},
	{"describe-variables",		desvars},
	{"detab-region",		detab},
	{"display",			dispvar},
	{"end-macro",			ctlxrp},
	{"end-of-file",			gotoeob},
	{"end-of-line",			gotoeol},
	{"end-of-word",			endword},
	{"entab-region",		entab},
	{"exchange-point-and-mark",	swapmark},
	{"execute-buffer",		execbuf},
	{"execute-command-line",	execcmd},
	{"execute-file",		execfile},
	{"execute-macro",		ctlxe},
	{"execute-named-command",	namedcmd},
	{"execute-procedure",		execproc},
	{"execute-program",		execprg},
	{"exit-emacs",			quit},
	{"fill-paragraph",		fillpara},
	{"filter-buffer",		uefilter},
	{"find-file",			filefind},
	{"find-screen",			find_screen},
	{"forward-character",		forwchar},
	{"global",			global_var},
	{"goto-line",			gotoline},
	{"goto-mark",			gotomark},
	{"goto-matching-fence",		getfence},
	{"grow-window",			enlargewind},
	{"handle-tab",			uetab},
	{"help",			help},
#if	WINDOW_MSWIN
	{"help-engine", 		helpengine},
#else
	{"help-engine", 		notavail},
#endif
#if	LIBHELP
	{"hide-buffer",			hidebuffer},
#else
	{"hide-buffer",			notavail},
#endif
	{"hunt-backward",		backhunt},
	{"hunt-forward",		forwhunt},
	{"i-shell",			spawncli},
#if	ISRCH
	{"incremental-search",		fisearch},
#else
	{"incremental-search",		notavail},
#endif
	{"indent-region",		indent_region},
#if LIBHELP
	{"index-help",			hlpindex},
#else
	{"index-help",			notavail},
#endif
	{"insert-abbrevs",		ins_abbrevs},
#if	WINDOW_MSWIN
	{"insert-clip", 		insertclip},
#else
	{"insert-clip", 		notavail},
#endif
	{"insert-file",			insfile},
	{"insert-space",		insspace},
	{"insert-string",		istring},
	{"kill-abbrevs",		kill_abbrevs},
	{"kill-paragraph",		killpara},
	{"kill-region",			killregion},
	{"kill-to-end-of-line",		killtext},
#if	FLABEL
	{"label-function-key",		fnclabel},
#else
	{"label-function-key",		notavail},
#endif
	{"list-buffers",		listbuffers},
#if UNIX || (VMS && SMG) || MPE || LINUX
	{"list-keymappings",		listkeymaps},
#else
	{"list-keymappings",		notavail},
#endif
	{"list-screens",		list_screens},
	{"list-undos",			undo_list},
	{"local",			local_var},
#if LIBHELP
	{"look-word",			lookupword},
#else
	{"look-word",			notavail},
#endif
	{"macro-to-key",		macrotokey},
#if	WINDOW_MSWIN || CURSES
	{"macro-to-menu",		macrotomenu},
#else
	{"macro-to-menu",		notavail},
#endif
#if	WINDOW_MSWIN
	{"maximize-screen",		maximizescreen},
#else
	{"maximize-screen",		notavail},
#endif
	{"meta-prefix", 		uemeta},
#if	WINDOW_MSWIN
	{"minimize-screen",		minimizescreen},
#else
	{"minimize-screen",		notavail},
#endif
#if	MOUSE
	{"mouse-move",			mmove},
	{"mouse-move-down",		movemd},
	{"mouse-move-up",		movemu},
	{"mouse-region-down",		mregdown},
	{"mouse-region-up",		mregup},
	{"mouse-resize-screen",		resizm},
	{"mouse-resize2-screen",	resizm2},
#else
	{"mouse-move",			notavail},
	{"mouse-move-down",		notavail},
	{"mouse-move-up",		notavail},
	{"mouse-region-down",		notavail},
	{"mouse-region-up",		notavail},
	{"mouse-resize-screen",		notavail},
	{"mouse-resize2-screen",	notavail},
#endif
	{"move-window-down",		mvdnwind},
	{"move-window-up",		mvupwind},
	{"name-buffer",			namebuffer},
	{"narrow-to-region",		narrow},
	{"newline",			new_line},
	{"newline-and-indent",		indent},
	{"next-buffer",			nextbuffer},
	{"next-line",			forwline},
	{"next-page",			forwpage},
	{"next-paragraph",		gotoeop},
	{"next-window",			nextwind},
	{"next-word",			forwword},
	{"nop",				nullproc},
	{"open-line",			openline},
	{"overwrite-string",		ovstring},
	{"pipe-command",		pipecmd},
	{"pop-buffer",			popbuffer},
	{"previous-line",		backline},
	{"previous-page",		backpage},
	{"previous-paragraph",		gotobop},
	{"previous-window",		prevwind},
	{"previous-word",		backword},
	{"print",			writemsg},
#if LIBHELP
	{"prompt-look",			promptlook},
#else
	{"prompt-look",			notavail},
#endif
	{"query-replace-string",	qreplace},
	{"quick-exit",			quickexit},
	{"quote-character",		quote},
#if	CTAGS
	{"re-tag-word",			retagword},
#else
	{"re-tag-word",			notavail},
#endif
	{"read-file",			fileread},
	{"redraw-display",		reposition},
	{"remove-mark",			remmark},
	{"rename-screen",		rename_screen},
	{"replace-string",		sreplace},
	{"resize-window",		resize},
#if	WINDOW_MSWIN
	{"restore-screen",		restorescreen},
#else
	{"restore-screen",		notavail},
#endif
	{"restore-window",		restwnd},
#if	ISRCH
	{"reverse-incremental-search",	risearch},
#else
	{"reverse-incremental-search",	notavail},
#endif
	{"run",				execproc},
	{"save-file",			filesave},
	{"save-window",			savewnd},
	{"scroll-next-down",		nextdown},
	{"scroll-next-up",		nextup},
	{"search-forward",		forwsearch},
	{"search-reverse",		backsearch},
	{"select-buffer",		usebuffer},
	{"set",				setvar},
#if	CRYPT
	{"set-encryption-key",		setekey},
#else
	{"set-encryption-key",		notavail},
#endif
	{"set-mark",			setmark},
	{"shell-command",		spawn},
	{"show-files",			showfiles},
	{"shrink-window",		shrinkwind},
	{"source",			execfile},
	{"split-current-window",	splitwind},
	{"store-procedure",		storeproc},
#if	BSD || FREEBSD || VMS || SUN || HPUX8 || HPUX9 || AVIION
	{"suspend-emacs",		bktoshell},
#else
	{"suspend-emacs",		notavail},
#endif
#if	CTAGS
	{"tag-word",			tagword},
#else
	{"tag-word",			notavail},
#endif
#if	WINDOW_MSWIN
	{"tile-screens",		tilescreens},
#else
	{"tile-screens",		notavail},
#endif
	{"transpose-characters",	twiddle},
	{"trim-region",			trim},
	{"unbind-key",			unbindkey},
#if	WINDOW_MSWIN || CURSES
	{"unbind-menu", 		unbindmenu},
#else
	{"unbind-menu", 		notavail},
#endif
	{"undent-region",		undent_region},
	{"undo",			undo},
	{"universal-argument",		unarg},
	{"unmark-buffer",		unmark},
	{"update-screen",		upscreen},
	{"view-file",			viewfile},
	{"widen-from-region",		widen},
	{"wrap-word",			wrapword},
	{"write-file",			filewrite},
	{"write-message",		writemsg},
	{"yank",			yank},
	{"yank-pop",			yank_pop},
#if LIBHELP
	{"zap-help",			zaphelp},
#else
	{"zap-help",			notavail},
#endif
	{"",			NULL}
};

#define	NFUNCS	(sizeof(names)/sizeof(NBIND)) - 1
