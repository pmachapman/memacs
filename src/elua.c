/*	ELUA.C:	Lua scripting for MicroEMACS

	This file embeds Lua 5.4 as MicroEMACS's main scripting language.
	Scripts reach the editor through the global table "me". See
	doc/lua.md for the full description of the interface.

	Lua sits alongside the original procedure language: .cmd files,
	store-procedure and the rest still work, and each language can
	call commands written in the other.
*/

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<ctype.h>
#include	"estruct.h"

#if	LUASCRIPT

#include	"lua/lua.h"
#include	"lua/lauxlib.h"
#include	"lua/lualib.h"
#include	"eproto.h"
#include	"edef.h"
#include	"elang.h"

#define	CMDTABLE	"me.commands"	/* registry key: name -> function */
#define	HIDDEN		' '		/* first char of internal command names */

static lua_State *Lmain = NULL;	/* the interpreter */
static lua_State *curL = NULL;	/* thread that most recently called into C */
static int anon_count = 0;	/* counter for anonymous key bindings */

/* Names given to KEYTAB entries bound to Lua commands. A key binding only
   holds a pointer, so names are kept here for the life of the program. */

typedef struct LNAME {
	struct LNAME *next;
	char name[1];
} LNAME;

static LNAME *lnames = NULL;

static char *intern(const char *s)
{
	LNAME *ln;

	for (ln = lnames; ln; ln = ln->next)
		if (strcmp(ln->name, s) == 0)
			return(ln->name);
	ln = (LNAME *)malloc(sizeof(LNAME) + strlen(s));
	if (ln == NULL)
		return(NULL);
	strcpy(ln->name, s);
	ln->next = lnames;
	lnames = ln;
	return(ln->name);
}

char *PASCAL NEAR elua_intern(char *name)
{
	return(intern(name));
}

static lua_State *thread(void)
{
	return(curL ? curL : Lmain);
}

/* ------------------------------------------------------------------ */
/*	Running Lua code						*/
/* ------------------------------------------------------------------ */

/* Count hook: lets ^G break out of a script stuck in a loop. Any other
   key typed meanwhile is pushed back for the editor to read later. */

#if	XPCON
extern int PASCAL NEAR ttpoll();	/* in xpconio.c */
#define	keywaiting()	ttpoll()
#else
#define	keywaiting()	typahead()
#endif

static void interrupt_hook(lua_State *L, lua_Debug *ar)
{
#if	TYPEAH
	int c;

	(void)ar;
	if (cpending || keywaiting() == FALSE)
		return;
	c = TTgetc();
	if (ctoec(c) == abortc)
		luaL_error(L, "interrupted");
	cpending = TRUE;
	charpending = c;
#else
	(void)L;
	(void)ar;
#endif
}

static int msghandler(lua_State *L)
{
	const char *msg = lua_tostring(L, 1);

	if (msg == NULL) {
		if (luaL_callmeta(L, 1, "__tostring") &&
		    lua_type(L, -1) == LUA_TSTRING)
			return(1);
		lua_pushfstring(L, "(error object is a %s value)",
				luaL_typename(L, 1));
	}
	return(1);
}

/* Show a Lua error on the message line */

static VOID report(lua_State *L)
{
	const char *msg;
	char buf[NSTRING];

	msg = lua_tostring(L, -1);
	if (msg == NULL)
		msg = "(no error message)";
	strcpy(buf, "%Lua: ");
	strncat(buf, msg, NSTRING - strlen(buf) - 1);
	mlforce(buf);
	lua_pop(L, 1);
}

/* Call the function below nargs arguments on the stack.
   Returns TRUE, or reports the error and returns FALSE. */

static int docall(lua_State *L, int nargs, int nres)
{
	int base;
	int status;
	lua_State *oldL;

	base = lua_gettop(L) - nargs;
	lua_pushcfunction(L, msghandler);
	lua_insert(L, base);
	oldL = curL;
	curL = L;
	status = lua_pcall(L, nargs, nres, base);
	curL = oldL;
	lua_remove(L, base);
	if (status != LUA_OK) {
		report(L);
		return(FALSE);
	}
	return(TRUE);
}

/* Is the named file a Lua script? (judged by a .lua extension) */

int PASCAL NEAR elua_isluafile(char *fname)
{
	int len;

	if (fname == NULL)
		return(FALSE);
	len = strlen(fname);
	if (len < 4)
		return(FALSE);
	fname += len - 4;
	return(fname[0] == '.' &&
	       tolower((unsigned char)fname[1]) == 'l' &&
	       tolower((unsigned char)fname[2]) == 'u' &&
	       tolower((unsigned char)fname[3]) == 'a');
}

/* Name of the Lua startup file: emacs.lua, or .emacs.lua where the
   classic startup file is .emacsrc */

char *PASCAL NEAR elua_startname()
{
	NOSHARE CONST extern char *pathname[];

	return(pathname[0][0] == '.' ? ".emacs.lua" : "emacs.lua");
}

int PASCAL NEAR elua_dofile(char *fname)
{
	lua_State *L = thread();

	if (L == NULL)
		return(FALSE);
	if (luaL_loadfile(L, fname) != LUA_OK) {
		report(L);
		return(FALSE);
	}
	return(docall(L, 0, 0));
}

/* Run the text of a buffer as a Lua chunk */

int PASCAL NEAR elua_dobuf(BUFFER *bp)
{
	lua_State *L = thread();
	luaL_Buffer b;
	LINE *lp;
	char chunkname[NBUFN + 2];

	if (L == NULL)
		return(FALSE);
	luaL_buffinit(L, &b);
	for (lp = lforw(bp->b_linep); lp != bp->b_linep; lp = lforw(lp)) {
		luaL_addlstring(&b, ltext(lp), lused(lp));
		luaL_addchar(&b, '\n');
	}
	luaL_pushresult(&b);
	strcpy(chunkname, "=");
	strcat(chunkname, bp->b_bname);
	if (luaL_loadbuffer(L, lua_tostring(L, -1), lua_rawlen(L, -1),
			    chunkname) != LUA_OK) {
		lua_remove(L, -2);
		report(L);
		return(FALSE);
	}
	lua_remove(L, -2);
	return(docall(L, 0, 0));
}

