#include "estruct.h"
#include "eproto.h"
#include "edef.h"

#if	DYNMSGS
#if WINDOW_MSWIN || WINDOW_MSWIN32
#include <windows.h>
#endif

#if	THEOS
#include <errno.h>
#endif

#include <stdio.h>
#include <string.h>

extern NOSHARE CONST char *otherpath[];

static char *msg[339] = {
	"english",
	"[Starting new CLI]",					/* TEXT1 */
	"[Cannot write filter file]",				/* TEXT2 */
	"[Execution failed]",					/* TEXT3 */
	"Shell variable TERM not defined!",			/* TEXT4 */
	"Terminal type not 'vt100'!",				/* TEXT5 */
	"\r\n\n[End]",						/* TEXT6 */
	"Line to GOTO: ",					/* TEXT7 */
	"[Aborted]",						/* TEXT8 */
	"[Mark %d set]",					/* TEXT9 */
	"[Mark %d removed]",					/* TEXT10 */
	"No mark %d in this window",				/* TEXT11 */
	"[Help file is not online]",				/* TEXT12 */
	": describe-key ",					/* TEXT13 */
	"Not Bound",						/* TEXT14 */
	": bind-to-key ",					/* TEXT15 */
	"[No such function]",					/* TEXT16 */
	"Binding table FULL!",					/* TEXT17 */
	": unbind-key ",					/* TEXT18 */
	"[Key not bound]",					/* TEXT19 */
	"Apropos string: ",					/* TEXT20 */
	"Binding list",	/* TEXT21 */
	"Can not display binding list",	/* TEXT22 */
	"[Building binding list]",	/* TEXT23 */
	"Use buffer",	/* TEXT24 */
	"Use buffer: ",	/* TEXT25 */
	"Kill buffer",	/* TEXT26 */
	"Pop buffer",	/* TEXT27 */
	"Buffer is being displayed",	/* TEXT28 */
	"Change buffer name to: ",	/* TEXT29 */
#if	JMDEXT
	"ACTN   Modes         Size Buffer          File",	/* TEXT30 */
#else
	"ACTN   Modes        Size Buffer          File",	/* TEXT30 */
#endif
	"         Global Modes",	/* TEXT31 */
	"Discard changes",	/* TEXT32 */
	"Encryption String: ",	/* TEXT33 */
	"File: ",	/* TEXT34 */
	"another user",	/* TEXT35 */
	"LOCK ERROR -- ",	/* TEXT36 */
	"checking for existence of %s\n",	/* TEXT37 */
	"making directory %s\n",	/* TEXT38 */
	"creating %s\n",	/* TEXT39 */
	"could not create lock file",	/* TEXT40 */
	"pid is %ld\n",	/* TEXT41 */
	"reading lock file %s\n",	/* TEXT42 */
	"could not read lock file",	/* TEXT43 */
	"pid in %s is %ld\n",	/* TEXT44 */
	"signaling process %ld\n",	/* TEXT45 */
	"process exists",	/* TEXT46 */
	"kill was bad",	/* TEXT47 */
	"kill was good; process exists",	/* TEXT48 */
	"attempting to unlink %s\n",	/* TEXT49 */
	"could not remove lock file",	/* TEXT50 */
	"Variable to set: ",	/* TEXT51 */
	"%%No such variable as '%s'",	/* TEXT52 */
	"Value: ",	/* TEXT53 */
	"[Macro aborted]",	/* TEXT54 */
	"Variable to display: ",	/* TEXT55 */
	"Variable list",	/* TEXT56 */
	"Can not display variable list",	/* TEXT57 */
	"[Building variable list]",	/* TEXT58 */
	"",	/* TEXT59 */
#if	THEOX
	"Line %D/%D Col %d/%d Char %D/%D (%d%%) Unicode char = 0x%x %d, TheoX char = 0x%x %d",
#else
	"Line %D/%D Col %d/%d Char %D/%D (%d%%) char = 0x%x %d",/* TEXT60 */
#endif
	"<Not Used>",	/* TEXT61 */
	"Global mode to ",	/* TEXT62 */
	"Mode to ",	/* TEXT63 */
	"add: ",	/* TEXT64 */
	"delete: ",	/* TEXT65 */
	"No such mode!",	/* TEXT66 */
	"Message to write: ",	/* TEXT67 */
	"String to insert: ",	/* TEXT68 */
	"String to overwrite: ",	/* TEXT69 */
	"[region copied]",	/* TEXT70 */
	"%%This buffer is already narrowed",	/* TEXT71 */
	"%%Must narrow at least 1 full line",	/* TEXT72 */
	"[Buffer is narrowed]",	/* TEXT73 */
	"%%This buffer is not narrowed",	/* TEXT74 */
	"[Buffer is widened]",	/* TEXT75 */
	"No mark set in this window",	/* TEXT76 */
	"Bug: lost mark",	/* TEXT77 */
	"Search ",	/* TEXT78 */
	"Not found ",	/* TEXT79 */
	"No pattern set",	/* TEXT80 */
	"Reverse search ",	/* TEXT81 */
	"",	/* TEXT82 */
	"",	/* TEXT83 */
	"Replace ",	/* TEXT84 */
	"Query replace ",	/* TEXT85 */
	"with <C-G><C-K>",	/* TEXT86 */
	"Replace '",	/* TEXT87 */
	"' with '",	/* TEXT88 */
	"Aborted!",	/* TEXT89 */
	"(Y)es, (N)o, (!)Do rest, (U)ndo last, (^G)Abort, (.)Abort back, (?)Help: ",	/* TEXT90 */
	"Empty string replaced, stopping.",	/* TEXT91 */
	"%D substitutions",	/* TEXT92 */
	"%%ERROR while deleting",	/* TEXT93 */
	"%%Out of memory",	/* TEXT94 */
	"%%mceq: what is %d?",	/* TEXT95 */
	"%%No characters in character class",	/* TEXT96 */
	"%%Character class not ended",	/* TEXT97 */
	"No fill column set",	/* TEXT98 */
	"[OUT OF MEMORY]",	/* TEXT99 */
	"Words %D Chars %D Lines %d Avg chars/word %f",	/* TEXT100 */
	"[Can not search and goto at the same time!]",	/* TEXT101 */
	"[Bogus goto argument]",	/* TEXT102 */
	"[Saving %s]",	/* TEXT103 */
	"Modified buffers exist. Leave anyway",	/* TEXT104 */
	"%%Macro already active",	/* TEXT105 */
	"[Start macro]",	/* TEXT106 */
	"%%Macro not active",	/* TEXT107 */
	"[End macro]",	/* TEXT108 */
	"[Key illegal in VIEW mode]",	/* TEXT109 */
	"[That command is RESTRICTED]",	/* TEXT110 */
	"No macro specified",	/* TEXT111 */
	"Macro number out of range",	/* TEXT112 */
	"Can not create macro",	/* TEXT113 */
	"Procedure name: ",	/* TEXT114 */
	"Execute procedure: ",	/* TEXT115 */
	"No such procedure",	/* TEXT116 */
	"Execute buffer: ",	/* TEXT117 */
	"No such buffer",	/* TEXT118 */
	"%%Out of memory during while scan",	/* TEXT119 */
	"%%!BREAK outside of any !WHILE loop",	/* TEXT120 */
	"%%!ENDWHILE with no preceding !WHILE",	/* TEXT121 */
	"%%!WHILE with no matching !ENDWHILE",	/* TEXT122 */
	"%%Out of Memory during macro execution",	/* TEXT123 */
	"%%Unknown Directive",	/* TEXT124 */
	"Out of memory while storing macro",	/* TEXT125 */
	"%%Internal While loop error",	/* TEXT126 */
	"%%No such label",	/* TEXT127 */
	"(e)val exp, (c/x)ommand, (t)rack exp, (^G)abort, <SP>exec, <META> stop debug",	/* TEXT128 */
	"File to execute: ",	/* TEXT129 */
	"Macro not defined",	/* TEXT130 */
	"Read file",	/* TEXT131 */
	"Insert file",	/* TEXT132 */
	"Find file",	/* TEXT133 */
	"View file",	/* TEXT134 */
	"[Old buffer]",	/* TEXT135 */
	"Buffer name: ",	/* TEXT136 */
	"Cannot create buffer",	/* TEXT137 */
	"[New file]",	/* TEXT138 */
	"[Reading file]",	/* TEXT139 */
	"Read ",	/* TEXT140 */
	"I/O ERROR, ",	/* TEXT141 */
	"OUT OF MEMORY, ",	/* TEXT142 */
	" line",	/* TEXT143 */
	"Write file: ",	/* TEXT144 */
	"No file name",	/* TEXT145 */
	"Truncated file..write it out",	/* TEXT146 */
	"Narrowed Buffer..write it out",	/* TEXT147 */
	"[Writing...]",	/* TEXT148 */
	"[Wrote ",	/* TEXT149 */
	", saved as ",	/* TEXT150 */
	"Name: ",	/* TEXT151 */
	"[No such file]",	/* TEXT152 */
	"[Inserting file]",	/* TEXT153 */
	"Inserted ",	/* TEXT154 */
	"Cannot open file for writing",	/* TEXT155 */
	"Error closing file",	/* TEXT156 */
	"Write I/O error",	/* TEXT157 */
	"File read error",	/* TEXT158 */
	"%Need function key number",	/* TEXT159 */
	"%Function key number out of range",	/* TEXT160 */
	"Label contents: ",	/* TEXT161 */
	" [y/n]? ",	/* TEXT162 */
	"no default",	/* TEXT163 */
	"[search failed]",	/* TEXT164 */
	"ISearch: ",	/* TEXT165 */
	"? Search string too long",	/* TEXT166 */
	"? command too long",	/* TEXT167 */
	"%%Can not insert string",	/* TEXT168 */
	"Inserted"	/* this not used anymore */,	/* TEXT169 */
	"bug: linsert",	/* TEXT170 */
	"Replaced"	/* this not used anymore */,	/* TEXT171 */
	"%%Out of memory while overwriting",	/* TEXT172 */
	"LOCK ERROR: Lock table full",	/* TEXT173 */
	"Cannot lock, out of memory",	/* TEXT174 */
	"LOCK",	/* TEXT175 */
	"File in use by ",	/* TEXT176 */
	", overide?",	/* TEXT177 */
	"[can not get system error message]",	/* TEXT178 */
	"  About MicroEmacs",	/* TEXT179 */
	"%%No such resolution",	/* TEXT180 */
	"%%Resolution illegal for this monitor",	/* TEXT181 */
	"Environment variable TERM not defined!",	/* TEXT182 */
	"Unknown terminal type %s!",	/* TEXT183 */
	"termcap entry incomplete (lines)",	/* TEXT184 */
	"Termcap entry incomplete (columns)",	/* TEXT185 */
	"Incomplete termcap entry\n",	/* TEXT186 */
	"Terminal description too big!\n",	/* TEXT187 */
	"[End]",	/* TEXT188 */
	"Cannot find entry for terminal type.\n",	/* TEXT189 */
	"Check terminal type with \"SHOW TERMINAL\" or\n",	/* TEXT190 */
	"try setting with \"SET TERMINAL/INQUIRE\"\n",	/* TEXT191 */
	"The terminal type does not have enough power to run\n",	/* TEXT192 */
	"MicroEMACS.  Try a different terminal or check\n",	/* TEXT193 */
	"type with \"SHOW TERMINAL\".\n",	/* TEXT194 */
	"Cannot open channel to terminal.\n",	/* TEXT195 */
	"Cannot obtain terminal settings.\n",	/* TEXT196 */
	"Cannot modify terminal settings.\n",	/* TEXT197 */
	"I/O error (%d,%d)\n",	/* TEXT198 */
	"[Starting DCL]\r\n",	/* TEXT199 */
	"[Calling DCL]\r\n",	/* TEXT200 */
	"[Not available yet under VMS]",	/* TEXT201 */
	"Terminal type not 'vt52'or 'z19' !",	/* TEXT202 */
	"Window number out of range",	/* TEXT203 */
	"Can not delete this window",	/* TEXT204 */
	"Cannot split a %d line window",	/* TEXT205 */
	"Only one window",	/* TEXT206 */
	"Impossible change",	/* TEXT207 */
	"[No such window exists]",	/* TEXT208 */
	"%%Screen size out of range",	/* TEXT209 */
	"%%Screen width out of range",	/* TEXT210 */
	"Function list",	/* TEXT211 */
	"Can not display function list",	/* TEXT212 */
	"[Building function list]",	/* TEXT213 */
	"%%No such file as %s",	/* TEXT214 */
	": macro-to-key ",	/* TEXT215 */
	"Cannot read/write directories!!!",	/* TEXT216 */
	"[Not available yet under AOS/VS]",	/* TEXT217 */
	"Append file: ",	/* TEXT218 */
	"%%Macro Failed",	/* TEXT219 */
	"Line %D/%D Col %d/%d Char %D/%D (%d%%) char = 0x%x%x",	/* TEXT220 */
	"Too many groups",	/* TEXT221 */
	"Group not ended",	/* TEXT222 */
	"%%Column origin out of range",	/* TEXT223 */
	"%%Row origin out of range",	/* TEXT224 */
	"[Switched to screen %s]",	/* TEXT225 */
	"%%Can not kill an executing buffer",	/* TEXT226 */
	"\n--- Press any key to Continue ---",	/* TEXT227 */
	"[Kill ring cleared]",	/* TEXT228 */
	" in < ",	/* TEXT229 */
	"> at line ",	/* TEXT230 */
	"Abbreviation to set: ",	/* TEXT231 */
	"Abbreviation to delete: ",	/* TEXT232 */
	"[Building Abbreviation list]",	/* TEXT233 */
	"Abbreviation list",	/* TEXT234 */
	"Can not display abbreviation list",	/* TEXT235 */
	"Define Abbreviations from buffer",	/* TEXT236 */
	"",	/* TEXT237 */
	"",	/* TEXT238 */
	"",	/* TEXT239 */
	"[No such screen]",	/* TEXT240 */
	"%%Can't delete current screen",	/* TEXT241 */
	"Find Screen: ",	/* TEXT242 */
	"Delete Screen: ",	/* TEXT243 */
	"%%No such function as '%s'",	/* TEXT244 */
	"%%Division by Zero is illegal",	/* TEXT245 */
	"%%Need function key number",	/* TEXT246 */
	"%%Function key number out of range",	/* TEXT247 */
	"Enter Label String: ",	/* TEXT248 */
	"Global variable to declare: ",	/* TEXT249 */
	"Local variable to declare: ",	/* TEXT250 */
	"",	/* TEXT251 */
	"",	/* TEXT252 */
	"",	/* TEXT253 */
	"",	/* TEXT254 */
	"",	/* TEXT255 */
	"",	/* TEXT256 */
	"",	/* TEXT257 */
	"",	/* TEXT258 */
	"",	/* TEXT259 */
	"",	/* TEXT260 */
	"",	/* TEXT261 */
	"",	/* TEXT262 */
	"",	/* TEXT263 */
	"",	/* TEXT264 */
	"",	/* TEXT265 */
	"",	/* TEXT266 */
	"",	/* TEXT267 */
	"",	/* TEXT268 */
	"",	/* TEXT269 */
	"",	/* TEXT270 */
	"",	/* TEXT271 */
	"",	/* TEXT272 */
	"",	/* TEXT273 */
	"",	/* TEXT274 */
	"",	/* TEXT275 */
	"",	/* TEXT276 */
	"",	/* TEXT277 */
	"",	/* TEXT278 */
	"",	/* TEXT279 */
	"",	/* TEXT280 */
	"",	/* TEXT281 */
	"",	/* TEXT282 */
	"",	/* TEXT283 */
	"",	/* TEXT284 */
#if	JMDEXT
	"No topic",						/* TEXT285 */
	"Searching topic \"%s\"",				/* TEXT286 */
	"Searching keyword \"%s\"",				/* TEXT287 */
	"Topic: ",						/* TEXT288 */
	"Keyword: ",						/* TEXT289 */
	"No help for topic \"%s\"",				/* TEXT290 */
	"No help for keyword \"%s\"",				/* TEXT291 */
	"Unable to open the help file %s",			/* TEXT292 */
	"Index of ",	/* TEXT293 */
	"[Help]",	/* TEXT294 */
	"[Errors]",	/* TEXT295 */
	"[Function Keys]",	/* TEXT296 */
	"%s is not a regular file",	/* TEXT297 */
#else
	"",	/* TEXT285 */
	"",	/* TEXT286 */
	"",	/* TEXT287 */
	"",	/* TEXT288 */
	"",	/* TEXT289 */
	"",	/* TEXT290 */
	"",	/* TEXT291 */
	"",	/* TEXT292 */
	"",	/* TEXT293 */
	"",	/* TEXT294 */
	"",	/* TEXT295 */
	"",	/* TEXT296 */
	"",	/* TEXT297 */
#endif
#if	THEOS
	"%s is not a sequential file",	/* TEXT298 */
	"%s is not a library",	/* TEXT299 */
#else
	"",
	"",
#endif
	"[Incorrect menu]",	/* TEXT300 */
	"[Too many nested popup menus]",	/* TEXT301 */
	"[Lack of resources]",	/* TEXT302 */
	"Menu: ",	/* TEXT303 */
	"Function: ",	/* TEXT304 */
	"Macro: ",	/* TEXT305 */
	"Menu: ",	/* TEXT306 */
	"Help file: ",	/* TEXT307 */
	"Help key: ",	/* TEXT308 */
	"",		/* TEXT309 */
	"Alt+",		/* TEXT310 */
	"Shift+",	/* TEXT311 */
	"BkSp",		/* TEXT312 */
	"Tab",		/* TEXT313 */
	"Enter",	/* TEXT314 */
	"Esc",		/* TEXT315 */
	"Ctrl+",	/* TEXT316 */
	"Home",		/* TEXT317 */
	"DownArrow",	/* TEXT318 */
	"UpArrow",	/* TEXT319 */
	"LeftArrow",	/* TEXT320 */
	"RightArrow",	/* TEXT321 */
	"End",		/* TEXT322 */
	"PageUp",	/* TEXT323 */
	"PageDown",	/* TEXT324 */
	"Ins",		/* TEXT325 */
	"Del",		/* TEXT326 */
	"F10",		/* TEXT327 */
	"F",		/* CHAR328 */
	"SpaceBar",	/* TEXT329 */
	" - Message history",	/* TEXT330 */
	"Global modes",	/* TEXT331 */
	"Modes for buffer: ",	// TEXT332
	"File write in progress. Quit later!",	/* TEXT333 */
	"[No such directory]",	/* TEXT334 */
	"Change screen name to: ",	/* TEXT335 */
	"[Screen name already in use]",	/* TEXT336 */
	"cannot monitor external program",	/* TEXT337 */
	"Internal command to execute (space for help):"			/* TEXT338 */
};

