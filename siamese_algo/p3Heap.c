////////////////////////////////////////////////////////////////////////////////
// Main File:        p3Heap.c
// This File:        p3Heap.c
// Other Files:      p3Heap.h p3Heap.o README Makefile
// Semester:         CS 354 Lecture 001      SPRING 2024
// Grade Group:      gg11  (See canvas.wisc.edu/groups for your gg#)
// Instructor:       deppeler
// 
// Author:           Ming Wang
// Email:            mwang583@wisc.edu
// CS Login:         mingw
//
/////////////////////////// SEARCH LOG //////////////////////////////// 
// Online sources: do not rely web searches to solve your problems, 
// but if you do search for help on a topic, include Query and URLs here.
// IF YOU FIND CODED SOLUTIONS, IT IS ACADEMIC MISCONDUCT TO USE THEM
//                               (even for "just for reference")
// Date:   Query:                      URL:
// --------------------------------------------------------------------- 
// 03/05	get the p-bit and a-bit		https://stackoverflow.com/questions/22004798/how-to-access-a-specific-bit-given-a-byte-in-c
// 03/06	how to cast from void*		https://stackoverflow.com/questions/16986214/why-type-cast-a-void-pointer
// 
// 
// 
// AI chats: save a transcript.  No need to submit, just have available 
// if asked.
/////////////////////////// COMMIT LOG  ////////////////////////////// 
//  Date and label your work sessions, include date and brief description
//  Date:   Commit Message: 
//  -------------------------------------------------------------------
// 03/02    Completed acitivity and got needed file for this program
// 03/05    started balloc() 
// 03/06	debugged prgram using gdb command
// 03/07	check everthing is working and complete the file header
// 03/14	started the bfree(), complete teh free step
// 03/15	started the coalescing steps, coalescing the back block and front blcok
// 03/15	gdb process find the problem in coalscing front block
///////////////////////// OTHER SOURCES OF HELP ////////////////////////////// 
// Persons:          Identify persons by name, relationship to you, and email.
//                   Describe in detail the the ideas and help they provided.
// Date:   Name (email):   Helped with: (brief description)
// ---------------------------------------------------------------------------
// 
//////////////////////////// 80 columns wide ///////////////////////////////////// PLACE YOUR FILE HEADER HERE

/////////////////////////////////////////////////////////////////////////////
//
// Copyright 2020-2024 Deb Deppeler based on work by Jim Skrentny
// Posting or sharing this file is prohibited, including any changes.
// Used by permission SPRING 2024, CS354-deppeler
//
/////////////////////////////////////////////////////////////////////////////

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include "p3Heap.h"

/*
 * This structure serves as the header for each allocated and free block.
 * It also serves as the footer for each free block.
 */
typedef struct blockHeader {           

    /*
     * 1) The size of each heap block must be a multiple of 8
     * 2) heap blocks have blockHeaders that contain size and status bits
     * 3) free heap block contain a footer, but we can use the blockHeader 
     *.
     * All heap blocks have a blockHeader with size and status
     * Free heap blocks have a blockHeader as its footer with size only
     *
     * Status is stored using the two least significant bits.
     *   Bit0 => least significant bit, last bit
     *   Bit0 == 0 => free block
     *   Bit0 == 1 => allocated block
     *
     *   Bit1 => second last bit 
     *   Bit1 == 0 => previous block is free
     *   Bit1 == 1 => previous block is allocated
     * 
     * Start Heap: 
     *  The blockHeader for the first block of the heap is after skip 4 bytes.
     *  This ensures alignment requirements can be met.
     * 
     * End Mark: 
     *  The end of the available memory is indicated using a size_status of 1.
     * 
     * Examples:
     * 
     * 1. Allocated block of size 24 bytes:
     *    Allocated Block Header:
     *      If the previous block is free      p-bit=0 size_status would be 25
     *      If the previous block is allocated p-bit=1 size_status would be 27
     * 
     * 2. Free block of size 24 bytes:
     *    Free Block Header:
     *      If the previous block is free      p-bit=0 size_status would be 24
     *      If the previous block is allocated p-bit=1 size_status would be 26
     *    Free Block Footer:
     *      size_status should be 24
     */
    int size_status;

} blockHeader;         

