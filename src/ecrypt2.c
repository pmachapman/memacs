/*	ECRYPT2.C:	Modern file encryption for MicroEMACS

	Files saved in CRYPT mode are encrypted with XChaCha20-Poly1305,
	using a key made from the user's password with Argon2id. Both come
	from Monocypher (src/monocypher). A wrong password or a changed
	file is detected instead of giving garbage.

	File layout (all numbers little-endian):

		 0	8	magic: 0 'M' 'E' 'C' 'R' 'Y' 'P' 'T'
		 8	1	format version (1)
		 9	1	password hashing method (1 = Argon2id)
		10	2	reserved, 0
		12	4	Argon2 memory, in KiB blocks
		16	4	Argon2 passes
		20	16	salt
		36	24	nonce
		60	n	encrypted text
		60+n	16	authentication tag

	The 60 header bytes are authenticated along with the text.

	Files encrypted by older versions (the DLH-POLY-86-B cipher in
	crypt.c) are still read. They are written in the new format the
	next time they are saved.

	Encryption works on the whole file at once, so in CRYPT mode
	ffropen() reads and decrypts the file into memory and ffgetline()
	reads lines from there, and ffputline() collects the lines in
	memory for ffclose() to encrypt and write.

	Prompts happen in resetkey(), before the file routines turn off
	the keyboard.
*/

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	"monocypher/monocypher.h"
#include	"estruct.h"
#include	"eproto.h"
#include	"edef.h"
#include	"elang.h"

#if	CRYPT2

#define	HDRLEN		60
#define	TAGLEN		16
#define	KDF_ARGON2ID	1
#define	DEF_BLOCKS	65536		/* 64 MiB			*/
#define	DEF_PASSES	3
#define	MIN_BLOCKS	8
#define	MAX_BLOCKS	1048576 	/* 1 GiB			*/
#define	MAX_PASSES	64

static CONST unsigned char magic[8] = {0, 'M', 'E', 'C', 'R', 'Y', 'P', 'T'};

/* Per-buffer key state, hung off bp->b_ckey. The password itself is
   kept in bp->b_key only until a key has been made from it. */

typedef struct {
	int ok;				/* key/salt below are valid	*/
	int pwok;			/* b_key was typed twice	*/
	int legacy;			/* last read was the old format	*/
	uint8_t key[32];
	uint8_t salt[16];
	uint32_t blocks, passes;
} CKEY;

/* State of the file being read or written */

#define	CR_NONE		0
#define	CR_READ		1
#define	CR_WRITE	2

NOSHARE extern FILE *ffp;		/* the open file (fileio.c)	*/
NOSHARE int cr_active = CR_NONE;	/* reading/writing via memory?	*/
static unsigned char *mbuf = NULL;	/* the file's plain text	*/
static size_t mlen = 0;			/* bytes in mbuf		*/
static size_t msize = 0;		/* bytes allocated		*/
static size_t mpos = 0;			/* read position		*/
static char oppw[NPAT];			/* one-off password for this file */
static int upgraded = FALSE;		/* last save upgraded old format */
static uint8_t wnonce[24];		/* nonce for the file being written */

/* ------------------------------------------------------------------ */
/*	Helpers								*/
/* ------------------------------------------------------------------ */

static CKEY *getck(BUFFER *bp, int create)
{
	if (bp->b_ckey == NULL && create) {
		bp->b_ckey = (char *)malloc(sizeof(CKEY));
		if (bp->b_ckey != NULL)
			memset(bp->b_ckey, 0, sizeof(CKEY));
	}
	return((CKEY *)bp->b_ckey);
}

static VOID wipe(VOID *p, size_t n)
{
	crypto_wipe(p, n);
}

static uint32_t get32(CONST unsigned char *p)
{
	return((uint32_t)p[0] | ((uint32_t)p[1] << 8) |
	       ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24));
}