/* ------------------------------------------------------------------ */
/*	Lua commands							*/
/* ------------------------------------------------------------------ */

/* push the command table */

static VOID pushcmds(lua_State *L)
{
	lua_getfield(L, LUA_REGISTRYINDEX, CMDTABLE);
}

int PASCAL NEAR elua_iscommand(char *name)
{
	lua_State *L = thread();
	int found;

	if (L == NULL || name == NULL || *name == 0)
		return(FALSE);
	pushcmds(L);
	found = (lua_getfield(L, -1, name) == LUA_TFUNCTION);
	lua_pop(L, 2);
	return(found);
}

/* Run a Lua command. It gets (n, f): the numeric argument and whether
   one was given. A false return means the command failed. */

int PASCAL NEAR elua_runcmd(char *name, int f, int n)
{
	lua_State *L = thread();
	int status;

	if (L == NULL)
		return(FALSE);
	pushcmds(L);
	if (lua_getfield(L, -1, name) != LUA_TFUNCTION) {
		lua_pop(L, 2);
		mlwrite(TEXT16);
/*			"[No such function]" */
		return(FALSE);
	}
	lua_remove(L, -2);
	lua_pushinteger(L, n);
	lua_pushboolean(L, f);
	if (docall(L, 2, 1) == FALSE)
		return(FALSE);
	status = (lua_isnil(L, -1) || lua_toboolean(L, -1)) ? TRUE : FALSE;
	lua_pop(L, 1);
	return(status);
}

/* add the Lua commands to a describe-bindings/apropos list */

int PASCAL NEAR elua_buildlist(BUFFER *listbuf, int type, char *mstring)
{
	lua_State *L = thread();
	KEYTAB *ktp;
	const char *name;
	char cname[80];
	char outseq[NSTRING];
	int cpos;
	int i, count;

	if (L == NULL)
		return(TRUE);

	/* make a sorted list of the visible names */
	lua_newtable(L);
	pushcmds(L);
	count = 0;
	lua_pushnil(L);
	while (lua_next(L, -2)) {
		lua_pop(L, 1);
		name = lua_tostring(L, -1);
		if (name[0] != HIDDEN) {
			lua_pushvalue(L, -1);
			lua_rawseti(L, -4, ++count);
		}
	}
	lua_pop(L, 1);
	lua_getglobal(L, "table");
	lua_getfield(L, -1, "sort");
	lua_remove(L, -2);
	lua_pushvalue(L, -2);
	lua_call(L, 1, 0);

	if (count > 0 && addline(listbuf, "") != TRUE) {
		lua_pop(L, 1);
		return(FALSE);
	}

	for (i = 1; i <= count; i++) {
		lua_rawgeti(L, -1, i);
		strncpy(cname, lua_tostring(L, -1), sizeof(cname) - 1);
		cname[sizeof(cname) - 1] = 0;
		lua_pop(L, 1);
		if (type == FALSE && strinc(cname, mstring) == FALSE)
			continue;
		strcpy(outseq, cname);
		strcat(outseq, " (Lua)");
		cpos = strlen(outseq);

		/* list each key bound to this command */
		for (ktp = &keytab[0]; ktp->k_type != BINDNUL; ++ktp) {
			if (ktp->k_type == BINDLUA &&
			    strcmp(ktp->k_ptr.lname, cname) == 0) {
				while (cpos < 25)
					outseq[cpos++] = ' ';
				cmdstr(ktp->k_code, &outseq[cpos]);
				if (addline(listbuf, outseq) != TRUE) {
					lua_pop(L, 1);
					return(FALSE);
				}
				cpos = 0;
			}
		}

		/* if no key was bound, list it anyway */
		if (cpos > 0) {
			outseq[cpos] = 0;
			if (addline(listbuf, outseq) != TRUE) {
				lua_pop(L, 1);
				return(FALSE);
			}
		}
	}
	lua_pop(L, 1);
	return(TRUE);
}

/* ------------------------------------------------------------------ */
/*	Key bindings							*/
/* ------------------------------------------------------------------ */

/* translate a key name (in MicroEMACS form, like "^XF" or "M-A") */

static unsigned int keycode(lua_State *L, int idx)
{
	char kname[NSTRING];
	const char *s;

	s = luaL_checkstring(L, idx);
	if (*s == 0 || strlen(s) >= NSTRING)
		luaL_argerror(L, idx, "bad key name");
	strcpy(kname, s);
	return(stock((unsigned char *)kname));
}

/* point a KEYTAB entry at a command. Returns FALSE if no such command. */

static int setkeytab(lua_State *L, KEYTAB *ktp, int idx)
{
	const char *name;
	char *lname;
	char anon[32];
	int (PASCAL NEAR *fp)();
	BUFFER *bp;
	char bufn[NBUFN + 2];

	if (lua_isfunction(L, idx)) {
		/* store it as a hidden command */
		sprintf(anon, "%cfn%d", HIDDEN, ++anon_count);
		pushcmds(L);
		lua_pushvalue(L, idx);
		lua_setfield(L, -2, anon);
		lua_pop(L, 1);
		if ((lname = intern(anon)) == NULL)
			luaL_error(L, "out of memory");
		ktp->k_type = BINDLUA;
		ktp->k_ptr.lname = lname;
		return(TRUE);
	}

	name = luaL_checkstring(L, idx);
	if ((fp = fncmatch((char *)name)) != NULL) {
		ktp->k_type = BINDFNC;
		ktp->k_ptr.fp = fp;
		return(TRUE);
	}
	if (elua_iscommand((char *)name)) {
		if ((lname = intern(name)) == NULL)
			luaL_error(L, "out of memory");
		ktp->k_type = BINDLUA;
		ktp->k_ptr.lname = lname;
		return(TRUE);
	}
	if (strlen(name) < NBUFN - 2) {
		strcpy(bufn, "[");
		strcat(bufn, name);
		strcat(bufn, "]");
		if ((bp = bfind(bufn, FALSE, 0)) != NULL) {
			ktp->k_type = BINDBUF;
			ktp->k_ptr.buf = bp;
			return(TRUE);
		}
	}
	return(FALSE);
}

