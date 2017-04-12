/* The routines in this file provide memory allocation under the
   Microsoft Windows environment on an IBM-PC or compatible computer.

   Must be compiled with Borland C++ 2.0 or later version.

   It should not be compiled if the WINDOW_MSWIN symbol is not set */

/* this module allocates memory by performing subsegment allocation from
   a list of global segments. This technique is inspired by an article
   by Paul Yao in Microsoft Systems Journal (January 1991) */

#include    "estruct.h"
#include    <stdio.h>
#include    "eproto.h"
#include    "edef.h"
#include    "elang.h"

#if	WINDOW_MSWIN

#include    "mswin.h"

#if SUBALLOC
#ifdef  malloc
#undef  malloc
#endif
#ifdef  free
#undef  free
#endif
#ifdef  realloc
#undef  realloc
#endif

#define SEGHEADOFFSET 16    /* offset of the segment header (the first
			       16 bytes of a heap segment are reserved
			       for windows) */
#define HEAPOFFSET  (SEGHEADOFFSET +  sizeof(SEGHEADER))
#define HEAPOVH     0x400      /* estimated heap overhead */
#define MINSEGSIZE  0x1000L /* minimum segment allocation: 4K */

/* Data segment switching macroes. The wS argument MUST be a stack
   variable */
#define SWITCH_DS(wS) asm {push ds; mov ax,wS; mov ds,ax;}
#define RESTORE_DS    asm {pop ds;}

typedef struct  SegHeader {     /* segment header */
    struct SegHeader *next;         /* link to next segment header */
    int     alloc_count;            /* number of allocated blocks */
} SEGHEADER;

static SEGHEADER SegList = {NULL, -1};/* list of segments (dummy header) */
static SEGHEADER *OptimumSeg = NULL;

static BOOL FarStorage = FALSE; /* validates the use of suballocation */

#if MEMTRACE
static  unsigned char mtrx = 0;
static  struct mtr_tag {
    WORD    m_func;         /* ASCII signature of function */
    union   {
        WORD    m_wseg;     /* segment selector */
        void    *m_block;   /* block address */
        SEGHEADER *m_segh;  /* segment header */
    } m_ptr;
    WORD    m_cnt;          /* alloc count */
    DWORD   m_size;         /* size of segment or block */
    SEGHEADER *m_optimseg;  /* value of OptimumSeg */
} mtr [256];

/* m_func values ... */
#define MTR_SUBALLOC    'SU'
#define MTR_MALLOC      'MA'
#define MTR_MALLOCSEG   'MS'
#define MTR_FREE        'FR'
#define MTR_FREESEG     'FS'
#define MTR_REALLOC     'RA'
#endif

/* SubAlloc:    performs suballocation from a global segment */
/* ========                                                  */

void *SubAlloc(WORD wSeg, unsigned size)

/* wSeg is the global segment's selector */
{
    HANDLE  hBlock;     /* better be a stack variable */
    void    *Block;     /* better be a stack variable */
#if MEMTRACE
    WORD    RealSize = 0;/* better be a stack variable */
#endif
    SWITCH_DS(wSeg)
    hBlock = LocalAlloc (LMEM_FIXED | LMEM_NOCOMPACT, size);
    /* no point attempting compaction: everything is FIXED
       in this heap! */
    if (hBlock) {
	Block = (void*)(LPSTR)LocalLock (hBlock);
#if MEMTRACE
	RealSize = LocalSize (hBlock);
#endif
    }
    RESTORE_DS
#if MEMTRACE
    mtr[mtrx].m_func = MTR_SUBALLOC;
    mtr[mtrx].m_ptr.m_block = hBlock ? Block : NULL;
    mtr[mtrx].m_size = RealSize;
    mtr[mtrx++].m_optimseg = OptimumSeg;
#endif
    if (hBlock) return Block;       /* success ! */
    else return NULL;               /* failure!! */
} /* SubAlloc */

/* malloc:  allocates a chunk of memory */
/* ======                               */

