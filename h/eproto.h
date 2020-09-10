/*	EPROTO:		Global function prototypes and declarations
			MicroEMACS 4.00

                        written by Daniel Lawrence
                        based on code by Dave G. Conroy,
                        	Steve Wilhite and George Jones
*/

#if LINUX
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <wctype.h>
#endif
#if	UTF8
#include <locale.h>
#endif
#if	CURSES
#undef CTRL
#if	UTF8
#include <ncursesw/ncurses.h>		/* Curses screen output		*/
#include <ncursesw/panel.h>		/* Curses screen output		*/
#define CTRL 0x01000000			/* Restore definition		*/
#else
#include <ncurses/ncurses.h>		/* Curses screen output		*/
#include <ncurses/panel.h>		/* Curses screen output		*/
#define CTRL 0x0100			/* Restore definition		*/
#endif
#endif

#if	PROTO

/***	global function prototypes	***/

#if	CURSES
extern int handlemenu(WINDOW* wmouse, int ypos, int xpos);
#endif

#if	CURSES
extern int bindtomenu (int f, int n);
extern int macrotomenu (int f, int n);
extern int unbindmenu (int f, int n);
extern void closedrop(void);
extern void menukey(int c);
extern int menudrop(int f, int n);
#endif

#if	WINDOW_MSWIN
extern char * PASCAL fullpathname (char *PathName, int Nbuf);
extern int PASCAL NEAR vtinitscr (ESCREEN *sp, int nrow, int ncol);
extern int PASCAL NEAR vtsizescr (ESCREEN *sp, int nrow, int ncol);
extern int PASCAL cutregion (int f, int n);
extern int PASCAL clipregion (int f, int n);
extern int PASCAL insertclip (int f, int n);
extern int PASCAL helpengine (int f, int n);
extern int PASCAL minimizescreen (int f, int n);
extern int PASCAL maximizescreen (int f, int n);
extern int PASCAL restorescreen (int f, int n);
extern int PASCAL tilescreens (int f, int n);
extern int PASCAL cascadescreens (int f, int n);
extern int PASCAL bindtomenu (int f, int n);
extern int PASCAL macrotomenu (int f, int n);
extern int PASCAL unbindmenu (int f, int n);
extern int PASCAL execmenu (int f, int n);
extern int PASCAL longop (int f);
extern int PASCAL filenamedlg (char *prompt, char *buf, int nbuf, int fullpath);
extern int PASCAL NEAR vtfreescr (ESCREEN *sp);
extern int PASCAL NEAR unlist_screen(ESCREEN *sp);
extern int PASCAL mlhistory(VOID);
extern int PASCAL updscrollbars (ESCREEN *sp, char w_flag);
extern VOID PASCAL NEAR vtscreen (ESCREEN *sp);
#endif

#if WINXP
#define ToWUpper(a) CharUpperW(a)
#define ToWLower(a) CharLowerW(a)
#else
#define ToWUpper(a) towupper(a)
#define ToWLower(a) towlower(a)
#endif

#if CALLED
extern int emacs(int argc, char *argv[]);
#endif

#if HANDLE_WINCH
#if LINUX
extern VOID winch_changed(int status);
#else
extern VOID winch_changed(VOID);
#endif
extern VOID winch_new_size(VOID);
VOID winch_vtresize(int rows, int cols);
#endif

#if DEBUG_SEARCH
int PASCAL NEAR mc_list(int f, int n);
int PASCAL NEAR rmc_list(int f, int n);
VOID PASCAL NEAR mctype_cat(char pline[], int mc_type);
#endif

extern VOID undo_insert(OPTYPE op_type, long count, OBJECT op_erand);
extern int undo_op(VOID);
extern VOID undo_dump(VOID);
extern VOID undo_zot(BUFFER *bp);
extern int PASCAL NEAR undo(int f, int n);
extern int PASCAL NEAR undo_delete(int f, int n);
extern int PASCAL NEAR undo_list(int f, int n);
extern VOID *room(int);
extern VOID *reroom(VOID *, int);

