/*	EPROTO:		Global function prototypes and declarations
			MicroEMACS 4.00

                        written by Daniel Lawrence
                        based on code by Dave G. Conroy,
                        	Steve Wilhite and George Jones

                        Unicode support by Jean-Michel Dubois
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

#if	WINDOW_MSWIN || CURSES
extern int bindtomenu (int f, int n);
extern int macrotomenu (int f, int n);
extern int unbindmenu (int f, int n);
extern void closedrop(void);
extern void menukey(int c);
extern int menudrop(int f, int n);
#endif

#if	WINDOW_MSWIN
extern char * fullpathname (char *PathName, int Nbuf);
extern int vtinitscr (ESCREEN *sp, int nrow, int ncol);
extern int vtsizescr (ESCREEN *sp, int nrow, int ncol);
extern int cutregion (int f, int n);
extern int clipregion (int f, int n);
extern int insertclip (int f, int n);
extern int helpengine (int f, int n);
extern int minimizescreen (int f, int n);
extern int maximizescreen (int f, int n);
extern int restorescreen (int f, int n);
extern int tilescreens (int f, int n);
extern int cascadescreens (int f, int n);
extern int execmenu (int f, int n);
extern int longop (int f);
extern int filenamedlg (char *prompt, char *buf, int nbuf, int fullpath);
extern int vtfreescr (ESCREEN *sp);
extern int unlist_screen(ESCREEN *sp);
extern int mlhistory(VOID);
extern int updscrollbars (ESCREEN *sp, char w_flag);
extern VOID vtscreen (ESCREEN *sp);
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
int mc_list(int f, int n);
int rmc_list(int f, int n);
VOID mctype_cat(char pline[], int mc_type);
#endif

extern VOID undo_insert(OPTYPE op_type, long count, OBJECT op_erand);
extern int undo_op(VOID);
extern VOID undo_dump(VOID);
extern VOID undo_zot(BUFFER *bp);
extern int undo(int f, int n);
extern int undo_delete(int f, int n);
extern int undo_list(int f, int n);
extern VOID *room(int);
extern VOID *reroom(VOID *, int);

extern int ab_insert(char *sym, char *expansion);
extern char *ab_lookup(char *sym);
extern int ab_delete(char *sym);
extern int ab_clean(VOID);
extern BUFFER *bfind(CONST char *bname, int cflag, int bflag);
extern BUFFER *getcbuf(CONST char *prompt, char *defval, int createflag);
extern BUFFER *getdefb(VOID);
extern BUFFER *getoldb(VOID);
extern ESCREEN *init_screen(char *, BUFFER *);
extern ESCREEN *lookup_screen(char *scr_name);
extern ESCREEN *index_screen(int scr_num);
extern int screen_index(ESCREEN *sp);
extern int insert_screen(ESCREEN *sp);
extern int select_screen(ESCREEN *sp, int announce);
extern VOID free_screen(ESCREEN *sp);
extern char *Eallocate(unsigned nbytes);
extern char *dolock(CONST char *fname);
extern char *getpath(char *filespec);
extern char *gtname(char *filespec);
extern char *bytecopy(char *dst, CONST char *src, int maxlen);
extern char *cmdstr(int c, char *seq);
extern char *copystr(char *);
extern char *complete(CONST char *prompt, char *defval, int type, int maxlen);
extern char *envval(int i);
extern CONST char *fixnull(CONST char *s);
extern CONST char *flook(CONST char *fname, int hflag);
extern char *funval(int i);
extern char *getctext(char *rline);
extern char *getffile(char *fspec);
extern CONST char *getfname(KEYTAB *key);
extern char *getkill(VOID);
extern char *getnfile(VOID);
extern char *getreg(char *value);
extern CONST char *getval(char *token);
extern char *getwlist(char *buf);
extern CONST char *gtenv(CONST char *vname);
extern char *gtfilename(CONST char *prompt);
extern CONST char *gtfun(char *fname);
extern char *gtusr(char *vname);
extern char *int_asc(int i);
extern char *long_asc(long num);
extern CONST char *ltos(int val);
extern CONST char *makename(char *bname, CONST char *fname);
extern char *mklower(char *str);
extern char *mkupper(char *str);
extern char *namval(int index);
extern char *timeset(VOID);
extern char *token(char *src, char *tok, int size);
extern CONST char *transbind(CONST char *skey);
extern char *trimstr(char *s);
extern char *xlat(char *source, char *lookup, char *trans);
extern char *undolock(CONST char *fname);
extern char *regtostr(char *buf, REGION *region);
#if	UTF8
extern unsigned int chcase(unsigned int ch);
extern unsigned int lowerc(unsigned int ch);
extern unsigned int upperc(unsigned int ch);
extern int setlower(char *ch, char *val);
extern int setupper(char *ch, char *val);
#else
extern unsigned int chcase(register unsigned int ch);
extern VOID initchars(VOID);
extern int lowerc(char ch);
extern int upperc(char ch);
#endif
extern int setlower(char *ch, char *val);
extern int setupper(char *ch, char *val);
extern int cycle_ring(int f, int n);
#if	ZTC || TURBO || IC
extern int (*fncmatch(char *fname))(int, int);
extern int (*getname(CONST char *prompt))(int, int);
#else	/* Sun (and others?) screwed up the prototyping.*/
#if	THEOSC
extern int *fncmatch(char* fname);
extern int *getname(CONST char* fname);
#else
extern int (*fncmatch(char *fname))(VOID);
extern int (*getname(CONST char *prompt))(VOID);
#endif
#endif
extern int asc_int(char *st);
extern VOID dolhello(VOID);
extern int dspram(VOID);
extern VOID lckerror(char *errstr);
extern VOID lckhello(VOID);
extern int xlock(CONST char *fname);
extern int lockchk(CONST char *fname);
extern int lockrel(VOID);
extern VOID mousehello(VOID);
extern int nocrypt(VOID);
extern int absv(int x);
extern int add_abbrev(int f, int n);
extern int del_abbrev(int f, int n);
extern int kill_abbrevs(int f, int n);
extern int ins_abbrevs(int f, int n);
extern int def_abbrevs(int f, int n);
extern int addline(BUFFER *bp, CONST char *text);
extern int backhunt(int f, int n);
extern int backsearch(int f, int n);
extern int bktoshell(int f, int n);
extern int boundry(LINE *curline, int curoff, int dir);
extern int checknext(int chr, int dir);
extern int clear_ring(int f, int n);
extern int delins(int dlength, char *instr, int use_rmc);
extern int desfunc(int f, int n);
extern int dispvar(int f, int n);
extern int ueechochar(unsigned char c);
extern int echostring(char *, int, int);
extern int eq(register unsigned char bc, register unsigned char pc);
#if	UTF8
extern int weq(unsigned int bc, unsigned int pc);
#endif
extern long ernd(VOID);
extern int execkey(KEYTAB *key, int f, int n);
extern int fbound(DELTA *tbl, int jump, LINE **pcurline, int *pcuroff, int dir);
extern int fexist(char *fname);
extern int findcol(LINE *lp, int pos);
extern int fisearch(int f, int n);
#if	FLABEL
extern int fnclabel(int f, int n);
#endif
extern int forwhunt(int f, int n);
extern int forwsearch(int f, int n);
extern int getcwnum(VOID);
extern int getgoal(LINE *dlp);
extern int getstring(unsigned char *buf, int nbuf, int eolchar);
extern int gettwnum(VOID);
extern int gettyp(CONST char *token);
extern int getkey(VOID);
extern int getwpos(VOID);
extern int get_char(VOID);
extern int global_var(int f, int n);
#if	DBCS
extern int is2byte(char *sp, char *cp);
#endif
#if	UTF8
extern int is_letter(unsigned int ch);
extern int is_lower(unsigned int ch);
extern int is_upper(unsigned int ch);
#else
extern int is_letter(char ch);
extern int is_lower(char ch);
extern int is_upper(char ch);
#endif
extern int is_num(char *st);
extern int isearch(int dir);
extern int kinsert(int back, char c);
extern int ldelnewline(VOID);
extern int linstr(CONST char *instr);
extern int liteq(LINE **curline, int *curpos, int direct, char *lstring);
extern int lnewline(VOID);
extern int local_var(int f, int n);
extern int lookup_color(char *sp);
extern int lover(CONST char *ostr);
extern int mcstr(VOID);
extern int mlprompt(CONST char *, char *, int);
extern int movelocalpoint(int n, int *pcuroff, LINE **pcurline);
extern int nextch(LINE **pcurline, int *pcuroff, int dir);
extern int pop(BUFFER *popbuffer);
extern int qreplace(int f, int n);
extern int readpattern(CONST char *prompt, char apat[], int srch);
#if	WINDOW_TEXT
extern VOID refresh_screen(ESCREEN *sp);
#endif
extern int reglines(VOID);
extern int rename_screen(int f, int n);
extern int replaces(int kind, int f, int n);
extern int risearch(int f, int n);
extern int rmcstr(VOID);
extern int savematch(VOID);
extern int scanmore(int dir);
#if MAGIC == 0
extern int scanner(int direct, int beg_or_end, int repeats);
#else
extern int amatch(MC *mcptr, int direct, LINE **pcwline, int *pcwoff);
extern int biteq(int bc, EBITMAP cclmap);
extern int cclmake(char **ppatptr, MC *mcptr);
extern int litmake(char **ppatptr, MC *mcptr);
extern int mceq(unsigned char bc, MC *mt);
extern int mcscanner(MC *mcpatrn, int direct, int beg_or_end, int repeats);
extern VOID setbit(int bc, EBITMAP cclmap);
#endif
extern int setvar(int f, int n);
extern int sindex(CONST char *source, CONST char *pattern);
extern int srindex(CONST char* source, CONST char* pattern);
extern int sreplace(int f, int n);
extern int stol(char *val);
#if	DBCS
extern int stopback(VOID);
extern int stopforw(VOID);
#endif
extern int svar(VDESC *var, char *value);
extern int tgetc(VOID);
extern int uneat(VOID);
extern VOID unlist_screen(ESCREEN *sp);
extern int upscreen(int f, int n);
extern int vtinit(VOID);
extern int yank(int f, int n);
extern int yank_pop(int f, int n);
extern int Erelease(char *mp);
extern int set_key(KEYTAB *key, char *name);
extern int xunlock(char *fname);
extern KEYTAB *getbind(int c);
extern LINE *lalloc(int used);
extern LINE *mouseline(EWINDOW *wp, int row);
extern long getlinenum(BUFFER *bp, LINE *sline);
extern int addkey(char * seq, int fn);
extern int addkeymap(int f, int n);
extern int adjustmode(int kind, int global);
extern int anycb(VOID);
extern int apro(int f, int n);
extern int backchar(int f, int n);
extern int backdel(int f, int n);
extern int backline(int f, int n);
extern int backpage(register int f, register int n);
extern int backword(int f, int n);
extern int bclear(BUFFER *bp);
#if	THEOSC
extern int binary();
#else
extern int binary(CONST char *key, char *(*tval)(), int tlength, int klength);
#endif
extern int bindtokey(int f, int n);
extern int buildlist(int type, char *mstring);
extern int capword(int f, int n);
extern int cex(int f, int n);
extern int cinsert(VOID);
extern int clean(VOID);
extern int clrmes(int f, int n);
extern int copyregion(int f, int n);
extern int ctlxe(int f, int n);
extern int ctlxlp(int f, int n);
extern int ctlxrp(int f, int n);
extern int ctoec(int c);
extern int ctrlg(int f, int n);
extern int cycle_screens(int f, int n);
extern VOID dcline(int argc, char *argv[], int firstflag);
extern int deblank(int f, int n);
extern int debug(BUFFER *bp, char *eline, int *skipflag);
extern int delbword(int f, int n);
extern int delete_screen(int f, int n);
extern int delfword(int f, int n);
extern int delgmode(int f, int n);
extern int delmode(int f, int n);
extern int delwind(int f, int n);
extern int desc_abbrevs(int f, int n);
extern int desbind(int f, int n);
extern int deskey(int f, int n);
extern int desvars(int f, int n);
extern int detab(int f, int n);
extern int dobuf(BUFFER *bp);
extern int docmd(char *cline);
extern int dofile(CONST char *fname);
extern int ectoc(int c);
extern int edinit(char bname[]);
extern int editloop(VOID);
extern int endword(int f, int n);
extern int enlargewind(int f, int n);
extern int entab(int f, int n);
extern int execbuf(int f, int n);
extern int execcmd(int f, int n);
extern int execfile(int f, int n);
extern int execprg(int f, int n);
extern int execproc(int f, int n);
extern int execute(int c, int f, int n);
extern int ffclose(VOID);
extern int ffgetline(int *nbytes);
extern int ffputline(char buf[], int nbuf);
extern int ffropen(CONST char *fn);
extern int ffwopen(CONST char *fn, CONST char *mode);
extern int fileapp(int f, int n);
extern int find_screen(int f, int n);
extern int filefind(int f, int n);
extern int filename(int f, int n);
extern int fileread(int f, int n);
extern int filesave(int f, int n);
extern int filewrite(int f, int n);
extern int fillpara(int f, int n);
extern int uefilter(int f, int n);
extern VOID findvar(char *var, VDESC *vd, int size, int scope);
extern int fmatch(char ch);
extern int forwchar(int f, int n);
extern int forwdel(int f, int n);
extern int forwline(int f, int n);
extern int forwpage(int f, int n);
extern int forwword(int f, int n);
extern int getccol(int bflg);
extern int getcmd(VOID);
extern int getfence(int f, int n);
extern int getfile(char fname[], int lockfl);
extern int get_key(VOID);
extern int getregion(REGION *rp);
extern int gotobob(int f, int n);
extern int gotobol(int f, int n);
extern int gotobop(int f, int n);
extern int gotoeob(int f, int n);
extern int gotoeol(int f, int n);
extern int gotoeop(int f, int n);
extern int gotoline(int f, int n);
extern int gotomark(int f, int n);
extern int help(int f, int n);
extern int ifile(char fname[]);
extern int indent(int f, int n);
extern int indent_region(int f, int n);
extern int insbrace(int n, int c);
extern int insfile(int f, int n);
extern int inspound(VOID);
extern int insspace(int f, int n);
extern int inword(VOID);
#if	UTF8
extern int isinword(unsigned int c);
#else
extern int isinword(char c);
#endif
extern int ismodeline(EWINDOW *wp, int row);
extern int istring(int f, int n);
extern int killbuffer(int f, int n);
extern int killpara(int f, int n);
extern int killregion(int f, int n);
extern int killtext(int f, int n);
extern VOID lchange(register int flag);
extern int ldelete(long n, int kflag);
extern VOID lfree(LINE *lp);
#if	UTF8
extern int ldelchar(long n, int kflag);
extern int linsert(int n, unsigned int c);
#else
extern int linsert(int n, char c);
#endif
extern int listbuffers(int f, int n);
extern int list_screens(int f, int n);
extern int lowerregion(int f, int n);
extern int lowerword(int f, int n);
#if	UTF8
extern int lowrite(unsigned int c);
#else
extern int lowrite(int c);
#endif
extern int macarg(char *tok);
extern int macrotokey(int f, int n);
extern int makelist(int iflag);
extern int movelocalpoint();
extern int undolist();
extern VOID mouse_screen(VOID);
extern int screenlist(int iflag);
extern int meexit(int status);
extern int uemeta(int f, int n);
extern int mlreply(CONST char *prompt, char *buf, int nbuf);
extern int mlyesno(CONST char *prompt);
extern int mouseoffset(EWINDOW *wp, LINE *lp, int col);
extern int movemd(int f, int n);
extern int movemu(int f, int n);
extern int mregdown(int f, int n);
extern int mmove(int f, int n);
extern int mregup(int f, int n);
extern int mvdnwind(int f, int n);
extern int mvupwind(int f, int n);
extern int namebuffer(int f, int n);
extern int namedcmd(int f, int n);
extern int narrow(int f, int n);
extern int new_line(int f, int n);
extern int new_col_org(int f, int n);
extern int new_row_org(int f, int n);
extern int newsize(int f, int n);
extern int newwidth(int f, int n);
extern int nextarg(CONST char *prompt, char *buffer, int size, int terminator);
extern int nextbuffer(int f, int n);
extern int nextdown(int f, int n);
extern int nextup(int f, int n);
extern int nextwind(int f, int n);
extern int nullproc(int f, int n);
extern int onlywind(int f, int n);
extern int openline(int f, int n);
extern VOID ostring(CONST char *s);
extern VOID outstring(CONST char *s);
extern int ovstring(int f, int n);
extern int pipecmd(int f, int n);
extern int popbuffer(int f, int n);
extern int prevwind(int f, int n);
extern int putctext(char *iline);
extern int putline(int row, int col, char buf[]);
extern int quickexit(int f, int n);
extern int quit(int f, int n);
extern int quote(int f, int n);
extern int rdonly(VOID);
extern int readin(CONST char *fname, int lockfl);
extern int uerefresh(int f, int n);
extern int remmark(int f, int n);
extern int reposition(int f, int n);
extern int resetkey(VOID);
extern int resize(int f, int n);
extern int resizm(int f, int n);
extern int resizm2(int f, int n);
extern int resterr(VOID);
extern int restwnd(int f, int n);
extern int savewnd(int f, int n);
extern int scwrite(int row, char *outstr, int forg, int bacg,
				int revleft, int revright);