/* me.bind(key, command_name_or_function) */

static int l_bind(lua_State *L)
{
	unsigned int c;
	KEYTAB *ktp;
	KEYTAB entry;

	c = keycode(L, 1);
	luaL_argcheck(L, lua_isfunction(L, 2) || lua_isstring(L, 2), 2,
		      "command name or function expected");
	if (setkeytab(L, &entry, 2) == FALSE)
		return(luaL_error(L, "no such command: %s", lua_tostring(L, 2)));

	for (ktp = &keytab[0]; ktp->k_type != BINDNUL; ++ktp)
		if (ktp->k_code == c)
			break;
	if (ktp->k_type == BINDNUL) {
		if (ktp >= &keytab[NBINDS])
			return(luaL_error(L, "binding table full"));
		ktp[1].k_code = 0;
		ktp[1].k_type = BINDNUL;
		ktp[1].k_ptr.fp = NULL;
	}
	ktp->k_code = c;
	ktp->k_type = entry.k_type;
	ktp->k_ptr = entry.k_ptr;
	return(0);
}

/* me.unbind(key) */

static int l_unbind(lua_State *L)
{
	lua_pushboolean(L, unbindchar(keycode(L, 1)));
	return(1);
}

/* me.binding(key): name of the command bound to key, or nil */

static int l_binding(lua_State *L)
{
	char *name;

	name = getfname(getbind(keycode(L, 1)));
	if (name == NULL)
		lua_pushnil(L);
	else if (name[0] == HIDDEN)
		lua_pushliteral(L, "a Lua function");
	else
		lua_pushstring(L, name);
	return(1);
}

/* me.runkey(key, [n]): run whatever is bound to a key */

static int l_runkey(lua_State *L)
{
	KEYTAB *ktp;
	int f = FALSE, n = 1;

	ktp = getbind(keycode(L, 1));
	if (!lua_isnoneornil(L, 2)) {
		f = TRUE;
		n = (int)luaL_checkinteger(L, 2);
	}
	if (ktp == NULL) {
		lua_pushboolean(L, FALSE);
		return(1);
	}
	lua_pushboolean(L, execkey(ktp, f, n) == TRUE);
	return(1);
}

/* me.findfile(name): look a file up along the startup file path */

static int l_findfile(lua_State *L)
{
	char name[NSTRING];
	char *found;

	strncpy(name, luaL_checkstring(L, 1), NSTRING - 1);
	name[NSTRING - 1] = 0;
	found = flook(name, TRUE);
	if (found == NULL)
		lua_pushnil(L);
	else
		lua_pushstring(L, found);
	return(1);
}

/* me._sethook(var, fn): point a hook ($readhook etc.) at a Lua function,
   or with nil, back to its default. */

static int l_sethook(lua_State *L)
{
	static CONST char *names[] = {"bufhook", "cmdhook", "exbhook",
		"exithook", "readhook", "wraphook", "writehook", NULL};
	KEYTAB *hooks[7];
	char hname[32];
	char *lname;
	int i;

	hooks[0] = &bufhook;
	hooks[1] = &cmdhook;
	hooks[2] = &exbhook;
	hooks[3] = &exithook;
	hooks[4] = &readhook;
	hooks[5] = &wraphook;
	hooks[6] = &writehook;
	i = luaL_checkoption(L, 1, NULL, names);

	sprintf(hname, "%chook:%s", HIDDEN, names[i]);
	pushcmds(L);
	if (lua_isnoneornil(L, 2)) {
		lua_pushnil(L);
		lua_setfield(L, -2, hname);
		hooks[i]->k_type = BINDFNC;
		hooks[i]->k_ptr.fp = (hooks[i] == &wraphook) ? wrapword : nullproc;
	} else {
		luaL_checktype(L, 2, LUA_TFUNCTION);
		lua_pushvalue(L, 2);
		lua_setfield(L, -2, hname);
		if ((lname = intern(hname)) == NULL)
			return(luaL_error(L, "out of memory"));
		hooks[i]->k_type = BINDLUA;
		hooks[i]->k_ptr.lname = lname;
	}
	return(0);
}

/* me._cancelexit(): called by an exit hook to stay in the editor */

static int l_cancelexit(lua_State *L)
{
	(void)L;
	gflags &= ~GFEXIT;
	return(0);
}

/* ------------------------------------------------------------------ */
/*	Commands							*/
/* ------------------------------------------------------------------ */

/* me.defcommand(name, fn): define (or with nil, remove) a command */

static int l_defcommand(lua_State *L)
{
	const char *name;

	name = luaL_checkstring(L, 1);
	luaL_argcheck(L, *name != 0 && *name != HIDDEN &&
		      strlen(name) < NSTRING, 1, "bad command name");
	if (!lua_isnoneornil(L, 2))
		luaL_checktype(L, 2, LUA_TFUNCTION);
	if (fncmatch((char *)name) != NULL)
		return(luaL_error(L, "%s is a built-in command", name));
	pushcmds(L);
	lua_pushvalue(L, 2);
	lua_setfield(L, -2, name);
	return(0);
}

/* append a string to an argument line, quoted the way token() reads it */