extern int PASCAL NEAR ab_insert(char *sym, char *expansion);
extern char *PASCAL NEAR ab_lookup(char *sym);
extern int PASCAL NEAR ab_delete(char *sym);
extern int PASCAL NEAR ab_clean(VOID);
extern BUFFER *PASCAL NEAR bfind(CONST char *bname, int cflag, int bflag);
extern BUFFER *PASCAL NEAR getcbuf(CONST char *prompt, char *defval, int createflag);
extern BUFFER *PASCAL NEAR getdefb(VOID);
extern BUFFER *PASCAL NEAR getoldb(VOID);
extern ESCREEN *PASCAL NEAR init_screen(char *, BUFFER *);
extern ESCREEN *PASCAL NEAR lookup_screen(char *scr_name);
extern ESCREEN *PASCAL NEAR index_screen(int scr_num);
extern int PASCAL NEAR screen_index(ESCREEN *sp);
extern int PASCAL NEAR insert_screen(ESCREEN *sp);
extern int PASCAL NEAR select_screen(ESCREEN *sp, int announce);
extern VOID PASCAL NEAR free_screen(ESCREEN *sp);
extern char *Eallocate(unsigned nbytes);
extern char *dolock(CONST char *fname);
extern char *getpath(char *filespec);
extern char *gtname(char *filespec);
extern char *PASCAL NEAR bytecopy(char *dst, CONST char *src, int maxlen);
extern char *PASCAL NEAR cmdstr(int c, char *seq);
extern char *PASCAL NEAR copystr(char *);
extern char *PASCAL NEAR complete(CONST char *prompt, char *defval, int type, int maxlen);
extern char *PASCAL NEAR envval(int i);
extern CONST char *PASCAL NEAR fixnull(CONST char *s);
extern CONST char *PASCAL NEAR flook(CONST char *fname, int hflag);
extern char *PASCAL NEAR funval(int i);
extern char *PASCAL NEAR getctext(char *rline);
extern char *PASCAL NEAR getffile(char *fspec);
extern CONST char *PASCAL NEAR getfname(KEYTAB *key);
extern char *PASCAL NEAR getkill(VOID);
extern char *PASCAL NEAR getnfile(VOID);
extern char *PASCAL NEAR getreg(char *value);
extern CONST char *PASCAL NEAR getval(char *token);
extern char *PASCAL NEAR getwlist(char *buf);
extern CONST char *PASCAL NEAR gtenv(CONST char *vname);
extern char *PASCAL NEAR gtfilename(CONST char *prompt);
extern CONST char *PASCAL NEAR gtfun(char *fname);
extern char *PASCAL NEAR gtusr(char *vname);
extern char *PASCAL NEAR int_asc(int i);
extern char *PASCAL NEAR long_asc(long num);
extern CONST char *PASCAL NEAR ltos(int val);
extern CONST char *PASCAL NEAR makename(char *bname, CONST char *fname);
extern char *PASCAL NEAR mklower(char *str);
extern char *PASCAL NEAR mkupper(char *str);
extern char *PASCAL NEAR namval(int index);
extern char *PASCAL NEAR timeset(VOID);
extern char *PASCAL NEAR token(char *src, char *tok, int size);
extern CONST char *PASCAL NEAR transbind(CONST char *skey);
extern char *PASCAL NEAR trimstr(char *s);
extern char *PASCAL NEAR xlat(char *source, char *lookup, char *trans);
extern char *undolock(CONST char *fname);
extern char *PASCAL NEAR regtostr(char *buf, REGION *region);
#if	UTF8
extern unsigned int PASCAL NEAR chcase(unsigned int ch);
extern unsigned int PASCAL NEAR lowerc(unsigned int ch);
extern unsigned int PASCAL NEAR upperc(unsigned int ch);
#else
extern unsigned int PASCAL NEAR chcase(register unsigned int ch);
extern int PASCAL NEAR lowerc(char ch);
extern int PASCAL NEAR upperc(char ch);
#endif
extern int PASCAL NEAR cycle_ring(int f, int n);
extern int PASCAL NEAR setlower(char *ch, char *val);
extern int PASCAL NEAR setupper(char *ch, char *val);
#if	ZTC || TURBO || IC
extern int (PASCAL NEAR *PASCAL NEAR fncmatch(char *fname))(int, int);
extern int (PASCAL NEAR *PASCAL NEAR getname(char *prompt))(int, int);
#else	/* Sun (and others?) screwed up the prototyping.*/
#if	THEOSC
extern int *fncmatch(char* fname);
extern int *getname(CONST char* fname);
#else
extern int (PASCAL NEAR *PASCAL NEAR fncmatch(char *fname))(VOID);
extern int (PASCAL NEAR *PASCAL NEAR getname(CONST char *prompt))(VOID);
#endif
#endif
extern int PASCAL NEAR asc_int(char *st);
extern VOID dolhello(VOID);
extern int dspram(VOID);
extern VOID lckerror(char *errstr);
extern VOID lckhello(VOID);
extern int xlock(CONST char *fname);
extern int lockchk(CONST char *fname);
extern int lockrel(VOID);
extern VOID mousehello(VOID);
extern int nocrypt(VOID);
extern int PASCAL NEAR absv(int x);
extern int PASCAL NEAR add_abbrev(int f, int n);
extern int PASCAL NEAR del_abbrev(int f, int n);
extern int PASCAL NEAR kill_abbrevs(int f, int n);
extern int PASCAL NEAR ins_abbrevs(int f, int n);
extern int PASCAL NEAR def_abbrevs(int f, int n);
extern int PASCAL NEAR addline(BUFFER *bp, CONST char *text);
extern int PASCAL NEAR backhunt(int f, int n);
extern int PASCAL NEAR backsearch(int f, int n);
extern int PASCAL NEAR bktoshell(int f, int n);
extern int PASCAL NEAR boundry(LINE *curline, int curoff, int dir);
extern int PASCAL NEAR checknext(int chr, int dir);
extern int PASCAL NEAR clear_ring(int f, int n);
extern int PASCAL NEAR delins(int dlength, char *instr, int use_rmc);
extern int PASCAL NEAR desfunc(int f, int n);
extern int PASCAL NEAR dispvar(int f, int n);
extern int PASCAL NEAR ueechochar(unsigned char c);
extern int PASCAL NEAR echostring(char *, int, int);
extern int PASCAL NEAR eq(register unsigned char bc, register unsigned char pc);
extern long PASCAL NEAR ernd(VOID);
extern int PASCAL NEAR execkey(KEYTAB *key, int f, int n);
extern int PASCAL NEAR fbound(DELTA *tbl, int jump, LINE **pcurline, int *pcuroff, int dir);
extern int PASCAL NEAR fexist(char *fname);
extern int PASCAL NEAR findcol(LINE *lp, int pos);
extern int PASCAL NEAR fisearch(int f, int n);
#if	FLABEL
extern int PASCAL NEAR fnclabel(int f, int n);
#endif
extern int PASCAL NEAR forwhunt(int f, int n);
extern int PASCAL NEAR forwsearch(int f, int n);
extern int PASCAL NEAR getcwnum(VOID);
extern int PASCAL NEAR getgoal(LINE *dlp);
extern int PASCAL NEAR getstring(unsigned char *buf, int nbuf, int eolchar);
extern int PASCAL NEAR gettwnum(VOID);
extern int PASCAL NEAR gettyp(CONST char *token);
extern int PASCAL NEAR getkey(VOID);
extern int PASCAL NEAR getwpos(VOID);
extern int PASCAL NEAR get_char(VOID);
extern int PASCAL NEAR global_var(int f, int n);
#if	DBCS
extern int PASCAL NEAR is2byte(char *sp, char *cp);
#endif
#if	UTF8
extern int PASCAL NEAR weq(unsigned int bc, unsigned int pc);
extern int PASCAL NEAR is_letter(unsigned int ch);
extern int PASCAL NEAR is_lower(unsigned int ch);
extern int PASCAL NEAR is_upper(unsigned int ch);
#else
extern int PASCAL NEAR is_letter(char ch);
extern int PASCAL NEAR is_lower(char ch);
extern int PASCAL NEAR is_upper(char ch);
#endif
extern int PASCAL NEAR is_num(char *st);
extern int PASCAL NEAR isearch(int dir);
extern int PASCAL NEAR kinsert(int back, char c);
extern int PASCAL NEAR ldelnewline(VOID);
extern int PASCAL NEAR linstr(CONST char *instr);
extern int PASCAL NEAR liteq(LINE **curline, int *curpos, int direct, char *lstring);
extern int PASCAL NEAR lnewline(VOID);
extern int PASCAL NEAR local_var(int f, int n);
extern int PASCAL NEAR lookup_color(char *sp);
extern int PASCAL NEAR lover(CONST char *ostr);
extern int PASCAL NEAR mlprompt(CONST char *, char *, int);
extern int PASCAL NEAR movelocalpoint(int n, int *pcuroff, LINE **pcurline);
extern int PASCAL NEAR nextch(LINE **pcurline, int *pcuroff, int dir);
extern int PASCAL NEAR pop(BUFFER *popbuffer);
extern int PASCAL NEAR qreplace(int f, int n);
extern int PASCAL NEAR readpattern(CONST char *prompt, char apat[], int srch);
#if	WINDOW_TEXT
extern VOID PASCAL NEAR refresh_screen(ESCREEN *sp);
#endif
extern int PASCAL NEAR reglines(VOID);
extern int PASCAL NEAR rename_screen(int f, int n);
extern int PASCAL NEAR replaces(int kind, int f, int n);
extern int PASCAL NEAR risearch(int f, int n);
extern int PASCAL NEAR rmcstr(VOID);
extern int PASCAL NEAR savematch(VOID);
extern int PASCAL NEAR scanmore(int dir);
#if MAGIC == 0
extern int PASCAL NEAR scanner(int direct, int beg_or_end, int repeats);
#else
extern int PASCAL NEAR amatch(MC *mcptr, int direct, LINE **pcwline, int *pcwoff);
extern int PASCAL NEAR biteq(int bc, EBITMAP cclmap);
extern int PASCAL NEAR cclmake(char **ppatptr, MC *mcptr);
extern int PASCAL NEAR litmake(char **ppatptr, MC *mcptr);
extern VOID PASCAL NEAR mcclear(VOID);
extern int PASCAL NEAR mceq(unsigned char bc, MC *mt);
extern int PASCAL NEAR mcscanner(MC *mcpatrn, int direct, int beg_or_end, int repeats);
extern int PASCAL NEAR mcstr(VOID);
extern VOID PASCAL NEAR setbit(int bc, EBITMAP cclmap);
#endif
extern int PASCAL NEAR setvar(int f, int n);
extern int PASCAL NEAR sindex(CONST char *source, CONST char *pattern);
extern int PASCAL NEAR srindex(CONST char* source, CONST char* pattern);
extern int PASCAL NEAR sreplace(int f, int n);
extern int PASCAL NEAR stol(char *val);
#if	DBCS
extern int PASCAL NEAR stopback(VOID);
extern int PASCAL NEAR stopforw(VOID);
#endif
extern int PASCAL NEAR svar(VDESC *var, char *value);
extern int PASCAL NEAR tgetc(VOID);
extern int PASCAL NEAR uneat(VOID);
extern VOID PASCAL NEAR unlist_screen(ESCREEN *sp);
extern int PASCAL NEAR upscreen(int f, int n);
extern int PASCAL NEAR vtinit(VOID);
extern int PASCAL NEAR yank(int f, int n);
extern int PASCAL NEAR yank_pop(int f, int n);
extern int Erelease(char *mp);
extern int set_key(KEYTAB *key, char *name);
extern int xunlock(char *fname);
extern KEYTAB *getbind(int c);
extern LINE *PASCAL NEAR lalloc(int used);
extern LINE *PASCAL NEAR mouseline(EWINDOW *wp, int row);
extern long PASCAL NEAR getlinenum(BUFFER *bp, LINE *sline);
extern int PASCAL NEAR addkey(unsigned char * seq, int fn);
extern int PASCAL NEAR addkeymap(int f, int n);
extern int PASCAL NEAR adjustmode(int kind, int global);
extern int PASCAL NEAR anycb(VOID);
extern int PASCAL NEAR apro(int f, int n);
extern int PASCAL NEAR backchar(int f, int n);
extern int PASCAL NEAR backdel(int f, int n);
extern int PASCAL NEAR backline(int f, int n);
extern int PASCAL NEAR backpage(register int f, register int n);
extern int PASCAL NEAR backword(int f, int n);
extern int PASCAL NEAR bclear(BUFFER *bp);
#if	THEOSC
extern int binary();
#else
extern int PASCAL NEAR binary(CONST char *key, char *(PASCAL NEAR *tval)(), int tlength, int klength);
#endif
extern int PASCAL NEAR bindtokey(int f, int n);
extern int PASCAL NEAR buildlist(int type, char *mstring);
extern int PASCAL NEAR capword(int f, int n);
extern int PASCAL NEAR cex(int f, int n);
extern int PASCAL NEAR cinsert(VOID);
extern int PASCAL NEAR clean(VOID);
extern int PASCAL NEAR clrmes(int f, int n);
extern int PASCAL NEAR copyregion(int f, int n);
extern int PASCAL NEAR ctlxe(int f, int n);
extern int PASCAL NEAR ctlxlp(int f, int n);
extern int PASCAL NEAR ctlxrp(int f, int n);
extern int PASCAL NEAR ctoec(int c);
extern int PASCAL NEAR ctrlg(int f, int n);
extern int PASCAL NEAR cycle_screens(int f, int n);
extern VOID PASCAL NEAR dcline(int argc, char *argv[], int firstflag);
extern int PASCAL NEAR deblank(int f, int n);
extern int PASCAL NEAR debug(BUFFER *bp, char *eline, int *skipflag);
extern int PASCAL NEAR delbword(int f, int n);
extern int PASCAL NEAR delete_screen(int f, int n);
extern int PASCAL NEAR delfword(int f, int n);
extern int PASCAL NEAR delgmode(int f, int n);
extern int PASCAL NEAR delmode(int f, int n);
extern int PASCAL NEAR delwind(int f, int n);
extern int PASCAL NEAR desc_abbrevs(int f, int n);
extern int PASCAL NEAR desbind(int f, int n);
extern int PASCAL NEAR deskey(int f, int n);
extern int PASCAL NEAR desvars(int f, int n);
extern int PASCAL NEAR detab(int f, int n);
extern int PASCAL NEAR dobuf(BUFFER *bp);
extern int PASCAL NEAR docmd(char *cline);
extern int PASCAL NEAR dofile(CONST char *fname);
extern int PASCAL NEAR ectoc(int c);
extern int PASCAL NEAR edinit(char bname[]);
extern int PASCAL NEAR editloop(VOID);
extern int PASCAL NEAR endword(int f, int n);
extern int PASCAL NEAR enlargewind(int f, int n);
extern int PASCAL NEAR entab(int f, int n);
extern int PASCAL NEAR execbuf(int f, int n);
extern int PASCAL NEAR execcmd(int f, int n);
extern int PASCAL NEAR execfile(int f, int n);
extern int PASCAL NEAR execprg(int f, int n);
extern int PASCAL NEAR execproc(int f, int n);
extern int PASCAL NEAR execute(int c, int f, int n);
extern int PASCAL NEAR ffclose(VOID);
extern int PASCAL NEAR ffgetline(int *nbytes);
extern int PASCAL NEAR ffputline(char buf[], int nbuf);
extern int PASCAL NEAR ffropen(CONST char *fn);
extern int PASCAL NEAR ffwopen(CONST char *fn, CONST char *mode);
extern int PASCAL NEAR fileapp(int f, int n);
extern int PASCAL NEAR find_screen(int f, int n);
extern int PASCAL NEAR filefind(int f, int n);
extern int PASCAL NEAR filename(int f, int n);
extern int PASCAL NEAR fileread(int f, int n);
extern int PASCAL NEAR filesave(int f, int n);
extern int PASCAL NEAR filewrite(int f, int n);
extern int PASCAL NEAR fillpara(int f, int n);
extern int PASCAL NEAR uefilter(int f, int n);
extern VOID PASCAL NEAR findvar(char *var, VDESC *vd, int size, int scope);
extern int PASCAL NEAR fmatch(char ch);
extern int PASCAL NEAR forwchar(int f, int n);
extern int PASCAL NEAR forwdel(int f, int n);
extern int PASCAL NEAR forwline(int f, int n);
extern int PASCAL NEAR forwpage(int f, int n);
extern int PASCAL NEAR forwword(int f, int n);
extern int PASCAL NEAR getccol(int bflg);
extern int PASCAL NEAR getcmd(VOID);
extern int PASCAL NEAR getfence(int f, int n);
extern int PASCAL NEAR getfile(char fname[], int lockfl);
extern int PASCAL NEAR get_key(VOID);
extern int PASCAL NEAR getregion(REGION *rp);
extern int PASCAL NEAR gotobob(int f, int n);
extern int PASCAL NEAR gotobol(int f, int n);
extern int PASCAL NEAR gotobop(int f, int n);
extern int PASCAL NEAR gotoeob(int f, int n);
extern int PASCAL NEAR gotoeol(int f, int n);
extern int PASCAL NEAR gotoeop(int f, int n);
extern int PASCAL NEAR gotoline(int f, int n);
extern int PASCAL NEAR gotomark(int f, int n);
extern int PASCAL NEAR help(int f, int n);
extern int PASCAL NEAR ifile(char fname[]);
extern int PASCAL NEAR indent(int f, int n);
extern int PASCAL NEAR indent_region(int f, int n);
extern int PASCAL NEAR insbrace(int n, int c);
extern int PASCAL NEAR insfile(int f, int n);
extern int PASCAL NEAR inspound(VOID);
extern int PASCAL NEAR insspace(int f, int n);
extern int PASCAL NEAR inword(VOID);
#if	UTF8
extern int PASCAL NEAR isinword(unsigned int c);
#else
extern int PASCAL NEAR isinword(char c);
#endif
extern int PASCAL NEAR ismodeline(EWINDOW *wp, int row);
extern int PASCAL NEAR istring(int f, int n);
extern int PASCAL NEAR killbuffer(int f, int n);
extern int PASCAL NEAR killpara(int f, int n);
extern int PASCAL NEAR killregion(int f, int n);
extern int PASCAL NEAR killtext(int f, int n);
extern VOID PASCAL NEAR lchange(register int flag);
extern int PASCAL NEAR ldelete(long n, int kflag);
extern VOID PASCAL NEAR lfree(LINE *lp);
#if	UTF8
extern int PASCAL NEAR ldelchar(long n, int kflag);
extern int PASCAL NEAR linsert(int n, unsigned int c);
#else
extern int PASCAL NEAR linsert(int n, char c);
#endif
extern int PASCAL NEAR listbuffers(int f, int n);
extern int PASCAL NEAR list_screens(int f, int n);
extern int PASCAL NEAR lowerregion(int f, int n);
extern int PASCAL NEAR lowerword(int f, int n);
#if	UTF8
extern int PASCAL NEAR lowrite(unsigned int c);
#else
extern int PASCAL NEAR lowrite(int c);
#endif
extern int PASCAL NEAR macarg(char *tok);
extern int PASCAL NEAR macrotokey(int f, int n);
extern int PASCAL NEAR makelist(int iflag);
extern int PASCAL NEAR movelocalpoint();
extern int PASCAL NEAR undolist();
extern VOID PASCAL NEAR mouse_screen(VOID);
extern int PASCAL NEAR screenlist(int iflag);
extern int PASCAL NEAR meexit(int status);
extern int PASCAL NEAR uemeta(int f, int n);
extern int PASCAL NEAR mlreply(CONST char *prompt, char *buf, int nbuf);
extern int PASCAL NEAR mlyesno(CONST char *prompt);
extern int PASCAL NEAR mouseoffset(EWINDOW *wp, LINE *lp, int col);
extern int PASCAL NEAR movemd(int f, int n);
extern int PASCAL NEAR movemu(int f, int n);
extern int PASCAL NEAR mregdown(int f, int n);
extern int PASCAL NEAR mmove(int f, int n);
extern int PASCAL NEAR mregup(int f, int n);
extern int PASCAL NEAR mvdnwind(int f, int n);
extern int PASCAL NEAR mvupwind(int f, int n);
extern int PASCAL NEAR namebuffer(int f, int n);
extern int PASCAL NEAR namedcmd(int f, int n);
extern int PASCAL NEAR narrow(int f, int n);
extern int PASCAL NEAR uenewline(int f, int n);
extern int PASCAL NEAR new_col_org(int f, int n);
extern int PASCAL NEAR new_row_org(int f, int n);
extern int PASCAL NEAR newsize(int f, int n);
extern int PASCAL NEAR newwidth(int f, int n);
extern int PASCAL NEAR nextarg(CONST char *prompt, char *buffer, int size, int terminator);
extern int PASCAL NEAR nextbuffer(int f, int n);
extern int PASCAL NEAR nextdown(int f, int n);
extern int PASCAL NEAR nextup(int f, int n);
extern int PASCAL NEAR nextwind(int f, int n);
extern int PASCAL NEAR nullproc(int f, int n);
extern int PASCAL NEAR onlywind(int f, int n);
extern int PASCAL NEAR openline(int f, int n);
extern VOID PASCAL NEAR ostring(CONST char *s);
extern VOID PASCAL NEAR outstring(CONST char *s);
extern int PASCAL NEAR ovstring(int f, int n);
extern int PASCAL NEAR pipecmd(int f, int n);
extern int PASCAL NEAR popbuffer(int f, int n);
extern int PASCAL NEAR prevwind(int f, int n);
extern int PASCAL NEAR putctext(char *iline);
extern int PASCAL NEAR putline(int row, int col, char buf[]);
extern int PASCAL NEAR quickexit(int f, int n);
extern int PASCAL NEAR quit(int f, int n);
extern int PASCAL NEAR quote(int f, int n);
extern int PASCAL NEAR rdonly(VOID);
extern int PASCAL NEAR readin(CONST char *fname, int lockfl);
extern int PASCAL NEAR uerefresh(int f, int n);
extern int PASCAL NEAR remmark(int f, int n);
extern int PASCAL NEAR reposition(int f, int n);
extern int PASCAL NEAR resetkey(VOID);
extern int PASCAL NEAR resize(int f, int n);
extern int PASCAL NEAR resizm(int f, int n);
extern int PASCAL NEAR resizm2(int f, int n);
extern int PASCAL NEAR resterr(VOID);
extern int PASCAL NEAR restwnd(int f, int n);
extern int PASCAL NEAR savewnd(int f, int n);
extern int PASCAL NEAR scwrite(int row, char *outstr, int forg, int bacg,
				int revleft, int revright);