void * CDECL malloc(size_t size)
{
    SEGHEADER *Seg;         /* segment header pointer */
    WORD    wSeg;           /* segment's selector, must be a stack variable */
    BOOL    NewSeg = FALSE; /* TRUE: a new segment has been allocated */
    void    *Block;         /* obtained block's address */

#if MEMTRACE
    mtr[mtrx].m_func = MTR_MALLOC;
    mtr[mtrx].m_ptr.m_block = NULL;
    mtr[mtrx].m_size = size;
    mtr[mtrx++].m_optimseg = OptimumSeg;
#endif

    if (size == 0) return NULL;

    if (FarStorage == FALSE) {  /* use the local heap */
	HANDLE hMem;

	if ((hMem = LocalAlloc (LMEM_FIXED, size)) != NULL) {
	    return (LPSTR)LocalLock (hMem);
	}
	else return NULL;
    }

    /*-Attempt to suballocate from last used segment */
    if (OptimumSeg) {
	wSeg = HIWORD(OptimumSeg);
#if MEMTRACE
        mtr[mtrx].m_cnt = OptimumSeg->alloc_count;
#endif
	Block = SubAlloc (wSeg, size);
	if (Block != NULL) {
            ++(OptimumSeg->alloc_count);
	    return (char*)Block;     /* quick success ! */
	}
    }

    /*-Scan segment list, attempting to find one where suballocation is
       possible */
    Seg = &SegList;
    for (;;) {
	if (Seg->next == NULL) {
	    /*-initialize a new Segment and chain it on tail */
	    HANDLE      hSeg;
	    DWORD       SegSize;

	    SegSize = max (MINSEGSIZE, HEAPOVH + SEGHEADOFFSET +
				       sizeof(SEGHEADER) + (DWORD)size);
	    hSeg = GlobalAlloc (GMEM_MOVEABLE, SegSize);
	    if (hSeg == NULL) return (char*)NULL;  /* segment allocation
						      failure */
	    SegSize = GlobalSize (hSeg);
	    wSeg = HIWORD(GlobalLock (hSeg));
	    LocalInit (wSeg, 0, SegSize - HEAPOFFSET);
	    GlobalUnlock (hSeg);
	    /* the segment remains locked once, due to LocalInit */

	    SWITCH_DS(wSeg)
	    LockData (0);   /* this ensures the segment's selector will
			       never change */
	    /* note that allocating a GMEM_FIXED segment would have been
	       cleaner but, in Windows 3.0, such segments also end up
	       being page-locked */
	    RESTORE_DS

	    Seg->next = (SEGHEADER*)MAKELONG(SEGHEADOFFSET,wSeg);
	    Seg = Seg->next;
	    Seg->next = NULL;
	    Seg->alloc_count = 0;
	    NewSeg = TRUE;
#if MEMTRACE
    mtr[mtrx].m_func = MTR_MALLOCSEG;
    mtr[mtrx].m_ptr.m_segh = Seg;
    mtr[mtrx].m_cnt = Seg->alloc_count;
    mtr[mtrx].m_size = SegSize;
    mtr[mtrx++].m_optimseg = OptimumSeg;
#endif
	}
	else {
	    Seg = Seg->next;
	    wSeg = HIWORD(Seg);
	}
        
	if (Seg == OptimumSeg) continue;    /* skip this already tried
					       one */
	/*-try to allocate space in that segment */
#if MEMTRACE
        mtr[mtrx].m_cnt = Seg->alloc_count;
#endif
	Block = SubAlloc (wSeg, size);
	if (Block != NULL) ++(Seg->alloc_count);
	if ((Block != NULL) || NewSeg) {
	    OptimumSeg = Seg;   /* next malloc will try this segment
				   first */
	    return (char*)Block;
	}
    }
} /* malloc */

/* free:    frees an allocated block */
/* =====                              */