static VOID addarg(luaL_Buffer *b, const char *s, size_t len)
{
	luaL_addchar(b, '"');
	while (len--) {
		switch (*s) {
			case '~':	luaL_addstring(b, "~~"); break;
			case '"':	luaL_addstring(b, "~\""); break;
			case '\n':	luaL_addstring(b, "~n"); break;
			case '\r':	luaL_addstring(b, "~r"); break;
			default:	luaL_addchar(b, *s);
		}
		++s;
	}
	luaL_addstring(b, "\" ");
}

/* me.command(name, [n], replies...): run any command */

static int l_command(lua_State *L)
{
	const char *name;
	int (PASCAL NEAR *fp)();
	BUFFER *bp;
	int f, n, first, nargs, i;
	int status;
	int oldcle;
	char *oldestr;
	char *args;
	size_t len;
	const char *s;
	luaL_Buffer b;
	char bufn[NBUFN + 2];

	name = luaL_checkstring(L, 1);
	nargs = lua_gettop(L);
	f = FALSE;
	n = 1;
	first = 2;
	if (nargs >= 2 && lua_type(L, 2) == LUA_TNUMBER) {
		f = TRUE;
		n = (int)luaL_checkinteger(L, 2);
		first = 3;
	}

	/* find the command */
	bp = NULL;
	fp = fncmatch((char *)name);
	if (fp == NULL && !elua_iscommand((char *)name)) {
		if (strlen(name) < NBUFN - 2) {
			strcpy(bufn, "[");
			strcat(bufn, name);
			strcat(bufn, "]");
			bp = bfind(bufn, FALSE, 0);
		}
		if (bp == NULL)
			return(luaL_error(L, "no such command: %s", name));
	}

	/* Replies to the command's prompts are passed the way a
	   command line would pass them */
	args = NULL;
	if (nargs >= first) {
		luaL_buffinit(L, &b);
		for (i = first; i <= nargs; i++) {
			s = luaL_tolstring(L, i, &len);
			lua_pop(L, 1);
			addarg(&b, s, len);
		}
		luaL_pushresult(&b);
		s = lua_tolstring(L, -1, &len);
		args = (char *)malloc(len + 1);
		if (args == NULL)
			return(luaL_error(L, "out of memory"));
		memcpy(args, s, len + 1);
		lua_pop(L, 1);
	}

	oldcle = clexec;
	oldestr = execstr;
	if (args) {
		clexec = TRUE;
		execstr = args;
	} else
		clexec = FALSE;
	lastflag = thisflag;
	thisflag = 0;

	if (fp != NULL) {
		status = (*fp)(f, n);
	} else if (bp == NULL) {
		status = elua_runcmd((char *)name, f, n);
	} else {
		clexec = TRUE;
		if (args == NULL)
			execstr = "";
		status = TRUE;
		while (n-- > 0)
			if ((status = dobuf(bp)) != TRUE)
				break;
	}

	cmdstatus = status;
	clexec = oldcle;
	execstr = oldestr;
	if (args)
		free(args);
	lua_pushboolean(L, status == TRUE);
	return(1);
}

/* me._builtins(): list of the built-in command names */

static int l_builtins(lua_State *L)
{
	int i;

	lua_createtable(L, numfunc, 0);
	for (i = 0; i < numfunc; i++) {
		lua_pushstring(L, names[i].n_name);
		lua_rawseti(L, -2, i + 1);
	}
	return(1);
}

/* me._luacommands(): list of the Lua command names */

static int l_luacommands(lua_State *L)
{
	const char *name;
	int count = 0;

	lua_newtable(L);
	pushcmds(L);
	lua_pushnil(L);
	while (lua_next(L, -2)) {
		lua_pop(L, 1);
		name = lua_tostring(L, -1);
		if (name[0] != HIDDEN) {
			lua_pushvalue(L, -1);
			lua_rawseti(L, -4, ++count);
		}
	}
	lua_pop(L, 1);
	return(1);
}

/* ------------------------------------------------------------------ */
/*	Variables							*/
/* ------------------------------------------------------------------ */

static CONST char *boolvars[] = {
	"abbell", "abcap", "abquick", "debug", "diagflag", "discmd",
	"disinp", "disphigh", "dispundo", "flicker", "hscrlbar", "hscroll",
	"modeflag", "msflag", "newscreen", "pending", "popflag", "popwait",
	"posflag", "ssave", "sscroll", "status", "timeflag", "undoflag",
	"vscrlbar", "yankflag", NULL
};

static CONST char *strvars[] = {
	"bufhook", "cbufname", "cfname", "cmdhook", "deskcolor", "exbhook",
	"exithook", "fmtlead", "isterm", "kill", "language", "lastmesg",
	"line", "lterm", "match", "os", "palette", "paralead", "progname",
	"readhook", "region", "replace", "rval", "scrname", "search",
	"sres", "sterm", "time", "version", "wchars", "wraphook",
	"writehook", NULL
};

static int inlist(CONST char **list, const char *name)
{
	while (*list)
		if (strcmp(*list++, name) == 0)
			return(TRUE);
	return(FALSE);
}

/* push an editor value that looks like a number as a Lua integer */

static VOID pushvalue(lua_State *L, const char *value)
{
	char *end;
	long num;

	if (*value != 0 && (isdigit((unsigned char)*value) ||
	    (*value == '-' && isdigit((unsigned char)value[1])))) {
		num = strtol(value, &end, 10);
		if (*end == 0) {
			lua_pushinteger(L, num);
			return;
		}
	}
	lua_pushstring(L, value);
}

/* convert a Lua value into an editor string */

static const char *tovalue(lua_State *L, int idx)
{
	if (lua_isboolean(L, idx))
		return(lua_toboolean(L, idx) ? "TRUE" : "FALSE");
	if (lua_isinteger(L, idx) || lua_type(L, idx) == LUA_TSTRING)
		return(lua_tostring(L, idx));
	if (lua_type(L, idx) == LUA_TNUMBER) {
		lua_pushinteger(L, (lua_Integer)lua_tonumber(L, idx));
		lua_replace(L, idx);
		return(lua_tostring(L, idx));
	}
	luaL_argerror(L, idx, "string, number or boolean expected");
	return(NULL);
}