extern int PASCAL NEAR setccol(int pos);
extern int PASCAL NEAR setekey(int f, int n);
extern int PASCAL NEAR setfillcol(int f, int n);
extern int PASCAL NEAR setgmode(int f, int n);
extern int PASCAL NEAR setmark(int f, int n);
extern int PASCAL NEAR setmod(int f, int n);
extern int PASCAL NEAR setwlist(char *wclist);
extern int PASCAL NEAR shellprog(char *cmd);
extern int PASCAL NEAR showcpos(int f, int n);
extern int PASCAL NEAR showfiles(int f, int n);
extern int PASCAL NEAR listkeymaps(int f, int n);
extern int PASCAL NEAR shrinkwind(int f, int n);
extern int PASCAL NEAR spal(char *pstr);
extern int PASCAL NEAR spawn(int f, int n);
extern int PASCAL NEAR spawncli(int f, int n);
extern int PASCAL NEAR splitwind(int f, int n);
extern int PASCAL NEAR startup(char *sfname);
extern int PASCAL NEAR storeproc(int f, int n);
extern int PASCAL NEAR strinc(char *source, char *sub);
extern int PASCAL NEAR swapmark(int f, int n);
extern int PASCAL NEAR swbuffer(BUFFER *bp);
extern int PASCAL NEAR uetab(int f, int n);
extern int PASCAL NEAR trim(int f, int n);
extern int PASCAL NEAR ttclose(VOID);
extern int PASCAL NEAR ttflush(VOID);
extern int PASCAL NEAR ttgetc(VOID);
extern int PASCAL NEAR ttopen(VOID);
extern int PASCAL NEAR ttputc(int c);
extern int PASCAL NEAR twiddle(int f, int n);
extern int PASCAL NEAR typahead(VOID);
extern int PASCAL NEAR unarg(VOID);
extern int PASCAL NEAR unbindchar(int c);
extern int PASCAL NEAR unbindkey(int f, int n);
extern int PASCAL NEAR undent_region(int f, int n);
extern int PASCAL NEAR unmark(int f, int n);
extern int PASCAL NEAR upperregion(int f, int n);
extern int PASCAL NEAR upperword(int f, int n);
extern int PASCAL NEAR usebuffer(int f, int n);
extern int PASCAL NEAR viewfile(int f, int n);
extern VOID PASCAL NEAR vteeol(VOID);
extern VOID PASCAL NEAR vtmove(int row, int col);
extern VOID PASCAL NEAR vtputc(int c);
extern VOID PASCAL NEAR vttidy(VOID);
extern int PASCAL NEAR widen(int f, int n);
extern int PASCAL NEAR wordcount(int f, int n);
extern int PASCAL NEAR wrapword(int f, int n);
extern int PASCAL NEAR writemsg(int f, int n);
extern int PASCAL NEAR writeout(char *fn, char *mode);
extern int PASCAL NEAR zotbuf(BUFFER *bp);
extern unsigned int PASCAL NEAR getckey(int mflag);
extern unsigned int PASCAL NEAR stock(unsigned char *keyname);
#if	VARARG && VARG
#if	GCC
extern VOID CDECL NEAR mlwrite(va_alist);
#else
extern VOID CDECL NEAR mlwrite(va_dcl);
#endif /* GCC */
#else
extern VOID CDECL NEAR mlwrite(CONST char *fmt, ...);
#endif
extern VOID PASCAL NEAR ab_init(VOID);
extern VOID PASCAL NEAR ab_save(char c);
extern VOID PASCAL NEAR ab_expand(VOID);
extern VOID PASCAL NEAR clist_buffer(char *name, int *cpos);
extern VOID PASCAL NEAR clist_command(char *name, int *cpos);
extern VOID PASCAL NEAR clist_file(char *name, int *cpos);
extern VOID PASCAL NEAR comp_buffer(char *name, int *cpos);
extern VOID PASCAL NEAR comp_command(char *name, int *cpos);
#if	!WINDOW_MSWIN
extern VOID PASCAL NEAR comp_file(char *name, int *cpos);
#endif
extern VOID PASCAL NEAR ecrypt(char *bptr, unsigned len);
extern VOID PASCAL NEAR errormesg(CONST char *mesg, BUFFER *bp, LINE *lp);
extern VOID PASCAL NEAR flong_asc(char buf[], int width, long num);
extern VOID PASCAL NEAR freewhile(WHBLOCK *wp);
extern VOID PASCAL NEAR getdtime(char *ts);
extern VOID PASCAL NEAR initchars(VOID);
extern VOID PASCAL NEAR kdelete(VOID);
extern int PASCAL NEAR lowercase(unsigned char *cp);
extern VOID PASCAL NEAR mlabort(CONST char *s);
extern VOID PASCAL NEAR mlerase(VOID);
extern VOID PASCAL NEAR mlferase(VOID);
extern VOID PASCAL NEAR mlforce(CONST char *s);
extern VOID PASCAL NEAR mlout(int c);
extern VOID PASCAL NEAR mlputf(int s);
extern VOID PASCAL NEAR mlputi(int i, int r);
extern VOID PASCAL NEAR mlputli(long l, int r);
extern VOID PASCAL NEAR mlputs(CONST char *s);
extern VOID PASCAL NEAR mlrquery(VOID);
extern VOID PASCAL NEAR modeline(EWINDOW *wp);
extern VOID PASCAL NEAR movecursor(int row, int col);
extern VOID PASCAL NEAR next_kill(VOID);
extern VOID PASCAL NEAR pad(char *s, int len);
extern VOID PASCAL NEAR reeat(int c);
extern VOID PASCAL NEAR reframe(EWINDOW *wp);
extern VOID PASCAL NEAR reform(char *para);
extern VOID PASCAL NEAR rmcclear(VOID);
extern VOID PASCAL NEAR setjtable(VOID);
extern VOID PASCAL NEAR unbind_buf(BUFFER *bp);
extern VOID PASCAL NEAR unqname(char *name);
extern VOID PASCAL NEAR updall(EWINDOW *wp);
extern VOID PASCAL NEAR update(int force);
extern VOID PASCAL NEAR update_size(VOID);
extern VOID PASCAL NEAR upddex(VOID);
extern VOID PASCAL NEAR updext(VOID);
extern VOID PASCAL NEAR updgar(VOID);
extern VOID PASCAL NEAR updone(EWINDOW *wp);
extern VOID PASCAL NEAR updpos(VOID);
extern VOID PASCAL NEAR updupd(int force);
extern VOID PASCAL NEAR upmode(VOID);
extern int uppercase(unsigned char *cp);
extern VOID PASCAL NEAR upwind(VOID);
extern VOID PASCAL NEAR varinit(VOID);
extern VOID PASCAL NEAR varclean(UTABLE *ut);
extern VOID PASCAL NEAR uv_init(UTABLE *ut);
extern VOID PASCAL NEAR uv_clean(UTABLE *ut);
extern VOID PASCAL NEAR vtfree(VOID);
extern VOID cook(VOID);
extern VOID qin(int ch);
extern VOID qrep(int ch);
extern EWINDOW *PASCAL NEAR mousewindow(int row);
extern int PASCAL NEAR wpopup(BUFFER *popbuf);