static VOID put32(unsigned char *p, uint32_t v)
{
	p[0] = (unsigned char)v;
	p[1] = (unsigned char)(v >> 8);
	p[2] = (unsigned char)(v >> 16);
	p[3] = (unsigned char)(v >> 24);
}

static VOID freembuf(VOID)
{
	if (mbuf != NULL) {
		wipe(mbuf, msize);
		free(mbuf);
	}
	mbuf = NULL;
	mlen = msize = mpos = 0;
}

static int growmbuf(size_t need)
{
	unsigned char *nb;
	size_t nsize;

	if (need <= msize)
		return(TRUE);
	nsize = msize ? msize : 4096;
	while (nsize < need)
		nsize *= 2;
	nb = (unsigned char *)malloc(nsize);
	if (nb == NULL)
		return(FALSE);
	if (mbuf != NULL) {
		memcpy(nb, mbuf, mlen);
		wipe(mbuf, msize);
		free(mbuf);
	}
	mbuf = nb;
	msize = nsize;
	return(TRUE);
}

/* Fill buf with random bytes from the operating system */

#if	defined(_WIN32)
#if	defined(_MSC_VER)
#pragma comment(lib, "bcrypt.lib")
#endif
extern long __stdcall BCryptGenRandom(VOID *alg, unsigned char *buf,
				      unsigned long len, unsigned long flags);

static int getrandom_bytes(unsigned char *buf, size_t len)
{
	/* 2 = BCRYPT_USE_SYSTEM_PREFERRED_RNG */
	return(BCryptGenRandom(NULL, buf, (unsigned long)len, 2) >= 0);
}
#else
static int getrandom_bytes(unsigned char *buf, size_t len)
{
	FILE *fp;
	size_t n;

	if ((fp = fopen("/dev/urandom", "rb")) == NULL)
		return(FALSE);
	n = fread(buf, 1, len, fp);
	fclose(fp);
	return(n == len);
}
#endif

/* Make a key from a password with Argon2id */

static int derive(uint8_t key[32], CONST char *pw, CONST uint8_t salt[16],
		  uint32_t blocks, uint32_t passes)
{
	VOID *work;
	crypto_argon2_config config;
	crypto_argon2_inputs inputs;

	work = malloc((size_t)blocks * 1024);
	if (work == NULL) {
		mlforce("[Not enough memory to encrypt]");
		return(FALSE);
	}
	config.algorithm = CRYPTO_ARGON2_ID;
	config.nb_blocks = blocks;
	config.nb_passes = passes;
	config.nb_lanes = 1;
	inputs.pass = (CONST uint8_t *)pw;
	inputs.pass_size = (uint32_t)strlen(pw);
	inputs.salt = salt;
	inputs.salt_size = 16;
	crypto_argon2(key, 32, work, config, inputs, crypto_argon2_no_extras);
	wipe(work, (size_t)blocks * 1024);
	free(work);
	return(TRUE);
}

/* Read a whole file. Returns FIOSUC, FIOFNF, FIOMEM or FIOERR. */

static int slurp(char *fn, unsigned char **bufp, size_t *lenp)
{
	FILE *fp;
	unsigned char *buf, *nb;
	size_t len, size, n;

	if ((fp = fopen(fn, "rb")) == NULL)
		return(FIOFNF);
	size = 4096;
	len = 0;
	if ((buf = (unsigned char *)malloc(size)) == NULL) {
		fclose(fp);
		return(FIOMEM);
	}
	while ((n = fread(buf + len, 1, size - len, fp)) > 0) {
		len += n;
		if (len == size) {
			if ((nb = (unsigned char *)malloc(size * 2)) == NULL) {
				wipe(buf, size);
				free(buf);
				fclose(fp);
				return(FIOMEM);
			}
			memcpy(nb, buf, len);
			wipe(buf, size);
			free(buf);
			buf = nb;
			size *= 2;
		}
	}
	if (ferror(fp)) {
		fclose(fp);
		free(buf);
		mlforce(TEXT158);
/*			"File read error" */
		return(FIOERR);
	}
	fclose(fp);
	*bufp = buf;
	*lenp = len;
	return(FIOSUC);
}

