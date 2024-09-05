////////////////////////////////////////////////////////////////////////////////
// Main File:        csim.c
// This File:        csim.c
// Other Files:      csim-ref* Makefile test-csim*
// Semester:         CS 354 Lecture 001      SPRING 2024
// Grade Group:      gg 11  (See canvas.wisc.edu/groups for your gg#)
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
// 04/04/2024   bit mask    //https://stackoverflow.com/questions/68726311/how-to-get-a-bit-range-from-a-hex 
// 
// 
// 
// 
// AI chats: save a transcript.  No need to submit, just have available 
// if asked.
/////////////////////////// COMMIT LOG  ////////////////////////////// 
//  Date and label your work sessions, include date and brief description
//  Date:   Commit Message: 
//  -------------------------------------------------------------------
//  04/04/2024		complete init-cache, free_cache, some lines in reply_trace 
//	04/04/2024		find the t bit and s bit mask
//	04/05/2024		using count to implement the LRU replcement policy
// 
// 
///////////////////////// OTHER SOURCES OF HELP ////////////////////////////// 
// Persons:          Identify persons by name, relationship to you, and email.
//                   Describe in detail the the ideas and help they provided.
// Date:   Name (email):   Helped with: (brief description)
// ---------------------------------------------------------------------------
// 
//////////////////////////// 80 columns wide ///////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Copyright 2013,2019-2024
// Posting or sharing this file is prohibited, including any changes/additions.
// Used by permission for Spring 2024
////////////////////////////////////////////////////////////////////////////////

/**
 * csim.c:  
 * A cache simulator that can replay traces (from Valgrind) and output
 * statistics to determine the number of hits, misses, and evictions.
 * The replacement policy is LRU.
 *
 * Implementation and assumptions:
 *  1. (L) load or (S) store cause at most one cache miss and a possible eviction.
 *  2. (I) Instruction loads are ignored.
 *  3. (M) Data modify is treated as a load followed by a store to the same
 *  address. Hence, an (M) operation can result in two cache hits, 
 *  or a miss and a hit plus a possible eviction.
 */  

#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

/******************************************************************************/
/* DO NOT MODIFY THESE VARIABLE NAMES and TYPES                               */
/* DO UPDATE THEIR VALUES AS NEEDED BY YOUR CSIM                              */

// Globals set by command line args.
int b = 0; // Number of (b) bits
int s = 0; // Number of (s) bits
int E = 0; // Number of lines per set

// Globals derived from command line args.
int B; // Block size in bytes: B = 2^b
int S; // Number of sets: S = 2^s

// Global counters to track cache statistics in access_data().
int hit_cnt = 0;
int miss_cnt = 0;
int evict_cnt = 0;

// Global to control trace output
int verbosity = 0; // Print trace if set
/******************************************************************************/

// Type mem_addr_t: Use when dealing with addresses or address masks.
typedef unsigned long long int mem_addr_t;

//Type cache_line_t: Use when dealing with cache lines.
//TODO - COMPLETE THIS TYPE
typedef struct cache_line {
    char valid;
    mem_addr_t tag;
	//Add a data member as needed by your implementation
    int lru_counter; // Add a counter for LRU policy
} cache_line_t;

// Type cache_set_t: Use when dealing with cache sets
// Note: Each set is a pointer to a heap array of one or more cache lines.
typedef cache_line_t* cache_set_t;

// Type cache_t: Use when dealing with the cache.
// Note: A cache is a pointer to a heap array of one or more sets.
typedef cache_set_t* cache_t;

// Create the cache we're simulating.
// Note: A cache is a pointer to a heap array of one or more cache sets.
cache_t cache;

/* TODO - COMPLETE THIS FUNCTION
 * init_cache:
 * Allocates the data structure for a cache with S sets and E lines per set.
 * Initializes all valid bits and tags with 0s.
 */ 
void init_cache() {
    B = (int)pow(2, b);
    S = (int)pow(2, s);
	//allocate sets for cache
    cache = (cache_t)malloc(S * sizeof(cache_set_t));
    if (cache == NULL) {
        printf("%s", "Memory allocation for sets failed");
        exit(1);
    }
	//allocate lines for sets
    for (int i = 0; i < S; i++) {
        cache[i] = (cache_set_t)malloc(E * sizeof(cache_line_t));
        if (cache[i] == NULL) {
            printf("%s", "Memory allocation for lines failed");
            exit(1);
        }
        for (int j = 0; j < E; j++) {
            cache[i][j].valid = 0;
            cache[i][j].tag = 0;
            cache[i][j].lru_counter = 0;
        }
    }
}

/* TODO - COMPLETE THIS FUNCTION 
 * free_cache:
 * Frees all heap allocated memory used by the cache.
 */ 
void free_cache() {
    for (int i = 0; i < S; i++) {
        free(cache[i]);
    }
    free(cache);
    cache = NULL;
}

/* TODO - COMPLETE THIS FUNCTION 
 * access_data:
 * Simulates data access at given "addr" memory address in the cache.
 *
 * If already in cache, increment hit_cnt
 * If not in cache, cache it (set tag), increment miss_cnt
 * If a line is evicted, increment evict_cnt
 */  