/* me.var.name */

static int l_getvar(lua_State *L)
{
	const char *name;
	char *value;
	char vname[NVSIZE + 2];

	name = luaL_checkstring(L, 2);
	if (strlen(name) > NVSIZE)
		return(luaL_error(L, "no such variable: %s", name));
	strcpy(vname, name);
	value = gtenv(vname);
	if (value == errorm || value == NULL)
		return(luaL_error(L, "no such variable: %s", name));
	if (inlist(boolvars, name))
		lua_pushboolean(L, strcmp(value, truem) == 0);
	else if (inlist(strvars, name))
		lua_pushstring(L, value);
	else
		pushvalue(L, value);
	return(1);
}

/* set an editor variable ("$name" or "%name") */

static int setvarnamed(lua_State *L, const char *prefix, int scope)
{
	const char *name;
	const char *value;
	char vname[NVSIZE + 2];
	char vbuf[NSTRING];
	VDESC vd;

	name = luaL_checkstring(L, 2);
	value = tovalue(L, 3);
	if (strlen(name) > NVSIZE)
		return(luaL_error(L, "variable name too long: %s", name));
	strcpy(vname, prefix);
	strcat(vname, name);
	findvar(vname, &vd, NVSIZE + 1, scope);
	if (vd.v_type == -1)
		return(luaL_error(L, "no such variable: %s", name));
	strncpy(vbuf, value, NSTRING - 1);
	vbuf[NSTRING - 1] = 0;
	if (svar(&vd, vbuf) != TRUE)
		return(luaL_error(L, "can not set variable %s", name));
	return(0);
}

static int l_setvar(lua_State *L)
{
	return(setvarnamed(L, "$", VT_GLOBAL));
}

/* me.uvar.name: the old language's %user variables */

static int l_getuvar(lua_State *L)
{
	const char *name;
	char *value;
	char vname[NVSIZE + 2];

	name = luaL_checkstring(L, 2);
	strncpy(vname, name, NVSIZE);
	vname[NVSIZE] = 0;
	value = gtusr(vname);
	if (value == errorm || value == NULL)
		lua_pushnil(L);
	else
		lua_pushstring(L, value);
	return(1);
}

static int l_setuvar(lua_State *L)
{
	return(setvarnamed(L, "%", VT_GLOBAL));
}

/* me.eval(expr): evaluate an expression in the old language */

static int l_eval(lua_State *L)
{
	const char *expr;
	char *oldestr;
	char *copy;
	int status;
	char result[NSTRING];

	expr = luaL_checkstring(L, 1);
	copy = (char *)malloc(strlen(expr) + 1);
	if (copy == NULL)
		return(luaL_error(L, "out of memory"));
	strcpy(copy, expr);
	oldestr = execstr;
	execstr = copy;
	status = macarg(result);
	execstr = oldestr;
	free(copy);
	if (status != TRUE || strcmp(result, errorm) == 0)
		lua_pushnil(L);
	else
		lua_pushstring(L, result);
	return(1);
}

/* ------------------------------------------------------------------ */
/*	Text								*/
/* ------------------------------------------------------------------ */

/* me.insert(s, ...) */

static int l_insert(lua_State *L)
{
	int i, nargs;
	size_t len, j;
	const char *s;
	char *copy;
	int status = TRUE;

	nargs = lua_gettop(L);
	for (i = 1; i <= nargs && status == TRUE; i++) {
		s = luaL_tolstring(L, i, &len);
		if (len == 0) {
			lua_pop(L, 1);
			continue;
		}
		copy = (char *)malloc(len + 1);
		if (copy == NULL)
			return(luaL_error(L, "out of memory"));
		for (j = 0; j < len; j++)
			copy[j] = (s[j] == '\n') ? '\r' : s[j];
		copy[len] = 0;
		lua_pop(L, 1);
		status = linstr(copy);
		free(copy);
	}
	lua_pushboolean(L, status == TRUE);
	return(1);
}

/* me.delete(n): delete n characters forward, or backward if negative */

static int l_delete(lua_State *L)
{
	lua_Integer n;
	int status;

	n = luaL_optinteger(L, 1, 1);
	if (n < 0) {
		if ((status = backchar(TRUE, (int)-n)) == TRUE)
			status = ldelete((long)-n, FALSE);
	} else
		status = ldelete((long)n, FALSE);
	lua_pushboolean(L, status == TRUE);
	return(1);
}

/* me.line(): text of the current line */

static int l_line(lua_State *L)
{
	LINE *lp = curwp->w_dotp;

	if (lp == curbp->b_linep)
		lua_pushliteral(L, "");
	else
		lua_pushlstring(L, ltext(lp), lused(lp));
	return(1);
}

/* me.setline(s): replace the text of the current line */

static int l_setline(lua_State *L)
{
	const char *s;
	char buf[NSTRING];

	s = luaL_checkstring(L, 1);
	luaL_argcheck(L, strlen(s) < NSTRING, 1, "line too long");
	strcpy(buf, s);
	lua_pushboolean(L, putctext(buf) == TRUE);
	return(1);
}

/* me.char(): character at the cursor; "\n" at end of line, nil at end
   of buffer */

static int l_char(lua_State *L)
{
	LINE *lp = curwp->w_dotp;
	char c;

	if (lp == curbp->b_linep)
		lua_pushnil(L);
	else if (curwp->w_doto >= lused(lp))
		lua_pushliteral(L, "\n");
	else {
		c = lgetc(lp, curwp->w_doto);
		lua_pushlstring(L, &c, 1);
	}
	return(1);
}

/* me.point(): line and column (both from 1) */