/* What kind of file is this? 0: missing or empty, 1: old format,
   2: new format (salt and settings are filled in). */

#define	KIND_NONE	0
#define	KIND_OLD	1
#define	KIND_NEW	2

static int peek(char *fn, uint8_t salt[16], uint32_t *blocks, uint32_t *passes)
{
	FILE *fp;
	unsigned char hdr[HDRLEN];
	size_t n;

	if ((fp = fopen(fn, "rb")) == NULL)
		return(KIND_NONE);
	n = fread(hdr, 1, HDRLEN, fp);
	fclose(fp);
	if (n == 0)
		return(KIND_NONE);
	if (n < HDRLEN || memcmp(hdr, magic, 8) != 0)
		return(KIND_OLD);
	memcpy(salt, hdr + 20, 16);
	*blocks = get32(hdr + 12);
	*passes = get32(hdr + 16);
	return(KIND_NEW);
}

/* Decrypt a file read into buf. On success the plain text is at the
   start of buf and *lenp is its length. */

static int unlock(BUFFER *bp, unsigned char *buf, size_t *lenp)
{
	CKEY *ck;
	uint8_t key[32];
	uint32_t blocks, passes;
	size_t tlen;
	int frompw;		/* key made from bp->b_key? */
	int r;

	ck = getck(bp, FALSE);
	if (*lenp < HDRLEN + TAGLEN || buf[8] != 1 || buf[9] != KDF_ARGON2ID) {
		mlforce("[Unknown encrypted file format]");
		return(FIOERR);
	}
	blocks = get32(buf + 12);
	passes = get32(buf + 16);
	if (blocks < MIN_BLOCKS || blocks > MAX_BLOCKS ||
	    passes < 1 || passes > MAX_PASSES) {
		mlforce("[Unknown encrypted file format]");
		return(FIOERR);
	}

	/* find the key: the buffer's own if this file used the same salt,
	   otherwise make one from a password */
	frompw = FALSE;
	if (ck && ck->ok && ck->blocks == blocks && ck->passes == passes &&
	    memcmp(ck->salt, buf + 20, 16) == 0)
		memcpy(key, ck->key, 32);
	else {
		CONST char *pw = oppw[0] ? oppw : bp->b_key;

		if (*pw == 0) {
			mlforce("[No encryption key]");
			return(FIOERR);
		}
		mlforce("[Checking encryption key...]");
		if (derive(key, pw, buf + 20, blocks, passes) == FALSE)
			return(FIOMEM);
		frompw = (pw == bp->b_key);
	}

	tlen = *lenp - HDRLEN - TAGLEN;
	r = crypto_aead_unlock(buf + HDRLEN, buf + HDRLEN + tlen, key,
			       buf + 36, buf, HDRLEN, buf + HDRLEN, tlen);
	if (r != 0) {
		wipe(key, 32);
		if (frompw)		/* let the user type it again */
			wipe(bp->b_key, NPAT);
		mlforce("[Wrong encryption key, or the file is damaged]");
		return(FIOERR);
	}

	/* a password that opens a file is a good one: keep its key */
	if (frompw && (ck = getck(bp, TRUE)) != NULL) {
		memcpy(ck->key, key, 32);
		memcpy(ck->salt, buf + 20, 16);
		ck->blocks = blocks;
		ck->passes = passes;
		ck->ok = TRUE;
		wipe(bp->b_key, NPAT);
	}
	wipe(key, 32);
	memmove(buf, buf + HDRLEN, tlen);
	wipe(buf + tlen, *lenp - tlen);
	*lenp = tlen;
	return(FIOSUC);
}

/* Decrypt a file in the old format, in place */