extern int setccol(int pos);
extern int setekey(int f, int n);
extern int setfillcol(int f, int n);
extern int setgmode(int f, int n);
extern int setmark(int f, int n);
extern int setmod(int f, int n);
extern int setwlist(char *wclist);
extern int shellprog(char *cmd);
extern int showcpos(int f, int n);
extern int showfiles(int f, int n);
extern int listkeymaps(int f, int n);
extern int shrinkwind(int f, int n);
extern int spal(char *pstr);
extern int spawn(int f, int n);
extern int spawncli(int f, int n);
extern int splitwind(int f, int n);
extern int startup(char *sfname);
extern int storeproc(int f, int n);
extern int strinc(char *source, char *sub);
extern int swapmark(int f, int n);
extern int swbuffer(BUFFER *bp);
extern int uetab(int f, int n);
extern int trim(int f, int n);
extern int ttclose(VOID);
extern int ttflush(VOID);
extern int ttgetc(VOID);
extern int ttopen(VOID);
extern int ttputc(int c);
extern int twiddle(int f, int n);
extern int typahead(VOID);
extern int unarg(VOID);
extern int unbindchar(int c);
extern int unbindkey(int f, int n);
extern int undent_region(int f, int n);
extern int unmark(int f, int n);
extern int upperregion(int f, int n);
extern int upperword(int f, int n);
extern int usebuffer(int f, int n);
extern int viewfile(int f, int n);
extern VOID vteeol(VOID);
extern VOID vtmove(int row, int col);
extern VOID vtputc(int c);
extern VOID vttidy(VOID);
extern int widen(int f, int n);
extern int wordcount(int f, int n);
extern int wrapword(int f, int n);
extern int writemsg(int f, int n);
extern int writeout(char *fn, char *mode);
extern int zotbuf(BUFFER *bp);
extern unsigned int getckey(int mflag);
extern unsigned int stock(unsigned char *keyname);
#if	VARARG && VARG
#if	GCC
extern VOID CDECL mlwrite(va_alist);
#else
extern VOID CDECL mlwrite(va_dcl);
#endif /* GCC */
#else
extern VOID CDECL mlwrite(CONST char *fmt, ...);
#endif
extern VOID ab_init(VOID);
extern VOID ab_save(char c);
extern VOID ab_expand(VOID);
extern VOID clist_buffer(char *name, int *cpos);
extern VOID clist_command(char *name, int *cpos);
extern VOID clist_file(char *name, int *cpos);
extern VOID comp_buffer(char *name, int *cpos);
extern VOID comp_command(char *name, int *cpos);
#if	!WINDOW_MSWIN
extern VOID comp_file(char *name, int *cpos);
#endif
extern VOID ecrypt(char *bptr, unsigned len);
extern VOID errormesg(CONST char *mesg, BUFFER *bp, LINE *lp);
extern VOID flong_asc(char buf[], int width, long num);
extern VOID freewhile(WHBLOCK *wp);
extern VOID getdtime(char *ts);
extern VOID kdelete(VOID);
extern int lowercase(unsigned char *cp);
extern VOID mcclear(VOID);
extern VOID mlabort(CONST char *s);
extern VOID mlerase(VOID);
extern VOID mlferase(VOID);
extern VOID mlforce(CONST char *s);
extern VOID mlout(int c);
extern VOID mlputf(int s);
extern VOID mlputi(int i, int r);
extern VOID mlputli(long l, int r);
extern VOID mlputs(CONST char *s);
extern VOID mlrquery(VOID);
extern VOID modeline(EWINDOW *wp);
extern VOID movecursor(int row, int col);
extern VOID next_kill(VOID);
extern VOID pad(char *s, int len);
extern VOID reeat(int c);
extern VOID reframe(EWINDOW *wp);
extern VOID reform(char *para);
extern VOID rmcclear(VOID);
extern VOID setjtable(VOID);
extern VOID unbind_buf(BUFFER *bp);
extern VOID unqname(char *name);
extern VOID updall(EWINDOW *wp);
extern VOID update(int force);
extern VOID update_size(VOID);
extern VOID upddex(VOID);
extern VOID updext(VOID);
extern VOID updgar(VOID);
extern VOID updone(EWINDOW *wp);
extern VOID updpos(VOID);
extern VOID updupd(int force);
extern VOID upmode(VOID);
extern int uppercase(unsigned char *cp);
extern VOID upwind(VOID);
extern VOID varinit(VOID);
extern VOID varclean(UTABLE *ut);
extern VOID uv_init(UTABLE *ut);
extern VOID uv_clean(UTABLE *ut);
extern VOID vtfree(VOID);
extern VOID cook(VOID);
extern VOID qin(int ch);
extern VOID qrep(int ch);
extern EWINDOW *mousewindow(int row);
extern int wpopup(BUFFER *popbuf);