void access_data(mem_addr_t addr) {
    int set_index = (addr >> b) & (S - 1);
    mem_addr_t tag = addr >> (s + b);
	//https://stackoverflow.com/questions/68726311/how-to-get-a-bit-range-from-a-hex	

    int lru_index = 0;
    int max_lru_counter = -1;
    int empty_line_index = -1;
    for (int i = 0; i < E; i++) {
        if (cache[set_index][i].valid && cache[set_index][i].tag == tag) {
            // Cache hit
            hit_cnt++;
            for (int j = 0; j < E; j++) {
                if (j != i && cache[set_index][j].valid) {
                    cache[set_index][j].lru_counter++;
                }
            }
            cache[set_index][i].lru_counter = 0;
            return;
        }
        if (!cache[set_index][i].valid && empty_line_index == -1) {
            // Empty line found
            empty_line_index = i;
        }
        if (cache[set_index][i].lru_counter > max_lru_counter) {
            // Update least recently used line
            lru_index = i;
            max_lru_counter = cache[set_index][i].lru_counter;
        }
    }

    // Cache miss
    miss_cnt++;
    if (empty_line_index != -1) {
        // Fill empty line
        cache[set_index][empty_line_index].valid = 1;
        cache[set_index][empty_line_index].tag = tag;
        cache[set_index][empty_line_index].lru_counter = 0;
    } else {
        // Evict least recently used line
        evict_cnt++;
        cache[set_index][lru_index].tag = tag;
        cache[set_index][lru_index].lru_counter = 0;
    }
    for (int i = 0; i < E; i++) {
        if (i != empty_line_index && i != lru_index && cache[set_index][i].valid) {
            cache[set_index][i].lru_counter++;
        }
    }
}

/* TODO - FILL IN THE MISSING CODE
 * replay_trace:
 * Replays the given trace file against the cache.
 *
 * Reads the input trace file line by line.
 * Extracts the type of each memory access : L/S/M
 * TRANSLATE each "L" as a load i.e. 1 memory access
 * TRANSLATE each "S" as a store i.e. 1 memory access
 * TRANSLATE each "M" as a load followed by a store i.e. 2 memory accesses 
 */     
void replay_trace(char* trace_fn) {
    char buf[1000];
    mem_addr_t addr = 0;
    unsigned int len = 0;
    FILE* trace_fp = fopen(trace_fn, "r");

    if (!trace_fp) {
        fprintf(stderr, "%s: %s\n", trace_fn, strerror(errno));
        exit(1);
    }

    while (fgets(buf, 1000, trace_fp) != NULL) {
        if (buf[1] == 'S' || buf[1] == 'L' || buf[1] == 'M') {
            sscanf(buf + 3, "%llx,%u", &addr, &len);

            if (verbosity)
                printf("%c %llx,%u ", buf[1], addr, len);

			// TODO - MISSING CODE
            // GIVEN: 1. addr has the address to be accessed
            //        2. buf[1] has type of acccess(S/L/M)
            // call access_data function here depending on type of access
            if (buf[1] == 'S' || buf[1] == 'L') {
                access_data(addr);
            }
            if (buf[1] == 'M') {
                access_data(addr);
                access_data(addr);
            }

            if (verbosity)
                printf("\n");
        }
    }

    fclose(trace_fp);
}

/*
 * print_usage:
 * Print information on how to use csim to standard output.
 */ 
void print_usage(char* argv[]) {
    printf("Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n", argv[0]);
    printf("Options:\n");
    printf("  -h         Print this help message.\n");
    printf("  -v         Verbose flag.\n");
    printf("  -s <num>   Number of s bits for set index.\n");
    printf("  -E <num>   Number of lines per set.\n");
    printf("  -b <num>   Number of b bits for word and byte offsets.\n");
    printf("  -t <file>  Trace file.\n");
    printf("\nExamples:\n");
    printf("  linux>  %s -s 4 -E 1 -b 4 -t traces/yi.trace\n", argv[0]);
    printf("  linux>  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n", argv[0]);
    exit(0);
}


/*
 * print_summary:
 * Prints a summary of the cache simulation statistics to a file.
 */
void print_summary(int hits, int misses, int evictions) {
    printf("hits:%d misses:%d evictions:%d\n", hits, misses, evictions);
    FILE* output_fp = fopen(".csim_results", "w");
    assert(output_fp);
    fprintf(output_fp, "%d %d %d\n", hits, misses, evictions);
    fclose(output_fp);
}

/*
 * main:
 * parses command line args, 
 * makes the cache, 
 * replays the memory accesses, 
 * frees the cache and 
 * prints the summary statistics.  
 */      
int main(int argc, char* argv[]) {
    char* trace_file = NULL;
    char c;

    // Parse the command line arguments: -h, -v, -s, -E, -b, -t
    while ((c = getopt(argc, argv, "s:E:b:t:vh")) != -1) {
        switch (c) {
            case 'b':
                b = atoi(optarg);
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 'h':
                print_usage(argv);
                exit(0);
            case 's':
                s = atoi(optarg);
                break;
            case 't':
                trace_file = optarg;
                break;
            case 'v':
                verbosity = 1;
                break;
            default:
                print_usage(argv);
                exit(1);
        }
    }

    // Make sure that all required command line args were specified.
    if (s == 0 || E == 0 || b == 0 || trace_file == NULL) {
        printf("%s: Missing required command line argument\n", argv[0]);
        print_usage(argv);
        exit(1);
    }

    // Initialize cache.
    init_cache();

    // Replay the memory access trace.
    replay_trace(trace_file);

    // Free memory allocated for cache.
    free_cache();

    // Print the statistics to a file.
    // DO NOT REMOVE: This function must be called for test_csim to work.
    print_summary(hit_cnt, miss_cnt, evict_cnt);
    return 0;
}

// 202401 