static int unlock_old(BUFFER *bp, unsigned char *buf, size_t *lenp)
{
	char pw[NPAT];
	CKEY *ck;

	strcpy(pw, oppw[0] ? oppw : bp->b_key);
	if (pw[0] == 0) {
		mlforce("[No encryption key]");
		return(FIOERR);
	}

	/* MS-DOS versions put a ^Z at the end */
	if (*lenp > 0 && buf[*lenp - 1] == 26)
		--*lenp;

	ecrypt((char *)NULL, 0);
	ecrypt(pw, (unsigned)strlen(pw));
	ecrypt((char *)buf, (unsigned)*lenp);
	wipe(pw, NPAT);

	if ((ck = getck(bp, TRUE)) != NULL)
		ck->legacy = TRUE;
	return(FIOSUC);
}

/* Read and decrypt a file into mbuf (appending to what is there) */

static int loadplain(char *fn)
{
	unsigned char *buf;
	size_t len;
	int s;

	if ((s = slurp(fn, &buf, &len)) != FIOSUC)
		return(s);
	if (len == 0)
		s = FIOSUC;
	else if (len >= 8 && memcmp(buf, magic, 8) == 0)
		s = unlock(curbp, buf, &len);
	else
		s = unlock_old(curbp, buf, &len);
	if (s == FIOSUC) {
		if (growmbuf(mlen + len) == FALSE)
			s = FIOMEM;
		else {
			memcpy(mbuf + mlen, buf, len);
			mlen += len;
		}
	}
	wipe(buf, len);
	free(buf);
	return(s);
}

/* Ask for a password without showing it */

static int askpw(char *prompt, char *buf)
{
	int s, odisinp;

	odisinp = disinp;
	disinp = FALSE;
	s = mlreply(prompt, buf, NPAT - 1);
	disinp = odisinp;
	mlerase();
	if (s != TRUE)
		wipe(buf, NPAT);
	return(s);
}

/* Ask for a new password twice and store it in the current buffer */

static int asknew(VOID)
{
	char pw[NPAT], pw2[NPAT];
	CKEY *ck;
	int s;

	if ((s = askpw(TEXT33, pw)) != TRUE)
/*			"Encryption String: " */
		return(s);
	if ((s = askpw("Type it again: ", pw2)) != TRUE) {
		wipe(pw, NPAT);
		return(s);
	}
	s = (strcmp(pw, pw2) == 0);
	wipe(pw2, NPAT);
	if (s == FALSE) {
		wipe(pw, NPAT);
		mlforce("[The two keys do not match]");
		return(FALSE);
	}
	if ((ck = getck(curbp, TRUE)) == NULL) {
		wipe(pw, NPAT);
		return(FALSE);
	}
	strcpy(curbp->b_key, pw);
	wipe(pw, NPAT);
	ck->pwok = TRUE;
	ck->ok = FALSE;		/* the next save uses a new salt */
	wipe(ck->key, 32);
	return(TRUE);
}

/* ------------------------------------------------------------------ */
/*	Commands and key setup						*/
/* ------------------------------------------------------------------ */

/* set-encryption-key: give the current buffer a new key */

PASCAL NEAR setekey(f, n)

int f;		/* default flag */
int n;		/* numeric argument */

{
	return(asknew());
}

/* Get ready to read (mode 'r'), insert ('i'), write ('w') or append to ('a') fname
   with the current buffer's key. Asks for a password if one is
   needed. Sets cryptflag if the buffer is in CRYPT mode. */

PASCAL NEAR resetkey(fname, mode)

char *fname;	/* file about to be read or written */
int mode;	/* 'r', 'w' or 'a' */