#if	LIBHELP
extern int lookupword(int f, int n);	/* Lookup a word...		*/
extern int promptlook(int f, int n);	/* Prompt for a word to look up	*/
extern int zaphelp(int f, int n);	/* Zap help window		*/
extern int hlpindex(int f, int n);	/* Help index generator		*/
extern int hidebuffer(int f, int n);	/* Set buffer as invisible	*/
extern VOID initlook(char* lf);

#endif

#if	CTAGS
extern int tagword(int f, int n);	/* vi-like tagging */
extern int retagword(int f, int n);	/* Try again (if redefined) */
extern int backtagword(int f, int n); /* return from tagged word */
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
int mc_list();
int rmc_list();
VOID mctype_cat();
#endif


extern VOID undo_insert();
extern int undo_op();
extern VOID undo_dump();
extern VOID undo_zot();
extern int undo();
extern int undo_delete();
extern int undo_list();
extern VOID *room();
extern VOID *reroom();

extern int ab_insert();
extern char *ab_lookup();
extern int ab_delete();
extern int ab_clean();
extern BUFFER *bfind();
extern BUFFER *getcbuf();
extern BUFFER *getdefb();
extern BUFFER *getoldb();
extern ESCREEN *init_screen();
extern ESCREEN *lookup_screen();
extern ESCREEN *index_screen();
extern int screen_index();
extern int insert_screen();
extern int select_screen();
extern VOID free_screen();
extern char *Eallocate();
extern char *dolock();
extern char *getpath();
extern char *gtname();
extern char *bytecopy();
extern char *cmdstr();
extern char *copystr();
extern char *complete();
extern char *envval();
extern CONST char *fixnull();
extern char *flook();
extern char *funval();
extern char *getctext();
extern char *getffile();
extern CONST char *getfname();
extern char *getkill();
extern char *getnfile();
extern char *getreg();
extern CONST char *getval();
extern char *getwlist();
extern CONST char *gtenv();
extern char *gtfilename();
extern CONST char *gtfun();
extern char *gtusr();
extern char *int_asc();
extern char *long_asc();
extern CONST char *ltos();
extern char *makename();
extern char *mklower();
extern char *mkupper();
extern char *namval();
extern char *timeset();
extern char *token();
extern char *transbind();
extern char *trimstr();
extern char *xlat();
extern char *undolock();
extern char *regtostr();
extern VOID initchars();
extern int lowerc();
extern int upperc();
extern int setlower();
extern int setupper();
#if	THEOSC
extern int* fncmatch();
extern int* getname();
#else
extern int (*fncmatch())();
extern int (*getname())();
#endif
extern int asc_int();
extern VOID dolhello();
extern int dspram();
extern VOID lckerror();
extern VOID lckhello();
extern int xlock();
extern int lockchk();
extern int lockrel();
extern VOID mousehello();
extern int nocrypt();
extern int absv();
extern int add_abbrev();
extern int del_abbrev();
extern int kill_abbrevs();
extern int ins_abbrevs();
extern int def_abbrevs();
extern int addkey();
extern int addkeymap();
extern int addline();
extern int amatch();
extern int backhunt();
extern int backsearch();
extern int biteq();
extern int bktoshell();
extern int boundry();
extern int cclmake();
extern int checknext();
extern int clear_ring();
extern int cycle_ring();
extern int delins();
extern int desfunc();
extern int dispvar();
extern int ueechochar();
extern int echostring();
extern int eq();
extern long ernd();
extern int execkey();
extern int fbound();
extern int fexist();
extern int findcol();
extern int fisearch();
#if	FLABEL
extern int fnclabel();
#endif
extern int forwhunt();
extern int forwsearch();
extern int getcwnum();
extern int getgoal();
extern int getstring();
extern int gettwnum();
extern int gettyp();
extern int getkey();
extern int getwpos();
extern int get_char();
extern int global_var();
#if	DBCS
extern int is2byte();
#endif
extern int is_letter();
extern int is_lower();
extern int is_num();
extern int isearch();
extern int is_upper();
extern int kinsert();
extern int ldelnewline();
extern int linstr();
extern int liteq();
extern int litmake();
extern int lnewline();
extern int local_var();
extern int lookup_color();
extern int lover();
extern int mceq();
extern int mcscanner();
extern int mcstr();
extern int mlprompt();
extern int nextch();
extern int pop();
extern int qreplace();
extern int readpattern();
#if	WINDOW_TEXT
extern VOID refresh_screen();
#endif
extern int reglines();
extern int rename_screen();
extern int replaces();
extern int risearch();
extern int rmcstr();
extern int savematch();
extern int scanmore();
extern int scanner();
extern int setvar();
extern int sindex();
extern int srindex();
extern int sreplace();
extern int stol();
#if	DBCS
extern int stopback();
extern int stopforw();
#endif
extern int svar();
extern int tgetc();
extern int uneat();
extern VOID unlist_screen();
extern int upscreen();
extern int vtinit();
extern int yank();
extern int yank_pop();
extern int Erelease();
extern int set_key();
extern int xunlock();
extern KEYTAB *getbind();
extern LINE *lalloc();
extern LINE *mouseline();
extern long getlinenum();
extern int adjustmode();
extern int anycb();
extern int apro();
extern int backchar();
extern int backdel();
extern int backline();
extern int backpage();
extern int backword();
extern int bclear();
extern int binary();
extern int bindtokey();
extern int buildlist();
extern int capword();
extern int cex();
extern int cinsert();
extern int clean();
extern int clrmes();
extern int copyregion();
extern int ctlxe();
extern int ctlxlp();
extern int ctlxrp();
extern int ctoec();
extern int ctrlg();
extern int cycle_screens();
extern VOID dcline();
extern int deblank();
extern int debug();
extern int delbword();
extern int delete_screen();
extern int delfword();
extern int delgmode();
extern int delmode();
extern int delwind();
extern int desbind();
extern int desc_abbrevs();
extern int deskey();
extern int desvars();
extern int detab();
extern int dobuf();
extern int docmd();
extern int dofile();
extern int ectoc();
extern int edinit();
extern int editloop();
extern int endword();
extern int enlargewind();
extern int entab();
extern int execbuf();
extern int execcmd();
extern int execfile();
extern int execprg();
extern int execproc();
extern int execute();
extern int ffclose();
extern int ffgetline();
extern int ffputline();
extern int ffropen();
extern int ffwopen();
extern int fileapp();
extern int find_screen();
extern int filefind();
extern int filename();
extern int fileread();
extern int filesave();
extern int filewrite();
extern int fillpara();
extern int uefilter();
extern VOID findvar();
extern int fmatch();
extern int forwchar();
extern int forwdel();
extern int forwline();
extern int forwpage();
extern int forwword();
extern int getccol();
extern int getcmd();
extern int getfence();
extern int getfile();
extern int get_key();
extern int getregion();
extern int gotobob();
extern int gotobol();
extern int gotobop();
extern int gotoeob();
extern int gotoeol();
extern int gotoeop();
extern int gotoline();
extern int gotomark();
extern int help();
extern int ifile();
extern int indent();
extern int indent_region();
extern int insbrace();
extern int insfile();
extern int inspound();
extern int insspace();
extern int inword();
extern int isinword();
extern int ismodeline();
extern int istring();
extern int killbuffer();
extern int killpara();
extern int killregion();
extern int killtext();
extern VOID lchange();
extern int ldelete();
extern VOID lfree();
extern int ldelchar();
extern int linsert();
extern int listbuffers();
extern int list_screens();
extern int lowerregion();
extern int lowerword();
extern int lowrite();
extern int macarg();
extern int macrotokey();
extern int makelist();
extern int undolist();
extern VOID mouse_screen();
extern int screenlist();
extern int meexit();
extern int uemeta();
extern int mlreply();
extern int mlyesno();
extern int mouseoffset();
extern int movemd();
extern int movemu();
extern int mregdown();
extern int mmove();
extern int mregup();
extern int mvdnwind();
extern int mvupwind();
extern int namebuffer();
extern int namedcmd();
extern int narrow();
extern int new_line();
extern int new_col_org();
extern int new_row_org();
extern int newsize();
extern int newwidth();
extern int nextarg();
extern int nextbuffer();
extern int nextdown();
extern int nextup();
extern int nextwind();
extern int nullproc();
extern int onlywind();
extern int openline();
extern VOID ostring();
extern VOID outstring();
extern int ovstring();
extern int pipecmd();
extern int popbuffer();
extern int prevwind();
extern int putctext();
extern int putline();
extern int quickexit();
extern int quit();
extern int quote();
extern int rdonly();
extern int readin();
extern int uerefresh();
extern int remmark();
extern int reposition();
extern int resetkey();
extern int resize();
extern int resizm();
extern int resterr();
extern int restwnd();
extern int savewnd();
extern int scwrite();
extern int setccol();
extern int setekey();
extern int setfillcol();
extern int setgmode();
extern int setmark();
extern int setmod();
extern int setwlist();
extern int shellprog();
extern int showfiles();
extern int listkeymaps();
extern int showcpos();
extern int shrinkwind();
extern int spal();
extern int spawn();
extern int spawncli();
extern int splitwind();
extern int startup();
extern int storeproc();
extern int strinc();
extern int swapmark();
extern int swbuffer();
extern int uetab();
extern int trim();
extern int ttclose();
extern int ttflush();
extern int ttgetc();
extern int ttopen();
extern int ttputc();
extern int twiddle();
extern int typahead();
extern int unarg();
extern int unbindchar();
extern int unbindkey();
extern int undent_region();
extern int unmark();
extern int upperregion();
extern int upperword();
extern int usebuffer();
extern int viewfile();
extern VOID vteeol();
extern VOID vtmove();
extern VOID vtputc();
extern VOID vttidy();
extern int widen();
extern int wordcount();
extern int wrapword();
extern int writemsg();
extern int writeout();
extern int zotbuf();
extern unsigned int chcase();
extern unsigned int getckey();
extern unsigned int stock();
extern VOID CDECL mlwrite();
extern VOID ab_init();
extern VOID ab_save();
extern VOID ab_expand();
extern VOID clist_buffer();
extern VOID clist_command();
extern VOID clist_file();
extern VOID comp_buffer();
extern VOID comp_command();
extern VOID comp_file();
extern VOID ecrypt();
extern VOID errormesg();
extern VOID flong_asc();
extern VOID freewhile();
extern VOID getdtime();
extern VOID kdelete();
extern int lowercase();
extern VOID mcclear();
extern VOID mlabort();
extern VOID mlerase();
extern VOID mlferase();
extern VOID mlforce();
extern VOID mlout();
extern VOID mlputf();
extern VOID mlputi();
extern VOID mlputli();
extern VOID mlputs();
extern VOID mlrquery();
extern VOID modeline();
extern VOID movecursor();
extern VOID next_kill();
extern VOID pad();
extern VOID reeat();
extern VOID reframe();
extern VOID reform();
extern VOID rmcclear();
extern VOID setbit();
extern VOID setjtable();
extern VOID unbind_buf();
extern VOID unqname();
extern VOID updall();
extern VOID update();
extern VOID update_size();
extern VOID upddex();
extern VOID updext();
extern VOID updgar();
extern VOID updone();
extern VOID updpos();
extern VOID upmode();
extern VOID updupd();
extern int uppercase();
extern VOID upwind();
extern VOID varinit();
extern VOID varclean();
extern VOID uv_init();
extern VOID uv_clean();
extern VOID vtfree();
extern VOID cook();
extern VOID qin();
extern VOID qrep();
extern EWINDOW *mousewindow();
extern int wpopup();

#if	CTAGS
extern int tagword();	/* vi-like tagging */
extern int retagword();	/* Try again (if redefined) */
extern int backtagword();	/* return from tagged word */
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