static int l_point(lua_State *L)
{
	lua_pushinteger(L, getlinenum(curbp, curwp->w_dotp));
	lua_pushinteger(L, curwp->w_doto + 1);
	return(2);
}

/* me.moveto(line, [col]) */

static int l_moveto(lua_State *L)
{
	lua_Integer line, col;
	LINE *lp;
	int found = TRUE;

	line = luaL_checkinteger(L, 1);
	col = luaL_optinteger(L, 2, 1);
	lp = lforw(curbp->b_linep);
	if (line < 1)
		found = FALSE;
	while (found && --line > 0) {
		if (lp == curbp->b_linep) {
			found = FALSE;
			break;
		}
		lp = lforw(lp);
	}
	if (!found || lp == curbp->b_linep) {
		lp = curbp->b_linep;
		col = 1;
		found = FALSE;
	}
	curwp->w_dotp = lp;
	if (col < 1)
		col = 1;
	if (col > lused(lp) + 1)
		col = lused(lp) + 1;
	curwp->w_doto = (int)col - 1;
	curwp->w_flag |= WFMOVE;
	lua_pushboolean(L, found);
	return(1);
}

/* me.region(): text between point and mark, or nil */

static int l_region(lua_State *L)
{
	REGION r;
	LINE *lp;
	int off;
	long size;
	luaL_Buffer b;

	if (curwp->w_markp[0] == NULL || getregion(&r) != TRUE) {
		lua_pushnil(L);
		return(1);
	}
	luaL_buffinit(L, &b);
	lp = r.r_linep;
	off = r.r_offset;
	size = r.r_size;
	while (size-- > 0) {
		if (off == lused(lp)) {
			luaL_addchar(&b, '\n');
			lp = lforw(lp);
			off = 0;
		} else
			luaL_addchar(&b, lgetc(lp, off++));
	}
	luaL_pushresult(&b);
	return(1);
}

static BUFFER *optbuffer(lua_State *L, int idx)
{
	BUFFER *bp;
	const char *name;

	if (lua_isnoneornil(L, idx))
		return(curbp);
	name = luaL_checkstring(L, idx);
	if (strlen(name) >= NBUFN)
		return(NULL);
	bp = bfind((char *)name, FALSE, 0);
	return(bp);
}

/* me.text([buffer]): whole text of a buffer */

static int l_text(lua_State *L)
{
	BUFFER *bp;
	LINE *lp;
	luaL_Buffer b;

	if ((bp = optbuffer(L, 1)) == NULL)
		return(luaL_error(L, "no such buffer: %s", lua_tostring(L, 1)));
	luaL_buffinit(L, &b);
	for (lp = lforw(bp->b_linep); lp != bp->b_linep; lp = lforw(lp)) {
		luaL_addlstring(&b, ltext(lp), lused(lp));
		luaL_addchar(&b, '\n');
	}
	luaL_pushresult(&b);
	return(1);
}

/* ------------------------------------------------------------------ */
/*	Buffers								*/
/* ------------------------------------------------------------------ */

/* me.buffer([name]): table describing a buffer, or nil */

static int l_buffer(lua_State *L)
{
	BUFFER *bp;
	LINE *lp;
	lua_Integer lines = 0;

	if ((bp = optbuffer(L, 1)) == NULL) {
		lua_pushnil(L);
		return(1);
	}
	for (lp = lforw(bp->b_linep); lp != bp->b_linep; lp = lforw(lp))
		++lines;
	lua_createtable(L, 0, 7);
	lua_pushstring(L, bp->b_bname);
	lua_setfield(L, -2, "name");
	lua_pushstring(L, bp->b_fname);
	lua_setfield(L, -2, "file");
	lua_pushboolean(L, (bp->b_flag & BFCHG) != 0);
	lua_setfield(L, -2, "changed");
	lua_pushboolean(L, (bp->b_flag & BFINVS) != 0);
	lua_setfield(L, -2, "invisible");
	lua_pushinteger(L, bp->b_mode);
	lua_setfield(L, -2, "mode");
	lua_pushinteger(L, lines);
	lua_setfield(L, -2, "lines");
	lua_pushinteger(L, bp->b_nwnd);
	lua_setfield(L, -2, "windows");
	return(1);
}

/* me.buffers([all]): list of buffer names (invisible ones only if all) */

static int l_buffers(lua_State *L)
{
	BUFFER *bp;
	int all, count = 0;

	all = lua_toboolean(L, 1);
	lua_newtable(L);
	for (bp = bheadp; bp; bp = bp->b_bufp) {
		if ((bp->b_flag & BFINVS) && !all)
			continue;
		lua_pushstring(L, bp->b_bname);
		lua_rawseti(L, -2, ++count);
	}
	return(1);
}

/* ------------------------------------------------------------------ */
/*	Talking to the user						*/
/* ------------------------------------------------------------------ */

static int l_message(lua_State *L)
{
	const char *s;
	char buf[NSTRING];

	s = luaL_checkstring(L, 1);
	strncpy(buf, s, NSTRING - 1);
	buf[NSTRING - 1] = 0;
	mlforce(buf);
	return(0);
}

/* me.prompt(text, [default]) */

static int l_prompt(lua_State *L)
{
	const char *prompt;
	const char *def;
	char pbuf[NSTRING];
	char reply[NSTRING];
	int oldcle;
	int status;

	prompt = luaL_checkstring(L, 1);
	def = luaL_optstring(L, 2, NULL);
	strncpy(pbuf, prompt, NSTRING - 1);
	pbuf[NSTRING - 1] = 0;
	oldcle = clexec;
	clexec = FALSE;
	reply[0] = 0;
	status = mlreply(pbuf, reply, NSTRING);
	clexec = oldcle;
	if (status == ABORT)
		lua_pushnil(L);
	else if (reply[0] == 0 && def)
		lua_pushstring(L, def);
	else
		lua_pushstring(L, reply);
	return(1);
}