{
	CKEY *ck;
	int kind;
	uint8_t salt[16];
	uint32_t blocks, passes;
	int s;

	cryptflag = FALSE;
	wipe(oppw, NPAT);

	/* a buffer not in CRYPT mode: look for an encrypted file */
	if ((curbp->b_mode & MDCRYPT) == 0) {
		if (mode == 'w' ||
		    peek(fname, salt, &blocks, &passes) != KIND_NEW)
			return(TRUE);
		if (mode == 'a') {
			/* plain text would spoil it */
			mlforce("[That file is encrypted; turn on CRYPT mode to add to it]");
			return(FALSE);
		}
		if (mode == 'i') {
			/* decrypt it, but leave this buffer plain */
			if ((s = askpw("Encryption key for this file: ",
				       oppw)) != TRUE)
				return(s);
			cryptflag = TRUE;
			return(TRUE);
		}
		/* reading into a buffer: it becomes an encrypted one */
		curbp->b_mode |= MDCRYPT;
		upmode();
	}
	ck = getck(curbp, FALSE);

	/* reading, or adding to what is there */
	if (mode != 'w') {
		kind = peek(fname, salt, &blocks, &passes);
		if (curbp->b_key[0] == 0 &&
		    (kind == KIND_OLD || (kind == KIND_NEW &&
		     !(ck && ck->ok && ck->blocks == blocks &&
		       ck->passes == passes &&
		       memcmp(ck->salt, salt, 16) == 0)))) {
			if (ck && ck->ok) {
				/* keep the buffer's key; this file has
				   its own */
				s = askpw("Encryption key for this file: ",
					  oppw);
			} else
				s = askpw(TEXT33, curbp->b_key);
/*					"Encryption String: " */
			if (s != TRUE)
				return(s);
		}
		if (kind == KIND_NONE && mode == 'r' &&	/* not 'i' */
		    curbp->b_key[0] == 0 && !(ck && ck->ok)) {
			/* a new file: ask now, like older versions */
			if ((s = askpw(TEXT33, curbp->b_key)) != TRUE)
/*					      "Encryption String: " */
				return(s);
		}
	}

	/* writing: make sure we have a key we trust */
	if (mode != 'r') {
		ck = getck(curbp, FALSE);
		if (!(ck && ck->ok)) {
			if (curbp->b_key[0] == 0) {
				if ((s = asknew()) != TRUE)
					return(s);
			} else if (!(ck && ck->pwok)) {
				char pw2[NPAT];

				if ((s = askpw("Type the encryption key again: ",
					       pw2)) != TRUE)
					return(s);
				s = (strcmp(pw2, curbp->b_key) == 0);
				wipe(pw2, NPAT);
				if (s == FALSE) {
					mlforce("[The keys do not match]");
					return(FALSE);
				}
				if ((ck = getck(curbp, TRUE)) == NULL)
					return(FALSE);
				ck->pwok = TRUE;
			}
		}
	}

	cryptflag = TRUE;
	return(TRUE);
}

/* Forget a buffer's keys (it is being deleted) */

VOID PASCAL NEAR cr_freekey(bp)

BUFFER *bp;

{
	wipe(bp->b_key, NPAT);
	if (bp->b_ckey != NULL) {
		wipe(bp->b_ckey, sizeof(CKEY));
		free(bp->b_ckey);
		bp->b_ckey = NULL;
	}
}

/* Did the last save turn an old-format file into the new format? */

int PASCAL NEAR cr_upgraded()

{
	int u;

	u = upgraded;
	upgraded = FALSE;
	return(u);
}

/* ------------------------------------------------------------------ */
/*	File access (called from fileio.c when cryptflag is set)	*/
/* ------------------------------------------------------------------ */

/* Open an encrypted file for reading */

int PASCAL NEAR cr_ropen(fn)

char *fn;

{
	int s;

	freembuf();
	s = loadplain(fn);
	if (s != FIOSUC) {
		freembuf();
		wipe(oppw, NPAT);
		cryptflag = FALSE;
		return(s == FIOMEM ? FIOERR : s);
	}
	cr_active = CR_READ;
	return(FIOSUC);
}

/* Next character of the decrypted file, or EOF */

int PASCAL NEAR cr_getc()

{
	if (mpos >= mlen)
		return(EOF);
	return(mbuf[mpos++]);
}

/* Open an encrypted file for writing. In append mode, the file's
   current text is decrypted first, and everything is written back. */