void CDECL free (void *block)
{
    HANDLE  hSeg;
    WORD    wSeg;
    SEGHEADER *Seg;
    HANDLE  hBlock;
#if MEMTRACE
    WORD    RealSize = 0;
#endif

    if (FarStorage == FALSE) {  /* use the local heap */
	HANDLE hMem;

	hMem = LocalHandle (LOWORD(block));
	LocalUnlock (hMem);
	LocalFree (hMem);
	return;
    }

    wSeg = HIWORD((DWORD)block);
    hSeg = LOWORD(GlobalHandle (wSeg));

    SWITCH_DS(wSeg)
    LocalUnlock (hBlock = LocalHandle (LOWORD(block)));
#if MEMTRACE
    RealSize = LocalSize (hBlock);
#endif
    LocalFree (hBlock);
    RESTORE_DS

    Seg = (SEGHEADER *)(MAKELONG(SEGHEADOFFSET,HIWORD(block)));
#if MEMTRACE
    mtr[mtrx].m_func = MTR_FREE;
    mtr[mtrx].m_ptr.m_block = block;
    mtr[mtrx].m_cnt = Seg->alloc_count;
    mtr[mtrx].m_size = RealSize;
    mtr[mtrx++].m_optimseg = OptimumSeg;
#endif
    if (--(Seg->alloc_count) == 0) {  /* this segment is no longer used!
					 Let's get rid of it */
        SEGHEADER   *sp;

        if (Seg == OptimumSeg) OptimumSeg = NULL;
        sp = &SegList;
        while (sp->next != Seg) {
            sp = sp->next;
            if (sp == NULL) {   /* this segment is not in the list!!! */
                /* this should not happen, but you never know... */
                static BOOL WarningDisplayed = FALSE;

                if (!WarningDisplayed) {
		    MessageBox (hFrameWnd,
                                "Please shutdown EMACS as soon as possible",
                                "Corrupted memory",
                                MB_OK | MB_ICONSTOP);
                }
                WarningDisplayed = TRUE;
                return;
            }
        }
        sp->next = Seg->next;       /* unlink the segment */
#if MEMTRACE
        mtr[mtrx].m_func = MTR_FREESEG;
        mtr[mtrx].m_ptr.m_segh = Seg;
        mtr[mtrx].m_cnt = Seg->alloc_count;
        mtr[mtrx].m_size = GlobalSize (hSeg);
        mtr[mtrx++].m_optimseg = OptimumSeg;
#endif

        SWITCH_DS(wSeg)
	UnlockData (0);
	RESTORE_DS
	
        GlobalUnlock (hSeg);
        GlobalFree (hSeg);          /* and release it */
    }
    else {  /* segment still in use */
	OptimumSeg = Seg;       /* next malloc will try this segment
				   first */
    }
} /* free */

/* realloc: reallocates a chunk of memory (shrink or expand) */
/* ========                                                   */

void * CDECL realloc(void * oldblock, size_t size);
{
    HANDLE  hBlock, hOldBlock;
    void    *Block;
    WORD    wSeg;
    int     OldSize;
#if MEMTRACE
    WORD    RealSize = 0;
#endif

    if (oldblock == NULL) return malloc (size);
    
    if (FarStorage == FALSE) {  /* use the local heap */
	HANDLE hMem;

	hMem = LocalHandle (LOWORD(oldblock));
	LocalUnlock (hMem);
	if ((hMem = LocalReAlloc (hMem, LMEM_MOVEABLE, size)) != NULL) {
	    return (LPSTR)LocalLock (hMem);
	}
	else return NULL;
    }

    wSeg = HIWORD(oldblock);

    SWITCH_DS(wSeg)
    LocalUnlock (hOldBlock = LocalHandle (LOWORD(oldblock)));
    OldSize = LocalSize (hOldBlock);
    hBlock = LocalReAlloc (hOldBlock, size, LMEM_MOVEABLE | LMEM_NOCOMPACT);
    if (hBlock) {
	Block = (LPSTR)LocalLock (hBlock);
#if MEMTRACE
	RealSize = LocalSize (hBlock);
#endif
    }
    else oldblock = LocalLock (hOldBlock); 
    RESTORE_DS
#if MEMTRACE
    mtr[mtrx].m_func = MTR_REALLOC;
    mtr[mtrx].m_ptr.m_block = hBlock ? Block : NULL;
    mtr[mtrx].m_cnt = ((SEGHEADER *)(MAKELONG(SEGHEADOFFSET,wSeg)))->alloc_count;
    mtr[mtrx].m_size = RealSize;
    mtr[mtrx++].m_optimseg = OptimumSeg;
#endif
    
    if (hBlock) return Block;
    else {  /* we have to malloc, possibly into another segment, and
	       copy the data over */
        if ((Block = malloc (size)) != NULL) {
            _fmemcpy (Block, oldblock, min(size, OldSize));
	    free (oldblock);
        }
        return Block;
    }
} /* realloc */
#endif  /* SUBALLOC */

/* InitializeFarStorage:    start the suballocation mechanism */
/* ====================                                       */

void FAR PASCAL InitializeFarStorage (void)
{
#if SUBALLOC
    FarStorage = TRUE;
#endif
} /* InitializeFarStorage */

/* JettisonFarStorage: Release all the global segments (quitting time) */
/* ==================                                                  */

void FAR PASCAL JettisonFarStorage (void)
{
#if SUBALLOC
    SEGHEADER   *sp;
    HANDLE      hSeg;

    sp = SegList.next;
    while (sp != NULL) {
	hSeg = LOWORD(GlobalHandle (HIWORD((DWORD)sp)));
	sp = sp->next;
        GlobalUnlock (hSeg);
        GlobalFree (hSeg);
    }
    FarStorage = FALSE;
#endif
} /* JettisonFarStorage */

#endif	/* WINDOW_MSWIN */