/* Global variable - DO NOT CHANGE NAME or TYPE. 
 * It must point to the first block in the heap and is set by init_heap()
 * i.e., the block at the lowest address.
 */
blockHeader *heap_start = NULL;     

/* Size of heap allocation padded to round to nearest page size.
 */
int alloc_size;

/*
 * Additional global variables may be added as needed below
 * TODO: add global variables needed by your function
 */
#define GET_BLOCK_SIZE 0xFFFFFFF8
#define AND_ONE 0x00000001 // check status, free or allocate
#define CLEAN_P_BIT 0x1111101
#define GET_P_BIT 0x00000002


/* 
 * Function for allocating 'size' bytes of heap memory.
 * Argument size: requested size for the payload
 * Returns address of allocated block (payload) on success.
 * Returns NULL on failure.
 *
 * This function must:
 * - Check size - Return NULL if size < 1 
 * - Determine block size rounding up to a multiple of 8 
 *   and possibly adding padding as a result.
 *
 * - Use BEST-FIT PLACEMENT POLICY to chose a free block
 *
 * - If the BEST-FIT block that is found is exact size match
 *   - 1. Update all heap blocks as needed for any affected blocks
 *   - 2. Return the address of the allocated block payload
 *
 * - If the BEST-FIT block that is found is large enough to split 
 *   - 1. SPLIT the free block into two valid heap blocks:
 *         1. an allocated block
 *         2. a free block
 *         NOTE: both blocks must meet heap block requirements 
 *       - Update all heap block header(s) and footer(s) 
 *              as needed for any affected blocks.
 *   - 2. Return the address of the allocated block payload
 *
 *   Return if NULL unable to find and allocate block for required size
 *
 * Note: payload address that is returned is NOT the address of the
 *       block header.  It is the address of the start of the 
 *       available memory for the requesterr.
 *
 * Tips: Be careful with pointer arithmetic and scale factors.
 */
void* balloc(int size) {     
    //TODO: Your code goes in here.
	
    if (size < 1 || size > alloc_size){
		return NULL;
	}
//	int somesize = sizeof(blockHeader) + size + 7;
//	int strBlockSize = somesize - somesize % 8; 
	
	int strBlockSize = size + sizeof(blockHeader);
	int paddingSize = (8 -(strBlockSize%8))%8;
	strBlockSize += paddingSize;

    blockHeader *best_fit_block = NULL;
    blockHeader *current_block = heap_start;
    int best_fit_block_size = alloc_size + 1;
    int current_block_size = current_block->size_status & GET_BLOCK_SIZE;
    
    while (current_block->size_status !=1){
        current_block_size = current_block->size_status & GET_BLOCK_SIZE;
        if (current_block_size < best_fit_block_size 
                && strBlockSize <= current_block_size 
                    && ((current_block->size_status & AND_ONE) == 0)){
            best_fit_block = current_block; 
            best_fit_block_size = current_block_size; //found the best_fit
        }
		if (current_block == NULL || current_block_size == 0){ break;}
        current_block = (blockHeader*)((void*) current_block + current_block_size); //update the current block to the next
    }
    
    if(best_fit_block ==  NULL){
        return NULL;
    }
    int remainingBytes = best_fit_block_size - strBlockSize;
    if (remainingBytes >= 8){
        int p_bite= best_fit_block->size_status - best_fit_block_size;
        best_fit_block->size_status = strBlockSize + p_bite + 1;

        blockHeader *new_free_block = (blockHeader*) ((void*) best_fit_block + strBlockSize);
        new_free_block->size_status = remainingBytes + 2;
        blockHeader *footer = (blockHeader*)((void*) new_free_block + (new_free_block->size_status & GET_BLOCK_SIZE) - sizeof(blockHeader)); 
        footer->size_status = remainingBytes;
    } else {
        int p_bite= best_fit_block->size_status - best_fit_block_size;
        best_fit_block->size_status = best_fit_block_size + p_bite + 1;

        blockHeader *next_block = (blockHeader*) ((void*) best_fit_block + best_fit_block_size);
        next_block->size_status += 2;
    }

	return ((void*)best_fit_block + sizeof(blockHeader));
} 




