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
#endif
	{"add-mode",			setmod},
	{"append-file",			fileapp},
	{"apropos",			apro},
#if	CTAGS
	{"back-from-tag-word",		backtagword},
#endif
	{"backward-character",		backchar},
	{"begin-macro",			ctlxlp},
	{"beginning-of-file",		gotobob},
	{"beginning-of-line",		gotobol},
	{"bind-to-key",			bindtokey},
#if	WINDOW_MSWIN
	{"bind-to-menu",		bindtomenu},
#endif
	{"buffer-position",		showcpos},
#if	WINDOW_MSWIN
	{"cascade-screens",		cascadescreens},
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
	{"clear-and-redraw",		refresh},
	{"clear-message-line",		clrmes},
#if	WINDOW_MSWIN
	{"clip-region", 		clipregion},
#endif
	{"copy-region",			copyregion},
	{"count-words",			wordcount},
	{"ctlx-prefix",			cex},
#if	WINDOW_MSWIN
	{"cut-region",			cutregion},
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
	{"filter-buffer",		filter},
	{"find-file",			filefind},
	{"find-screen",			find_screen},
	{"forward-character",		forwchar},
	{"global",			global_var},
	{"goto-line",			gotoline},
	{"goto-mark",			gotomark},
	{"goto-matching-fence",		getfence},
	{"grow-window",			enlargewind},
	{"handle-tab",			tab},
	{"help",			help},
#if	WINDOW_MSWIN
	{"help-engine", 		helpengine},
#endif
	{"hunt-backward",		backhunt},
	{"hunt-forward",		forwhunt},
	{"i-shell",			spawncli},
#if	ISRCH
	{"incremental-search",		fisearch},
#endif
	{"indent-region",		indent_region},
	{"insert-abbrevs",		ins_abbrevs},
#if	WINDOW_MSWIN
	{"insert-clip", 		insertclip},
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
#endif
	{"list-buffers",		listbuffers},
#if UNIX || (VMS && SMG) || MPE
	{"list-keymappings",		listkeymaps},
#endif
	{"list-screens",		list_screens},
	{"list-undos",			undo_list},
	{"local",			local_var},
	{"macro-to-key",		macrotokey},
#if	WINDOW_MSWIN
	{"macro-to-menu",		macrotomenu},
	{"maximize-screen",		maximizescreen},
#endif
	{"meta-prefix", 		meta},
#if	WINDOW_MSWIN
	{"minimize-screen",		minimizescreen},
#endif
#if	MOUSE
	{"mouse-move",			mmove},
	{"mouse-move-down",		movemd},
	{"mouse-move-up",		movemu},
	{"mouse-region-down",		mregdown},
	{"mouse-region-up",		mregup},
	{"mouse-resize-screen",		resizm},
#endif
	{"move-window-down",		mvdnwind},
	{"move-window-up",		mvupwind},
	{"name-buffer",			namebuffer},
	{"narrow-to-region",		narrow},
	{"newline",			newline},
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
	{"query-replace-string",	qreplace},
	{"quick-exit",			quickexit},
	{"quote-character",		quote},
#if	CTAGS
	{"re-tag-word",			retagword},
#endif
	{"read-file",			fileread},
	{"redraw-display",		reposition},
	{"remove-mark",			remmark},
	{"rename-screen",		rename_screen},
	{"replace-string",		sreplace},
	{"resize-window",		resize},
#if	WINDOW_MSWIN
	{"restore-screen",		restorescreen},
#endif
	{"restore-window",		restwnd},
#if	ISRCH
	{"reverse-incremental-search",	risearch},
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
#endif
#if	CTAGS
	{"tag-word",			tagword},
#endif
#if	WINDOW_MSWIN
	{"tile-screens",		tilescreens},
#endif
	{"transpose-characters",	twiddle},
	{"trim-region",			trim},
	{"unbind-key",			unbindkey},
#if	WINDOW_MSWIN
	{"unbind-menu", 		unbindmenu},
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

	{"",			NULL}
};

#define	NFUNCS	(sizeof(names)/sizeof(NBIND)) - 1