int PASCAL NEAR cr_wopen(fn, mode)

char *fn;
char *mode;

{
	CKEY *ck;
	int s;

	freembuf();

	/* first save with this password: make a key, before the file
	   is opened and emptied */
	if ((ck = getck(curbp, TRUE)) == NULL)
		goto fail;
	if (!ck->ok) {
		if (curbp->b_key[0] == 0) {
			mlforce("[No encryption key]");
			goto fail;
		}
		if (getrandom_bytes(ck->salt, 16) == FALSE) {
			mlforce("[Cannot get random numbers]");
			goto fail;
		}
		mlforce("[Encrypting...]");
		ck->blocks = DEF_BLOCKS;
		ck->passes = DEF_PASSES;
		if (derive(ck->key, curbp->b_key, ck->salt,
			   ck->blocks, ck->passes) == FALSE)
			goto fail;
		ck->ok = TRUE;
		wipe(curbp->b_key, NPAT);
	}

	/* a new nonce for every save */
	if (getrandom_bytes(wnonce, 24) == FALSE) {
		mlforce("[Cannot get random numbers]");
		goto fail;
	}

	if (*mode == 'a') {
		s = loadplain(fn);
		if (s != FIOSUC && s != FIOFNF) {
			freembuf();
			goto fail;
		}
	}
	if ((ffp = fopen(fn, "wb")) == NULL) {
		freembuf();
		mlforce(TEXT155);
/*			"Cannot open file for writing" */
		goto fail;
	}
	cr_active = CR_WRITE;
	return(FIOSUC);

fail:
	wipe(oppw, NPAT);
	cryptflag = FALSE;
	return(FIOERR);
}

/* Add a character to the text being written */

int PASCAL NEAR cr_putc(c)

int c;

{
	if (mlen >= msize && growmbuf(mlen + 1) == FALSE)
		return(EOF);
	mbuf[mlen++] = (unsigned char)c;
	return(c);
}

/* Finish reading or writing. Writing encrypts the text and saves it. */

int PASCAL NEAR cr_close()

{
	CKEY *ck;
	unsigned char hdr[HDRLEN];
	unsigned char tag[TAGLEN];
	int s;

	s = FIOSUC;
	if (cr_active == CR_WRITE) {
		ck = getck(curbp, FALSE);	/* made by cr_wopen() */

		memset(hdr, 0, HDRLEN);
		memcpy(hdr, magic, 8);
		hdr[8] = 1;
		hdr[9] = KDF_ARGON2ID;
		put32(hdr + 12, ck->blocks);
		put32(hdr + 16, ck->passes);
		memcpy(hdr + 20, ck->salt, 16);
		memcpy(hdr + 36, wnonce, 24);

		if (mbuf == NULL && growmbuf(1) == FALSE) {	/* empty file */
			s = FIOMEM;
			goto close;
		}
		crypto_aead_lock(mbuf, tag, ck->key, hdr + 36, hdr, HDRLEN,
				 mbuf, mlen);
		if (fwrite(hdr, 1, HDRLEN, ffp) != HDRLEN ||
		    (mlen > 0 && fwrite(mbuf, 1, mlen, ffp) != mlen) ||
		    fwrite(tag, 1, TAGLEN, ffp) != TAGLEN) {
			mlforce(TEXT157);
/*				"Write I/O error" */
			s = FIOERR;
		}
		if (s == FIOSUC && ck->legacy) {
			ck->legacy = FALSE;
			upgraded = TRUE;
		}
	}

close:
	if (cr_active == CR_WRITE && fclose(ffp) != 0 && s == FIOSUC) {
		mlforce(TEXT156);
/*			"Error closing file" */
		s = FIOERR;
	}
	ffp = NULL;
	freembuf();
	wipe(oppw, NPAT);
	cr_active = CR_NONE;
	cryptflag = FALSE;
	return(s);
}

#else
noecrypt2()
{
}
#endif