#if	LIBHELP
extern int lookupword(int f, int n);	/* Lookup a word...		*/
extern int promptlook(int f, int n);	/* Prompt for a word to look up	*/
extern int zaphelp(int f, int n);	/* Zap help window		*/
extern int hlpindex(int f, int n);	/* Help index generator		*/
extern int hidebuffer(int f, int n);	/* Set buffer as invisible	*/
extern VOID initlook(char* lf);

#endif

#if	CTAGS
extern int PASCAL NEAR tagword(int f, int n);	/* vi-like tagging */
extern int PASCAL NEAR retagword(int f, int n);	/* Try again (if redefined) */
extern int PASCAL NEAR backtagword(int f, int n); /* return from tagged word */
#endif

#if DYNMSGS
extern VOID msinit(VOID);
#endif

extern int grabwait(void);
extern int grabnowait(void);

#if	THEOX
#include "theox.h"
#endif

extern int getmline(VOID);

#if	MDSLINE
extern unsigned int tosgraph(unsigned int c);
#endif
/* some library redefinitions */

#if WINXP == 0
char *strrev(char *);
#endif

#if WINXP || WINNT || WINDOW_MSWIN || (MSDOS && IC) || GCC || VMS
#include <stdlib.h>
#include <string.h>
#else
char *getenv(char *);
char *strcat(char *, char *);
char *strcpy(char *, char *);
int  strncmp(char *, char *, int);
char *strchr(char *, int);
int  strcmp(char *, char *);
#if	XVT == 0 || XVTDRIVER == 0
int  strlen(char *);
#if RAMSIZE == 0
char *malloc(int);
VOID free(char *);
#endif
char *realloc(char *block, int siz);
#endif
#endif

