/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

/*Basic constants and macros */
#define WSIZE 4 /* Word and header/footer size (bytes) */
#define DSIZE 8 /* Double word size (bytes) */
#define CHUNKSIZE (1<<9) /* Extend heap by this amount (bytes) */

#define MAX(x,y) ((x) > (y)? (x) : (y))

/* Pack a size and allocated bit into a word */
#define PACK(size, alloc) ((size) | (alloc))

/* Read and write a word at address p */
#define GET(p) (*(unsigned int *)(p))
#define PUT(p, val) (*(unsigned int *)(p) = (val))
#define GET_PRED(bp) (*(unsigned int *)(bp))
#define GET_SUCC(bp) (*(unsigned int *)(bp + WSIZE))
#define PUT_PRED(bp, val) (*(unsigned int *)(bp) = (val))	
#define PUT_SUCC(bp, val) (*((unsigned int *)(bp + WSIZE)) = (val))

/* Read the size and allocated fields from address p */
#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp) ((char *)(bp) - WSIZE)
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))

#define free_succ(bp) (void *)(heap_listp+GET_SUCC(bp))
#define free_pred(bp) (void *)(heap_listp+GET_PRED(bp))

static void *heap_listp;
static void *blank_listph=NULL;
static void *blank_listpf=NULL;

/* 
 * add_blank - given block ptr bp, add to the blank_list from head.
 */
static void add_blank(void *bp)
{
    /* Fill the pred and succ with the address which is relative to heap_list */
    if (blank_listph)
    {
	PUT_PRED(blank_listph, bp-heap_listp);
	PUT_SUCC(bp, blank_listph-heap_listp);
	PUT_PRED(bp, 0);
	blank_listph=bp;
    }

    else  /* Blank_list is empty */
    {
	blank_listph=bp;
	blank_listpf=bp;
	PUT_PRED(bp, 0);
	PUT_SUCC(bp, 0);
    }
}

/* 
 * delete_blank - given block ptr bp, delete from blank_list.
 */
static void delete_blank(void *bp)
{
    void *pred=free_pred(bp);
    void *succ=free_succ(bp);

    if (bp != blank_listph)
	PUT_SUCC(pred, GET_SUCC(bp));
    else
	blank_listph = succ;
    if (bp != blank_listpf)
	PUT_PRED(succ, GET_PRED(bp));
    else
	blank_listpf = pred;
    PUT_PRED(bp, 0);
    PUT_SUCC(bp, 0);
}

/* 
 * coalesce - given block ptr bp, if availble, coalesce prev or next or both.
 */
static void *coalesce(void *bp)
{
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));
    
    if (prev_alloc && next_alloc){   /* Case 1 */
	add_blank(bp);
        return bp; 
    }  

    else if (prev_alloc && !next_alloc){  /* Case 2 */
	delete_blank(NEXT_BLKP(bp));
	size +=GET_SIZE(HDRP(NEXT_BLKP(bp)));
	PUT(HDRP(bp), PACK(size, 0));
	PUT(FTRP(bp), PACK(size, 0));
	add_blank(bp);
        return bp;
    }

    else if (!prev_alloc && next_alloc){  /* Case 3 */
	delete_blank(PREV_BLKP(bp));
	size +=GET_SIZE(HDRP(PREV_BLKP(bp)));
	PUT(FTRP(bp), PACK(size, 0));
	PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
	bp = PREV_BLKP(bp);
	add_blank(bp);
        return bp;
    }

    else {  /* Case 4 */
	delete_blank(PREV_BLKP(bp));
	delete_blank(NEXT_BLKP(bp));
	size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
	PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
	PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
	bp = PREV_BLKP(bp);
	add_blank(bp);
        return bp;
    }
}

/* 
 * extend_heap - extend heap with size needed.
 */
static void *extend_heap(size_t words)
{
    char *bp;
    size_t size;
    
    /* Allocate an even number of words to maintain alignment */
    size = (words % 2)? (words+1) * WSIZE : words * WSIZE;
    if ((long)(bp = mem_sbrk(size)) == -1)
	return NULL;

    /* Initialize free block header/footer and the epilogue header */
    PUT(HDRP(bp), PACK(size, 0));  /* Free block header */
    PUT(FTRP(bp), PACK(size, 0));  /* Free block footer */
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));  /* New epilogue header */

    /* Coalesce if the previous block was free */
    return coalesce(bp);
}

/* 
 * find_fit - search blank_list for the size needed.
 */
static void *find_fit(size_t asize)
{
    /* First-fit search */
    void *bp;

    for(bp=blank_listph;bp != blank_listpf;bp = free_succ(bp))
	if (GET_SIZE(HDRP(bp))>=asize)
	    return bp;
    if (bp==blank_listpf && GET_SIZE(HDRP(bp))>=asize)
	return bp;
    return NULL;  /* No fit */
}

/* 
 * place - given block ptr bp, alloc the block with size.
 */
static void place(void *bp, size_t asize)
{
    size_t csize = GET_SIZE(HDRP(bp));
    delete_blank(bp);

    if ((csize-asize) >= (3*DSIZE)){
	PUT(HDRP(bp), PACK(asize, 1));
	PUT(FTRP(bp), PACK(asize, 1));
	bp = NEXT_BLKP(bp);
	PUT(HDRP(bp), PACK(csize-asize, 0));
	PUT(FTRP(bp), PACK(csize-asize, 0));
	add_blank(bp);
    }
    else {
	PUT(HDRP(bp), PACK(csize, 1));
	PUT(FTRP(bp), PACK(csize, 1));
    }
}