VOID msinit(VOID)
{
	FILE *menufile;		/* message file */
	char s[NSTRING];
	char *p, *q;
	int i;

#if	THEOS
	if ((menufile = openmenu(otherpath[0])) == NULL)
		return;
#else
#if BSD || FREEBSD || USG || LINUX || AIX || AUX || SMOS || HPUX8 || HPUX9 || SUN || XENIX
	if ((p = getenv("LANG"))) {
		q = strchr(p, '_');
		if (q)
			*q = '\0';
		snprintf(s, sizeof(s), "emacs%s.msg", p);
	}
	if ((menufile = fopen(flook(s, FALSE), "r")) == NULL
	 && (menufile = fopen(flook(otherpath[0], FALSE), "r")) == NULL)
		return;
# else
	if ((menufile = fopen(flook(otherpath[0], FALSE), "r")) == NULL)
		return;
# endif
#endif
	for (i = 0; fgets(s,NSTRING-1,menufile) != NULL && i < sizeof(msg) / sizeof(char*); i++) {
		if ((p = strchr(s,'\n')))
			*p = '\0';
		if (! (msg[i] = strdup(s))) {
#if	THEOS
			syserr(8, ENOMEM, NULL);
#else
#if WINDOW_MSWIN || WINDOW_MSWIN32
			MessageBox(NULL, otherpath[0], "Out of memory", MB_ICONSTOP | MB_OK);
#else
			perror(otherpath[0]);
#endif
			exit(8);
#endif
		}
	}
	fclose(menufile);
}

CONST char *ldmsg(int n)
{
	static char m[NSTRING];

	if (n < (sizeof(msg) / sizeof(char*)))
		return msg[n];

	snprintf(m, sizeof(m),"message %d missing", n);
	return m;
}
#else
msgmello()	/* dummy function */
{
}
#endif	/* DYNMSGS */