/* 
 * Function for freeing up a previously allocated block.
 * Argument ptr: address of the block to be freed up.
 * Returns 0 on success.
 * Returns -1 on failure.
 * This function should:
 * - Return -1 if ptr is NULL.
 * - Return -1 if ptr is not a multiple of 8.
 * - Return -1 if ptr is outside of the heap space.
 * - Return -1 if ptr block is already freed.
 * - Update header(s) and footer as needed.
 *
 * If free results in two or more adjacent free blocks,
 * they will be immediately coalesced into one larger free block.
 * so free blocks require a footer (blockHeader works) to store the size
 *
 * TIP: work on getting immediate coalescing to work after your code 
 *      can pass the tests in partA and partB of tests/ directory.
 *      Submit code that passes partA and partB to Canvas before continuing.
 */                    
int bfree(void *ptr) {    
    //TODO: Your code goes in here.
	blockHeader *ptr_header = (blockHeader*)((void*)ptr - sizeof(blockHeader));
    if(ptr == NULL || (int)ptr % 8 != 0
		||ptr_header < heap_start //write in a seprate if-else statement if it is not working, or add the parathsises to include everything 
		//	|| ptr > (blockHeader*)(void*)ptr + ptr_size) it is outside the end of this heap
			||(ptr_header->size_status & AND_ONE) == 0){
		return -1;
	}
	
	//free prt_block, update the a-bit in prt_block, add footer, update the p-bit in the next block 
	int ptr_size = (ptr_header->size_status) & GET_BLOCK_SIZE;
	ptr_header->size_status -= 1;//update a-bit in ptr block
	blockHeader *ptr_footer = (blockHeader*)((void*)ptr_header + ptr_size - sizeof(blockHeader));
	ptr_footer->size_status = ptr_size; //update size in footer
	blockHeader *adj_b_block = (blockHeader*)((void*)ptr_header + ptr_size);
	//it is not the END mark and it is not NULL
	if(adj_b_block->size_status != 1 && adj_b_block != NULL){
		adj_b_block->size_status -= 2;//clean the  p-bit in the adjacent back block
	}

	//coalescing adj_back_block, it is free and not NULL
	if (adj_b_block->size_status % 2 == 0 && adj_b_block != NULL){
		int adj_b_size = adj_b_block->size_status & GET_BLOCK_SIZE;
		blockHeader *adj_b_footer = (blockHeader*)((void*)adj_b_block + adj_b_size - sizeof(blockHeader));
		adj_b_footer->size_status = ptr_size + adj_b_size; //add both block size to the adj back block footer
		ptr_header->size_status += adj_b_size; //update the ptr header size, a bit has been freed above and p bit is unknown.
	}
	//coalescing adj_front_block, check it the ptr_header's p bit to get teh status
	if ((ptr_header->size_status & GET_P_BIT) == 0){
		blockHeader *adj_f_footer = (blockHeader*)((void*)ptr_header - sizeof(blockHeader)); //allocate the front block footer
		blockHeader *adj_f_block = (blockHeader*)((void*)ptr_header -adj_f_footer->size_status);//allocate the front block'header
		int adj_f_size = adj_f_block->size_status & GET_BLOCK_SIZE;
		blockHeader *new_footer =(blockHeader*)((void*)ptr_header + (ptr_header->size_status & GET_BLOCK_SIZE) - sizeof(blockHeader)); // footer can change its position dependons on if it coalescing with back block or not
		new_footer->size_status += adj_f_size; //update the size in footer of ptr block
		adj_f_block->size_status += ptr_header->size_status; //update the size in front block
	}

	return 0;
} 


/* 
 * Initializes the memory allocator.
 * Called ONLY once by a program.
 * Argument sizeOfRegion: the size of the heap space to be allocated.
 * Returns 0 on success.
 * Returns -1 on failure.
 */                    