#if	JMDEXT | 1
extern	int	notavail();		/* bound to unavailable functions */
#endif

#else   /* PROTO */

/***	global function declarations	***/
#if CALLED
extern int emacs();
#endif

#if HANDLE_WINCH
extern VOID winch_changed();
extern VOID winch_new_size();
#endif

#if DEBUG_SEARCH
int PASCAL NEAR mc_list();
int PASCAL NEAR rmc_list();
VOID PASCAL NEAR mctype_cat();
#endif


extern VOID undo_insert();
extern int undo_op();
extern VOID undo_dump();
extern VOID undo_zot();
extern int PASCAL NEAR undo();
extern int PASCAL NEAR undo_delete();
extern int PASCAL NEAR undo_list();
extern VOID *room();
extern VOID *reroom();

extern int PASCAL NEAR ab_insert();
extern char *PASCAL NEAR ab_lookup();
extern int PASCAL NEAR ab_delete();
extern int PASCAL NEAR ab_clean();
extern BUFFER *PASCAL NEAR bfind();
extern BUFFER *PASCAL NEAR getcbuf();
extern BUFFER *PASCAL NEAR getdefb();
extern BUFFER *PASCAL NEAR getoldb();
extern ESCREEN *PASCAL NEAR init_screen();
extern ESCREEN *PASCAL NEAR lookup_screen();
extern ESCREEN *PASCAL NEAR index_screen();
extern int PASCAL NEAR screen_index();
extern int PASCAL NEAR insert_screen();
extern int PASCAL NEAR select_screen();
extern int PASCAL NEAR free_screen();
extern char *Eallocate();
extern char *dolock();
extern char *getpath();
extern char *gtname();
extern char *PASCAL NEAR bytecopy();
extern char *PASCAL NEAR cmdstr();
extern char *PASCAL NEAR copystr();
extern char *PASCAL NEAR complete();
extern char *PASCAL NEAR envval();
extern char *PASCAL NEAR fixnull();
extern char *PASCAL NEAR flook();
extern char *PASCAL NEAR funval();
extern char *PASCAL NEAR getctext();
extern char *PASCAL NEAR getffile();
extern char *PASCAL NEAR getfname();
extern char *PASCAL NEAR getkill();
extern char *PASCAL NEAR getnfile();
extern char *PASCAL NEAR getreg();
extern char *PASCAL NEAR getval();
extern char *PASCAL NEAR getwlist();
extern char *PASCAL NEAR gtenv();
extern char *PASCAL NEAR gtfilename();
extern char *PASCAL NEAR gtfun();
extern char *PASCAL NEAR gtusr();
extern char *PASCAL NEAR int_asc();
extern char *PASCAL NEAR long_asc();
extern char *PASCAL NEAR ltos();
extern char *PASCAL NEAR makename();
extern char *PASCAL NEAR mklower();
extern char *PASCAL NEAR mkupper();
extern char *PASCAL NEAR namval();
extern char *PASCAL NEAR timeset();
extern char *PASCAL NEAR token();
extern char *PASCAL NEAR transbind();
extern char *PASCAL NEAR trimstr();
extern char *PASCAL NEAR xlat();
extern char *undolock();
extern char *PASCAL NEAR regtostr();
extern int PASCAL NEAR lowerc();
extern int PASCAL NEAR upperc();
#if	THEOSC
extern int* fncmatch();
extern int* getname();
#else
extern int (PASCAL NEAR *PASCAL NEAR fncmatch())();
extern int (PASCAL NEAR *PASCAL NEAR getname())();
#endif
extern int PASCAL NEAR asc_int();
extern VOID dolhello();
extern int dspram();
extern VOID lckerror();
extern VOID lckhello();
extern int xlock();
extern int lockchk();
extern int lockrel();
extern VOID mousehello();
extern int nocrypt();
extern int PASCAL NEAR absv();
extern int PASCAL NEAR add_abbrev();
extern int PASCAL NEAR del_abbrev();
extern int PASCAL NEAR kill_abbrevs();
extern int PASCAL NEAR ins_abbrevs();
extern int PASCAL NEAR def_abbrevs();
extern int PASCAL NEAR addkey();
extern int PASCAL NEAR addkeymap();
extern int PASCAL NEAR addline();
extern int PASCAL NEAR amatch();
extern int PASCAL NEAR backhunt();
extern int PASCAL NEAR backsearch();
extern int PASCAL NEAR biteq();
extern int PASCAL NEAR bktoshell();
extern int PASCAL NEAR boundry();
extern int PASCAL NEAR cclmake();
extern int PASCAL NEAR checknext();
extern int PASCAL NEAR clear_ring();
extern int PASCAL NEAR cycle_ring();
extern int PASCAL NEAR delins();
extern int PASCAL NEAR desfunc();
extern int PASCAL NEAR dispvar();
extern int PASCAL NEAR ueechochar();
extern int PASCAL NEAR echostring();
extern int PASCAL NEAR eq();
extern long PASCAL NEAR ernd();
extern int PASCAL NEAR execkey();
extern int PASCAL NEAR fbound();
extern int PASCAL NEAR fexist();
extern int PASCAL NEAR findcol();
extern int PASCAL NEAR fisearch();
#if	FLABEL
extern int PASCAL NEAR fnclabel();
#endif
extern int PASCAL NEAR forwhunt();
extern int PASCAL NEAR forwsearch();
extern int PASCAL NEAR getcwnum();
extern int PASCAL NEAR getgoal();
extern int PASCAL NEAR getstring();
extern int PASCAL NEAR gettwnum();
extern int PASCAL NEAR gettyp();
extern int PASCAL NEAR getkey();
extern int PASCAL NEAR getwpos();
extern int PASCAL NEAR get_char();
extern int PASCAL NEAR global_var();
#if	DBCS
extern int PASCAL NEAR is2byte();
#endif
extern int PASCAL NEAR is_letter();
extern int PASCAL NEAR is_lower();
extern int PASCAL NEAR is_num();
extern int PASCAL NEAR isearch();
extern int PASCAL NEAR is_upper();
extern int PASCAL NEAR kinsert();
extern int PASCAL NEAR ldelnewline();
extern int PASCAL NEAR linstr();
extern int PASCAL NEAR liteq();
extern int PASCAL NEAR litmake();
extern int PASCAL NEAR lnewline();
extern int PASCAL NEAR local_var();
extern int PASCAL NEAR lookup_color();
extern int PASCAL NEAR lover();
extern int PASCAL NEAR mceq();
extern int PASCAL NEAR mcscanner();
extern int PASCAL NEAR mcstr();
extern int PASCAL NEAR mlprompt();
extern int PASCAL NEAR nextch();
extern int PASCAL NEAR pop();
extern int PASCAL NEAR qreplace();
extern int PASCAL NEAR readpattern();
#if	WINDOW_TEXT
extern VOID PASCAL NEAR refresh_screen();
#endif
extern int PASCAL NEAR reglines();
extern int PASCAL NEAR rename_screen();
extern int PASCAL NEAR replaces();
extern int PASCAL NEAR risearch();
extern int PASCAL NEAR rmcstr();
extern int PASCAL NEAR savematch();
extern int PASCAL NEAR scanmore();
extern int PASCAL NEAR scanner();
extern int PASCAL NEAR setlower();
extern int PASCAL NEAR setlower();
extern int PASCAL NEAR setupper();
extern int PASCAL NEAR setvar();
extern int PASCAL NEAR sindex();
extern int PASCAL NEAR sreplace();
extern int PASCAL NEAR stol();
#if	DBCS
extern int PASCAL NEAR stopback();
extern int PASCAL NEAR stopforw();
#endif
extern int PASCAL NEAR svar();
extern int PASCAL NEAR tgetc();
extern int PASCAL NEAR uneat();
extern VOID PASCAL NEAR unlist_screen();
extern int PASCAL NEAR upscreen();
extern int PASCAL NEAR vtinit();
extern int PASCAL NEAR yank();
extern int PASCAL NEAR yank_pop();
extern int Erelease();
extern int set_key();
extern int xunlock();
extern KEYTAB *getbind();
extern LINE *PASCAL NEAR lalloc();
extern LINE *PASCAL NEAR mouseline();
extern long PASCAL NEAR getlinenum();
extern int PASCAL NEAR adjustmode();
extern int PASCAL NEAR anycb();
extern int PASCAL NEAR apro();
extern int PASCAL NEAR backchar();
extern int PASCAL NEAR backdel();
extern int PASCAL NEAR backline();
extern int PASCAL NEAR backpage();
extern int PASCAL NEAR backword();
extern int PASCAL NEAR bclear();
extern int PASCAL NEAR binary();
extern int PASCAL NEAR bindtokey();
extern int PASCAL NEAR buildlist();
extern int PASCAL NEAR capword();
extern int PASCAL NEAR cex();
extern int PASCAL NEAR cinsert();
extern int PASCAL NEAR clean();
extern int PASCAL NEAR clrmes();
extern int PASCAL NEAR copyregion();
extern int PASCAL NEAR ctlxe();
extern int PASCAL NEAR ctlxlp();
extern int PASCAL NEAR ctlxrp();
extern int PASCAL NEAR ctoec();
extern int PASCAL NEAR ctrlg();
extern int PASCAL NEAR cycle_screens();
extern VOID PASCAL NEAR dcline();
extern int PASCAL NEAR deblank();
extern int PASCAL NEAR debug();
extern int PASCAL NEAR delbword();
extern int PASCAL NEAR delete_screen();
extern int PASCAL NEAR delfword();
extern int PASCAL NEAR delgmode();
extern int PASCAL NEAR delmode();
extern int PASCAL NEAR delwind();
extern int PASCAL NEAR desbind();
extern int PASCAL NEAR desc_abbrevs();
extern int PASCAL NEAR deskey();
extern int PASCAL NEAR desvars();
extern int PASCAL NEAR detab();
extern int PASCAL NEAR dobuf();
extern int PASCAL NEAR docmd();
extern int PASCAL NEAR dofile();
extern int PASCAL NEAR ectoc();
extern int PASCAL NEAR edinit();
extern int PASCAL NEAR editloop();
extern int PASCAL NEAR endword();
extern int PASCAL NEAR enlargewind();
extern int PASCAL NEAR entab();
extern int PASCAL NEAR execbuf();
extern int PASCAL NEAR execcmd();
extern int PASCAL NEAR execfile();
extern int PASCAL NEAR execprg();
extern int PASCAL NEAR execproc();
extern int PASCAL NEAR execute();
extern int PASCAL NEAR ffclose();
extern int PASCAL NEAR ffgetline();
extern int PASCAL NEAR ffputline();
extern int PASCAL NEAR ffropen();
extern int PASCAL NEAR ffwopen();
extern int PASCAL NEAR fileapp();
extern int PASCAL NEAR find_screen();
extern int PASCAL NEAR filefind();
extern int PASCAL NEAR filename();
extern int PASCAL NEAR fileread();
extern int PASCAL NEAR filesave();
extern int PASCAL NEAR filewrite();
extern int PASCAL NEAR fillpara();
extern int PASCAL NEAR uefilter();
extern VOID PASCAL NEAR findvar();
extern int PASCAL NEAR fmatch();
extern int PASCAL NEAR forwchar();
extern int PASCAL NEAR forwdel();
extern int PASCAL NEAR forwline();
extern int PASCAL NEAR forwpage();
extern int PASCAL NEAR forwword();
extern int PASCAL NEAR getccol();
extern int PASCAL NEAR getcmd();
extern int PASCAL NEAR getfence();
extern int PASCAL NEAR getfile();
extern int PASCAL NEAR get_key();
extern int PASCAL NEAR getregion();
extern int PASCAL NEAR gotobob();
extern int PASCAL NEAR gotobol();
extern int PASCAL NEAR gotobop();
extern int PASCAL NEAR gotoeob();
extern int PASCAL NEAR gotoeol();
extern int PASCAL NEAR gotoeop();
extern int PASCAL NEAR gotoline();
extern int PASCAL NEAR gotomark();
extern int PASCAL NEAR help();
extern int PASCAL NEAR ifile();
extern int PASCAL NEAR indent();
extern int PASCAL NEAR indent_region();
extern int PASCAL NEAR insbrace();
extern int PASCAL NEAR insfile();
extern int PASCAL NEAR inspound();
extern int PASCAL NEAR insspace();
extern int PASCAL NEAR inword();
extern int PASCAL NEAR isinword();
extern int PASCAL NEAR ismodeline();
extern int PASCAL NEAR istring();
extern int PASCAL NEAR killbuffer();
extern int PASCAL NEAR killpara();
extern int PASCAL NEAR killregion();
extern int PASCAL NEAR killtext();
extern VOID PASCAL NEAR lchange();
extern int PASCAL NEAR ldelete();
extern VOID PASCAL NEAR lfree();
extern int PASCAL NEAR ldelchar();
extern int PASCAL NEAR linsert();
extern int PASCAL NEAR listbuffers();
extern int PASCAL NEAR list_screens();
extern int PASCAL NEAR lowerregion();
extern int PASCAL NEAR lowerword();
extern int PASCAL NEAR lowrite();
extern int PASCAL NEAR macarg();
extern int PASCAL NEAR macrotokey();
extern int PASCAL NEAR makelist();
extern int PASCAL NEAR undolist();
extern VOID PASCAL NEAR mouse_screen();
extern int PASCAL NEAR screenlist();
extern int PASCAL NEAR meexit();
extern int PASCAL NEAR uemeta();
extern int PASCAL NEAR mlreply();
extern int PASCAL NEAR mlyesno();
extern int PASCAL NEAR mouseoffset();
extern int PASCAL NEAR movemd();
extern int PASCAL NEAR movemu();
extern int PASCAL NEAR mregdown();
extern int PASCAL NEAR mmove();
extern int PASCAL NEAR mregup();
extern int PASCAL NEAR mvdnwind();
extern int PASCAL NEAR mvupwind();
extern int PASCAL NEAR namebuffer();
extern int PASCAL NEAR namedcmd();
extern int PASCAL NEAR narrow();
extern int PASCAL NEAR uenewline();
extern int PASCAL NEAR new_col_org();
extern int PASCAL NEAR new_row_org();
extern int PASCAL NEAR newsize();
extern int PASCAL NEAR newwidth();
extern int PASCAL NEAR nextarg();
extern int PASCAL NEAR nextbuffer();
extern int PASCAL NEAR nextdown();
extern int PASCAL NEAR nextup();
extern int PASCAL NEAR nextwind();
extern int PASCAL NEAR nullproc();
extern int PASCAL NEAR onlywind();
extern int PASCAL NEAR openline();
extern VOID PASCAL NEAR ostring();
extern VOID PASCAL NEAR outstring();
extern int PASCAL NEAR ovstring();
extern int PASCAL NEAR pipecmd();
extern int PASCAL NEAR popbuffer();
extern int PASCAL NEAR prevwind();
extern int PASCAL NEAR putctext();
extern int PASCAL NEAR putline();
extern int PASCAL NEAR quickexit();
extern int PASCAL NEAR quit();
extern int PASCAL NEAR quote();
extern int PASCAL NEAR rdonly();
extern int PASCAL NEAR readin();
extern int PASCAL NEAR uerefresh();
extern int PASCAL NEAR remmark();
extern int PASCAL NEAR reposition();
extern int PASCAL NEAR resetkey();
extern int PASCAL NEAR resize();
extern int PASCAL NEAR resizm();
extern int PASCAL NEAR resterr();
extern int PASCAL NEAR restwnd();
extern int PASCAL NEAR savewnd();
extern int PASCAL NEAR scwrite();
extern int PASCAL NEAR setccol();
extern int PASCAL NEAR setekey();
extern int PASCAL NEAR setfillcol();
extern int PASCAL NEAR setgmode();
extern int PASCAL NEAR setmark();
extern int PASCAL NEAR setmod();
extern int PASCAL NEAR setwlist();
extern int PASCAL NEAR shellprog();
extern int PASCAL NEAR showfiles();
extern int PASCAL NEAR listkeymaps();
extern int PASCAL NEAR showcpos();
extern int PASCAL NEAR shrinkwind();
extern int PASCAL NEAR spal();
extern int PASCAL NEAR spawn();
extern int PASCAL NEAR spawncli();
extern int PASCAL NEAR splitwind();
extern int PASCAL NEAR startup();
extern int PASCAL NEAR storeproc();
extern int PASCAL NEAR strinc();
extern int PASCAL NEAR swapmark();
extern int PASCAL NEAR swbuffer();
extern int PASCAL NEAR uetab();
extern int PASCAL NEAR trim();
extern int PASCAL NEAR ttclose();
extern int PASCAL NEAR ttflush();
extern int PASCAL NEAR ttgetc();
extern int PASCAL NEAR ttopen();
extern int PASCAL NEAR ttputc();
extern int PASCAL NEAR twiddle();
extern int PASCAL NEAR typahead();
extern int PASCAL NEAR unarg();
extern int PASCAL NEAR unbindchar();
extern int PASCAL NEAR unbindkey();
extern int PASCAL NEAR undent_region();
extern int PASCAL NEAR unmark();
extern int PASCAL NEAR upperregion();
extern int PASCAL NEAR upperword();
extern int PASCAL NEAR usebuffer();
extern int PASCAL NEAR viewfile();
extern VOID PASCAL NEAR vteeol();
extern VOID PASCAL NEAR vtmove();
extern VOID PASCAL NEAR vtputc();
extern VOID PASCAL NEAR vttidy();
extern int PASCAL NEAR widen();
extern int PASCAL NEAR wordcount();
extern int PASCAL NEAR wrapword();
extern int PASCAL NEAR writemsg();
extern int PASCAL NEAR writeout();
extern int PASCAL NEAR zotbuf();
extern unsigned int PASCAL NEAR chcase();
extern unsigned int PASCAL NEAR getckey();
extern unsigned int PASCAL NEAR stock();
extern VOID CDECL NEAR mlwrite();
extern VOID PASCAL NEAR ab_init();
extern VOID PASCAL NEAR ab_save();
extern VOID PASCAL NEAR ab_expand();
extern VOID PASCAL NEAR clist_buffer();
extern VOID PASCAL NEAR clist_command();
extern VOID PASCAL NEAR clist_file();
extern VOID PASCAL NEAR comp_buffer();
extern VOID PASCAL NEAR comp_command();
extern VOID PASCAL NEAR comp_file();
extern VOID PASCAL NEAR ecrypt();
extern VOID PASCAL NEAR errormesg();
extern VOID PASCAL NEAR flong_asc();
extern VOID PASCAL NEAR freewhile();
extern VOID PASCAL NEAR getdtime();
extern VOID PASCAL NEAR initchars();
extern VOID PASCAL NEAR kdelete();
extern int PASCAL NEAR lowercase();
extern VOID PASCAL NEAR mcclear();
extern VOID PASCAL NEAR mlabort();
extern VOID PASCAL NEAR mlerase();
extern VOID PASCAL NEAR mlferase();
extern VOID PASCAL NEAR mlforce();
extern VOID PASCAL NEAR mlout();
extern VOID PASCAL NEAR mlputf();
extern VOID PASCAL NEAR mlputi();
extern VOID PASCAL NEAR mlputli();
extern VOID PASCAL NEAR mlputs();
extern VOID PASCAL NEAR mlrquery();
extern VOID PASCAL NEAR modeline();
extern VOID PASCAL NEAR movecursor();
extern VOID PASCAL NEAR next_kill();
extern VOID PASCAL NEAR pad();
extern VOID PASCAL NEAR reeat();
extern VOID PASCAL NEAR reframe();
extern VOID PASCAL NEAR reform();
extern VOID PASCAL NEAR rmcclear();
extern VOID PASCAL NEAR setbit();
extern VOID PASCAL NEAR setjtable();
extern VOID PASCAL NEAR unbind_buf();
extern VOID PASCAL NEAR unqname();
extern VOID PASCAL NEAR updall();
extern VOID PASCAL NEAR update();
extern VOID PASCAL NEAR update_size();
extern VOID PASCAL NEAR upddex();
extern VOID PASCAL NEAR updext();
extern VOID PASCAL NEAR updgar();
extern VOID PASCAL NEAR updone();
extern VOID PASCAL NEAR updpos();
extern VOID PASCAL NEAR upmode();
extern VOID PASCAL NEAR updupd();
extern int PASCAL NEAR uppercase();
extern VOID PASCAL NEAR upwind();
extern VOID PASCAL NEAR varinit();
extern VOID PASCAL NEAR varclean();
extern VOID PASCAL NEAR uv_init();
extern VOID PASCAL NEAR uv_clean();
extern VOID PASCAL NEAR vtfree();
extern VOID cook();
extern VOID qin();
extern VOID qrep();
extern EWINDOW *PASCAL NEAR mousewindow();
extern int PASCAL NEAR wpopup();

#if	CTAGS
extern int PASCAL NEAR tagword();	/* vi-like tagging */
extern int PASCAL NEAR retagword();	/* Try again (if redefined) */
extern int PASCAL NEAR backtagword();	/* return from tagged word */
#endif

#if	LIBHELP
extern	int	lookupword();		/* Lookup a word...		*/
extern	int	promptlook();		/* Prompt for a word to look up	*/
extern	int	zaphelp();		/* Zap help window		*/
extern	int	hlpindex();		/* Help index generator		*/
extern	int	hidebuffer();		/* Set buffer as invisible	*/
#endif

#if	JMDEXT
extern	int	notavail();		/* bound to unavailable functions */
#endif

#if	MDSLINE
extern unsigned int tosgraph();
#endif

/* some library redefinitions */

char *getenv();
char *strcat();
char *strcpy();
char *strrev();
#if	XVT == 0 || XVTDRIVER == 0
int  strlen();
#if RAMSIZE == 0
char *malloc();
VOID free();
#endif
char *realloc();
#endif

#endif