/* me.yesno(text): true, false, or nil for ^G */

static int l_yesno(lua_State *L)
{
	char pbuf[NSTRING];
	int status;

	strncpy(pbuf, luaL_checkstring(L, 1), NSTRING - 20);
	pbuf[NSTRING - 20] = 0;
	status = mlyesno(pbuf);
	if (status == ABORT)
		lua_pushnil(L);
	else
		lua_pushboolean(L, status == TRUE);
	return(1);
}

/* me.getkey(): wait for a key and return its name */

static int l_getkey(lua_State *L)
{
	char kname[NSTRING];

	cmdstr(getcmd(), kname);
	lua_pushstring(L, kname);
	return(1);
}

static int l_update(lua_State *L)
{
	(void)L;
	update(TRUE);
	return(0);
}

static int l_beep(lua_State *L)
{
	(void)L;
	TTbeep();
	return(0);
}

/* ------------------------------------------------------------------ */
/*	The eval-lua command						*/
/* ------------------------------------------------------------------ */

int PASCAL NEAR elua_evalcmd(int f, int n)
{
	lua_State *L = thread();
	char line[NSTRING];
	int status, base, nres, i;
	luaL_Buffer b;
	char result[NSTRING];

	(void)f;
	(void)n;
	if (L == NULL)
		return(FALSE);
	if ((status = mlreply("Lua: ", line, NSTRING)) != TRUE)
		return(status);

	base = lua_gettop(L);

	/* try it as an expression first, then as a statement */
	lua_pushliteral(L, "return ");
	lua_pushstring(L, line);
	lua_concat(L, 2);
	if (luaL_loadbuffer(L, lua_tostring(L, -1), lua_rawlen(L, -1),
			    "=eval-lua") != LUA_OK) {
		lua_pop(L, 2);
		if (luaL_loadbuffer(L, line, strlen(line), "=eval-lua")
		    != LUA_OK) {
			report(L);
			return(FALSE);
		}
	} else
		lua_remove(L, -2);

	if (docall(L, 0, LUA_MULTRET) == FALSE)
		return(FALSE);

	nres = lua_gettop(L) - base;
	if (nres > 0) {
		luaL_buffinit(L, &b);
		for (i = 1; i <= nres; i++) {
			if (i > 1)
				luaL_addstring(&b, "  ");
			luaL_tolstring(L, base + i, NULL);
			luaL_addvalue(&b);
		}
		luaL_pushresult(&b);
		strncpy(result, lua_tostring(L, -1), NSTRING - 1);
		result[NSTRING - 1] = 0;
		mlforce(result);
	}
	lua_settop(L, base);
	return(TRUE);
}

/* ------------------------------------------------------------------ */
/*	Setting up							*/
/* ------------------------------------------------------------------ */

static CONST luaL_Reg melib[] = {
	{"command",	l_command},
	{"defcommand",	l_defcommand},
	{"bind",	l_bind},
	{"unbind",	l_unbind},
	{"binding",	l_binding},
	{"runkey",	l_runkey},
	{"findfile",	l_findfile},
	{"insert",	l_insert},
	{"delete",	l_delete},
	{"line",	l_line},
	{"setline",	l_setline},
	{"char",	l_char},
	{"point",	l_point},
	{"moveto",	l_moveto},
	{"region",	l_region},
	{"text",	l_text},
	{"buffer",	l_buffer},
	{"buffers",	l_buffers},
	{"prompt",	l_prompt},
	{"yesno",	l_yesno},
	{"getkey",	l_getkey},
	{"update",	l_update},
	{"beep",	l_beep},
	{"eval",	l_eval},
	{"_message",	l_message},
	{"_sethook",	l_sethook},
	{"_cancelexit",	l_cancelexit},
	{"_builtins",	l_builtins},
	{"_luacommands", l_luacommands},
	{NULL,		NULL}
};

/* The parts of the "me" library that are easier to write in Lua */