int init_heap(int sizeOfRegion) {    

    static int allocated_once = 0; //prevent multiple myInit calls

    int   pagesize; // page size
    int   padsize;  // size of padding when heap size not a multiple of page size
    void* mmap_ptr; // pointer to memory mapped area
    int   fd;

    blockHeader* end_mark;

    if (0 != allocated_once) {
        fprintf(stderr, 
                "Error:mem.c: InitHeap has allocated space during a previous call\n");
        return -1;
    }

    if (sizeOfRegion <= 0) {
        fprintf(stderr, "Error:mem.c: Requested block size is not positive\n");
        return -1;
    }

    // Get the pagesize from O.S. 
    pagesize = getpagesize();

    // Calculate padsize as the padding required to round up sizeOfRegion 
    // to a multiple of pagesize
    padsize = sizeOfRegion % pagesize;
    padsize = (pagesize - padsize) % pagesize;

    alloc_size = sizeOfRegion + padsize;

    // Using mmap to allocate memory
    fd = open("/dev/zero", O_RDWR);
    if (-1 == fd) {
        fprintf(stderr, "Error:mem.c: Cannot open /dev/zero\n");
        return -1;
    }
    mmap_ptr = mmap(NULL, alloc_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (MAP_FAILED == mmap_ptr) {
        fprintf(stderr, "Error:mem.c: mmap cannot allocate space\n");
        allocated_once = 0;
        return -1;
    }

    allocated_once = 1;

    // for double word alignment and end mark
    alloc_size -= 8;

    // Initially there is only one big free block in the heap.
    // Skip first 4 bytes for double word alignment requirement.
    heap_start = (blockHeader*) mmap_ptr + 1;

    // Set the end mark
    end_mark = (blockHeader*)((void*)heap_start + alloc_size);
    end_mark->size_status = 1;

    // Set size in header
    heap_start->size_status = alloc_size;

    // Set p-bit as allocated in header
    // note a-bit left at 0 for free
    heap_start->size_status += 2;

    // Set the footer
    blockHeader *footer = (blockHeader*) ((void*)heap_start + alloc_size - 4);
    footer->size_status = alloc_size;

    return 0;
} 

/* STUDENTS MAY EDIT THIS FUNCTION, but do not change function header.
 * TIP: review this implementation to see one way to traverse through
 *      the blocks in the heap.
 *
 * Can be used for DEBUGGING to help you visualize your heap structure.
 * It traverses heap blocks and prints info about each block found.
 * 
 * Prints out a list of all the blocks including this information:
 * No.      : serial number of the block 
 * Status   : free/used (allocated)
 * Prev     : status of previous block free/used (allocated)
 * t_Begin  : address of the first byte in the block (where the header starts) 
 * t_End    : address of the last byte in the block 
 * t_Size   : size of the block as stored in the block header
 */
void disp_heap(){

    int counter;
    char status[6];
    char p_status[6];
    char *t_begin = NULL;
    char *t_end = NULL;
    int t_size;

    blockHeader *current = heap_start;
    counter = 1;

    int used_size = 0;
    int free_size = 0;
    int is_used = -1;

    fprintf(stdout,
            "********************************** HEAP: Block List ****************************\n");
    fprintf(stdout, "No.\tStatus\tPrev\tt_Begin\t\tt_End\t\tt_Size\n");
    fprintf(stdout,
            "--------------------------------------------------------------------------------\n");

    while (current->size_status != 1)
    {
        t_begin = (char *)current;
        t_size = current->size_status;

        if (t_size & 1)
        {
            // LSB = 1 => used block
            strcpy(status, "alloc");
            is_used = 1;
            t_size = t_size - 1;
        }
        else
        {
            strcpy(status, "FREE ");
            is_used = 0;
        }

        if (t_size & 2)
        {
            strcpy(p_status, "alloc");
            t_size = t_size - 2;
        }
        else
        {
            strcpy(p_status, "FREE ");
        }

        if (is_used)
            used_size += t_size;
        else
            free_size += t_size;

        t_end = t_begin + t_size - 1;

        fprintf(stdout, "%d\t%s\t%s\t0x%08lx\t0x%08lx\t%4i\n", counter, status,
                p_status, (unsigned long int)t_begin, (unsigned long int)t_end, t_size);

        current = (blockHeader *)((char *)current + t_size);
        counter = counter + 1;
    }

    fprintf(stdout,
            "--------------------------------------------------------------------------------\n");
    fprintf(stdout,
            "********************************************************************************\n");
    fprintf(stdout, "Total used size = %4d\n", used_size);
    fprintf(stdout, "Total free size = %4d\n", free_size);
    fprintf(stdout, "Total size      = %4d\n", used_size + free_size);
    fprintf(stdout,
            "********************************************************************************\n");
    fflush(stdout);

    return;
}                     