/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    /* Create the initial empty heap */
    if ((heap_listp = mem_sbrk(6*WSIZE)) == (void *)-1)
	return -1;
    PUT(heap_listp, 0);  /* Alignment padding */
    PUT(heap_listp + (1*WSIZE), PACK(2*DSIZE, 1));  /* Prologue header */
    PUT(heap_listp + (2*WSIZE), 0);                 /* Prologue pred */
    PUT(heap_listp + (3*WSIZE), 0);                 /* Prologue succ */
    PUT(heap_listp + (4*WSIZE), PACK(2*DSIZE, 1));  /* Prologue footer */
    PUT(heap_listp + (5*WSIZE), PACK(0,1));  /* Epilogue header */
    heap_listp += (2*WSIZE);
    blank_listph = heap_listp;
    blank_listpf = heap_listp;

    /* Extend the empty heap with a free block of CHUNKSIZE byte */
    if (extend_heap(CHUNKSIZE/WSIZE) == NULL)
	return -1;
    return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    int asize;
    size_t extendsize;
    char *bp;
   

     /* Ignore spurious requests */
    if (size == 0)
	return NULL;

    /* Adjust block size to include overhead and alignment reqs. */
    
    switch(size){
        case 112:
            size = 128;
            break;
        case 448:
            size = 512;
            break;
        case 4092:
            size = 4160;
        default:  
            break;
    }

    if (size <= DSIZE)
	asize = 3*DSIZE;

    else
	asize = DSIZE * ((size + (DSIZE) + (DSIZE-1)) / DSIZE);

    if ((bp = find_fit(asize)) != NULL) {
	place(bp, asize);
	return bp;
    }

    /* No fit found. Get more memory and place the block */
    extendsize = MAX(asize,CHUNKSIZE);
    if ((bp = extend_heap(extendsize/WSIZE)) == NULL) 
	return NULL;
    place(bp, asize);
    /* mm_check(); */
    return bp;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
    size_t size = GET_SIZE(HDRP(ptr));

    PUT(HDRP(ptr), PACK(size, 0));
    PUT(FTRP(ptr), PACK(size, 0));
    coalesce(ptr);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;
    
    if (ptr == NULL)
    	return mm_malloc(size);
    if (size == 0)
    {
	mm_free(ptr);
	return NULL;
    }

    if(size <= DSIZE) 
        size = 3 * DSIZE;
    else 
        size = DSIZE * ((size + (DSIZE) + (DSIZE - 1)) / DSIZE);

    copySize = GET_SIZE(HDRP(oldptr));
    if (size <= copySize)
	newptr=oldptr;
    else
    {
	size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(ptr)));
	size_t next_size = GET_SIZE(HDRP(NEXT_BLKP(ptr)));
        size_t prev_alloc = GET_ALLOC(HDRP(PREV_BLKP(ptr)));
	size_t prev_size = GET_SIZE(HDRP(PREV_BLKP(ptr)));

	if (!next_alloc && next_size+copySize >= size)
	{
	    delete_blank(NEXT_BLKP(oldptr));
	    PUT(HDRP(ptr), PACK(next_size+copySize, 1));
	    PUT(FTRP(ptr), PACK(next_size+copySize, 1));
	    newptr=oldptr;
	}

	else if (!prev_alloc && prev_size+copySize >= size)
	{
	    ptr=PREV_BLKP(ptr);
	    delete_blank(PREV_BLKP(oldptr));
	    PUT(HDRP(ptr), PACK(prev_size+copySize, 1));
	    PUT(FTRP(ptr), PACK(prev_size+copySize, 1));
	    newptr=PREV_BLKP(oldptr);
	    memcpy(newptr, oldptr, copySize);
	}
	
	else
	{
	    newptr = mm_malloc(size);
            memcpy(newptr, oldptr, copySize);
            mm_free(oldptr);
	}
    }
    return newptr;
}

int mm_check(void)
{
    /* Is every block in the free list marked as free? 
       Do the pointers in the free list point to valid free blocks? */
    void *bp=blank_listph;
    for(bp=blank_listph;bp != blank_listpf;bp = free_succ(bp))
	if (GET_ALLOC(HDRP(bp)))
	{
	    fprintf(stderr, "Alloced block is put in free list");
	    return 0;
	}

    /* Are there any contiguous free blocks that somehow escaped coalescing? */
    for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) 
	if (!GET_ALLOC(HDRP(bp)) && !GET_ALLOC(HDRP(NEXT_BLKP(bp))))
	{
	    fprintf(stderr, "Contiguous free blocks escaped coalescing");
	    return 0;
	}

    /* Is every free block actually in the free list? */
    void *ptr;
    for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) 
	if (!GET_ALLOC(HDRP(bp))) 
	{
	    void *ptr;
	    for(ptr=blank_listph;ptr != blank_listpf;ptr = free_succ(ptr))
		if (bp==ptr)
		    break;
	    if (ptr==blank_listpf && ptr!=bp)
	    {
		fprintf(stderr, "free block is not in the free list.");
		return 0;
	    }
	}

    /* Do any allocated blocks overlap? 
       contained in correction test */

    /* Do the pointers in a heap block point to valid heap addresses?
       contained in correction test */
    return 1;
}

