static CONST char prelude[] =
"local me = ...\n"
"local format, concat, select, tostring = string.format, table.concat, select, tostring\n"
"local _message, _bind, _unbind, _binding = me._message, me.bind, me.unbind, me.binding\n"
"local _runkey = me.runkey\n"
"local _sethook, _cancelexit = me._sethook, me._cancelexit\n"
"local _builtins, _luacommands = me._builtins, me._luacommands\n"
"me._message, me._sethook, me._cancelexit, me._builtins, me._luacommands = nil\n"
"\n"
"function me.message(fmt, ...)\n"
"  if select('#', ...) > 0 then _message(format(fmt, ...)) else _message(tostring(fmt)) end\n"
"end\n"
"\n"
"function print(...)\n"
"  local t = {}\n"
"  for i = 1, select('#', ...) do t[i] = tostring((select(i, ...))) end\n"
"  _message(concat(t, '  '))\n"
"end\n"
"\n"
"-- Key names: MicroEMACS form ('^XF', 'M-A', 'FN1') or Emacs form\n"
"-- ('C-x C-f', 'M-a', 'C-x 2', 'M-C-h', 'C-x TAB').\n"
"local special = { SPC = ' ', TAB = '^I', RET = '^M', LFD = '^J', ESC = '^[', DEL = '\\127' }\n"
"function me.keyname(k)\n"
"  if type(k) ~= 'string' or k == '' then error('key name expected', 2) end\n"
"  local emacs = k:find('C%-') or k:find(' ')\n"
"  for name in pairs(special) do if k:find(name, 1, true) then emacs = true end end\n"
"  if not emacs then return k end\n"
"  local toks = {}\n"
"  for t in k:gmatch('%S+') do toks[#toks + 1] = t end\n"
"  local prefix = ''\n"
"  if #toks == 2 and toks[1] == 'C-x' then prefix = '^X'; table.remove(toks, 1) end\n"
"  if #toks ~= 1 then error('bad key name: ' .. k, 2) end\n"
"  local t, meta, ctrl = toks[1], false, false\n"
"  while #t > 2 do\n"
"    local p = t:sub(1, 2)\n"
"    if p == 'M-' then meta = true elseif p == 'C-' then ctrl = true else break end\n"
"    t = t:sub(3)\n"
"  end\n"
"  local base = special[t]\n"
"  if base then\n"
"    if ctrl then error('bad key name: ' .. k, 2) end\n"
"  elseif #t == 1 then\n"
"    base = ctrl and ('^' .. t:upper()) or t\n"
"  else\n"
"    error('bad key name: ' .. k, 2)\n"
"  end\n"
"  return prefix .. (meta and 'M-' or '') .. base\n"
"end\n"
"local keyname = me.keyname\n"
"function me.bind(k, target) return _bind(keyname(k), target) end\n"
"function me.unbind(k) return _unbind(keyname(k)) end\n"
"function me.binding(k) return _binding(keyname(k)) end\n"
"function me.runkey(k, n) return _runkey(keyname(k), n) end\n"
"\n"
"function me.commands()\n"
"  local list, seen = {}, {}\n"
"  for _, n in ipairs(_builtins()) do list[#list + 1] = n; seen[n] = true end\n"
"  for _, n in ipairs(_luacommands()) do if not seen[n] then list[#list + 1] = n end end\n"
"  table.sort(list)\n"
"  return list\n"
"end\n"
"\n"
"me.cmd = setmetatable({}, { __index = function(t, name)\n"
"  local cname = name:gsub('_', '-')\n"
"  local f = function(...) return me.command(cname, ...) end\n"
"  return f\n"
"end })\n"
"\n"
"-- Hooks\n"
"local hookvar = { read = 'readhook', write = 'writehook', ['buffer-enter'] = 'bufhook',\n"
"  ['buffer-exit'] = 'exbhook', command = 'cmdhook', wrap = 'wraphook', exit = 'exithook' }\n"
"local hooks = {}\n"
"function me.hook(event, fn)\n"
"  local var = hookvar[event]\n"
"  if not var then error('unknown hook event: ' .. tostring(event), 2) end\n"
"  if type(fn) ~= 'function' then error('function expected', 2) end\n"
"  local list = hooks[event]\n"
"  if not list then\n"
"    list = {}\n"
"    hooks[event] = list\n"
"    local running = false\n"
"    _sethook(var, function()\n"
"      if running then return true end\n"
"      running = true\n"
"      local arg\n"
"      if event == 'read' or event == 'write' then arg = me.var.cfname\n"
"      elseif event == 'buffer-enter' or event == 'buffer-exit' then arg = me.var.cbufname end\n"
"      local stay, failed = false, nil\n"
"      for _, h in ipairs({ table.unpack(list) }) do\n"
"        local ok, r = pcall(h, arg)\n"
"        if not ok then failed = failed or r\n"
"        elseif event == 'exit' and r == false then stay = true end\n"
"      end\n"
"      running = false\n"
"      if stay then _cancelexit() end\n"
"      if failed then me.message('%%%s hook: %s', event, tostring(failed)) end\n"
"      return true\n"
"    end)\n"
"  end\n"
"  list[#list + 1] = fn\n"
"end\n"
"function me.unhook(event, fn)\n"
"  local var = hookvar[event]\n"
"  if not var then error('unknown hook event: ' .. tostring(event), 2) end\n"
"  local list = hooks[event]\n"
"  if not list then return end\n"
"  if fn == nil then\n"
"    for i = #list, 1, -1 do list[i] = nil end\n"
"  else\n"
"    for i = #list, 1, -1 do if list[i] == fn then table.remove(list, i) end end\n"
"  end\n"
"  if #list == 0 then hooks[event] = nil; _sethook(var, nil) end\n"
"end\n";

static int setup(lua_State *L)
{
	luaL_openlibs(L);

	/* the command table */
	lua_newtable(L);
	lua_setfield(L, LUA_REGISTRYINDEX, CMDTABLE);

	/* the me library */
	luaL_newlib(L, melib);

	/* me.var: the $environment variables */
	lua_newtable(L);
	lua_newtable(L);
	lua_pushcfunction(L, l_getvar);
	lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, l_setvar);
	lua_setfield(L, -2, "__newindex");
	lua_setmetatable(L, -2);
	lua_setfield(L, -2, "var");

	/* me.uvar: the %user variables of the old language */
	lua_newtable(L);
	lua_newtable(L);
	lua_pushcfunction(L, l_getuvar);
	lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, l_setuvar);
	lua_setfield(L, -2, "__newindex");
	lua_setmetatable(L, -2);
	lua_setfield(L, -2, "uvar");

	lua_pushstring(L, VERSION);
	lua_setfield(L, -2, "version");

	lua_pushvalue(L, -1);
	lua_setglobal(L, "me");

	/* run the prelude with the me table as its argument */
	if (luaL_loadbuffer(L, prelude, sizeof(prelude) - 1, "=prelude")
	    != LUA_OK)
		return(lua_error(L));
	lua_insert(L, -2);
	lua_call(L, 1, 0);
	return(0);
}

int PASCAL NEAR elua_init()
{
	lua_State *L;

	if (Lmain != NULL)
		return(TRUE);
	if ((L = luaL_newstate()) == NULL)
		return(FALSE);
	lua_pushcfunction(L, setup);
	if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
		fprintf(stderr, "Lua: %s\n", lua_tostring(L, -1));
		lua_close(L);
		return(FALSE);
	}
	lua_sethook(L, interrupt_hook, LUA_MASKCOUNT, 10000);
	Lmain = L;
	return(TRUE);
}

VOID PASCAL NEAR elua_close()
{
	if (Lmain) {
		lua_close(Lmain);
		Lmain = NULL;
		curL = NULL;
	}
}

#endif	/* LUASCRIPT */
