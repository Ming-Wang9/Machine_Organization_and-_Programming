////////////////////////////////////////////////////////////////////////////////
// Main File:        my_magic_square.c
// This File:        my_magic_square.c
// Other Files:      N/A
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
// 02/22/2024	Siamese method				https://en.wikipedia.org/wiki/Siamese_method
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
//		Feb 22				download file from remote					 
//		Feb 23				complete getSize, fileOutputMagicSquare, and main methods
//		Feb 23				disconnect from remote, recover file with ./my_magic_square.c.swp
//		Feb 23				test with magic_square.txt files
// 
///////////////////////// OTHER SOURCES OF HELP ////////////////////////////// 
// Persons:          Identify persons by name, relationship to you, and email.
//                   Describe in detail the the ideas and help they provided.
// Date:   Name (email):   Helped with: (brief description)
// ---------------------------------------------------------------------------
// 
//////////////////////////// 80 columns wide ///////////////////////////////////

// add your own File Header information here (as described in Commenting guide)

///////////////////////////////////////////////////////////////////////////////
// Copyright 2020 Jim Skrentny
// Posting or sharing this file is prohibited, including any changes/additions.
// Used by permission, CS 354 Spring 2024, Deb Deppeler
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure that represents a magic square
typedef struct {
	int size;           // dimension of the square
	int **magic_square; // pointer to heap allocated magic square
} MagicSquare;

/* TODO:
 * Promps the user for the magic square's size, read it,
 * check if it's an odd number >= 3 (if not, display the required
 * error message and exit)
 *
 * return the valid number
 */
int getSize() {
	int size;
	printf("Enter magic square's (odd integer >=3)\n");
	scanf("%d", &size);

	if (size < 3 && size !=2) {
		printf("Magic square size must be >= 3.\n");
		exit(1);
	}

	if (size % 2 != 1 || size < 3){
		printf("Magic square size must be odd.\n");
		exit(1);
	}

	return size;   
} 

/* TODO:
 * Makes a magic square of size n,
 * and stores it in a MagicSquare (on the heap)
 *
 * It may use the Siamese magic square algorithm 
 * or alternate from assignment 
 * or another valid algorithm that produces a magic square.
 *
 * n - the number of rows and columns
 *
 * returns a pointer to the completed MagicSquare struct.
 */
MagicSquare *generateMagicSquare(int n) {
	MagicSquare *magicSquare = malloc(n * sizeof(MagicSquare));
	if(magicSquare ==  NULL){
		printf("%s", "Memory allocation failure in MagicSquare strucs");
		exit(1);
	}	

	magicSquare->magic_square = malloc(n * sizeof(int*));
	if(magicSquare->magic_square ==  NULL){
		printf("%s", "Memory allocation failure in **magic_square");
		exit(1);
	}
	
	for (int i = 0; i < n; i++){
		*(magicSquare->magic_square + i) =  malloc(n * sizeof(int));
		if (*(magicSquare->magic_square + i) == NULL){
			printf("%s", "Memory allocation failure in *magic_square");
			exit(1);
		}
	}

	for (int i = 0; i < n; i++){
		for (int j = 0; j < n; j++){
			 *(*(magicSquare->magic_square + i) + j) = 0;
		}
	}

	int num = 1;
	int row = 0;
	int col = (n-1)/2; //(n+1)/2 is not working because it starts from 0

	while (num <= n*n){
		*(*(magicSquare->magic_square + row) + col) = num;
		
		int newRow = (row - 1 + n) % n; //recursicve?
		int newCol = (col + 1) % n;
			
		//move up and right if it is a empty cell
		if (*(*(magicSquare->magic_square + newRow) + newCol) == 0){
			row = newRow;
			col = newCol;
		} else {
			row = (row + 1) % n;//move down if it is filled
		}
		num++;
	}

	magicSquare->size = n;	

	return magicSquare;	
}    

/* TODO:  
 * Opens a new file (or overwrites the existing file)
 * and writes the magic square values to the file
 * in the specified format.
 *
 * magic_square - the magic square to write to a file
 * filename - the name of the oiutput file
 */
void fileOutputMagicSquare(MagicSquare *magic_square, char *filename) {
	FILE *fptr = fopen(filename, "w");
	if (fptr == NULL){
		printf("%s", "Can not open file");
		exit(1);
	}
	
	int sizeOfMagicSquare = magic_square->size;
	int **msPtr = magic_square->magic_square;
	
	fprintf(fptr, "%d\n", sizeOfMagicSquare);
	
	for (int i = 0; i <sizeOfMagicSquare; i++){
		for (int j = 0; j < sizeOfMagicSquare; j++){
			fprintf(fptr, "%d", *(*(msPtr+i)+j));
			if (j != sizeOfMagicSquare - 1){
				fprintf(fptr, "%c", ',');
			}
		}
		fprintf(fptr, "\n");
	}
	
	fclose(fptr);	
}

/* TODO:
 * Generates a magic square of the user specified size and
 * outputs the square to the output filename.
 * 
 * Add description of required CLAs here
 */
int main(int argc, char **argv) {
	// TODO: Check input arguments to get output filename
	if (argc != 2){
		printf("Usage: %s <output_filename>\n", *argv);
		exit(1);
	}
		
	// TODO: Get magic square's size from user
		int size = getSize();

	// TODO: Generate the magic square by correctly interpreting 
	//       the algorithm(s) in the write-up or by writing or your own.  
	//       You must confirm that your program produces a 
	//       Magic Sqare as described in the linked Wikipedia page.
	MagicSquare *magic_square = generateMagicSquare(size);

	// TODO: Output the magic square
	fileOutputMagicSquare(magic_square, *(argv+1));
	
	for (int i = 0; i < size; i++){
		free (*(magic_square->magic_square+i));
	}
	free(magic_square->magic_square);
	free(magic_square);

	return 0;	
} 

// Spring 2024


