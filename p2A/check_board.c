// Add your file header here
// Fix compiler error
// Implement remaining functionality


///////////////////////////////////////////////////////////////////////////////
// Copyright 2021-24 Deb Deppeler
// Posting or sharing this file is prohibited, including any changes/additions.
//
// We have provided comments and structure for this program to help you get 
// started.  Later programs will not provide the same level of commenting,
// rather you will be expected to add same level of comments to your work.
// 09/20/2021 Revised to free memory allocated in get_board_size function.
// 01/24/2022 Revised to use pointers for CLAs
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *DELIM = ",";  // commas ',' are a common delimiter character for data strings

/* TODO: implement this function
 * Returns 1 if and only if the board is in a valid Sudoku board state.
 * Otherwise returns 0.
 * 
 * A valid row or column contains only blanks or the digits 1-size, 
 * with no duplicate digits, where size is the value 1 to 9.
 * 
 * Note: p2A requires only that each row and each column are valid.
 * 
 * board: heap allocated 2D array of integers 
 * size:  number of rows and columns in the board
 */
int valid_board(int **board, int size) {
	if (size < 1 || size > 9){
		return 0;
	}
	for (int i = 0; i < size; i++){
		for (int j = 0; j < size; j++){
			int value = *(*(board + i) + j);
			if (value == 0) {
				continue;
			}

			for (int k = j+1; k < size; k++){
				if (value == *(*(board + i) + k)){
					return 0;
				}
			}
			for (int p = i + 1; p < size; p++){
				if (value == *(*(board + p) + j)){
					return 0;
				}	
			}	
		}
	}	
	return 1;
}
	
/* COMPLETED (DO NOT EDIT):       
 * Read the first line of file to get the size of that board.
 * 
 * PRE-CONDITION #1: file exists
 * PRE-CONDITION #2: first line of file contains valid non-zero integer value
 *
 * fptr: file pointer for the board's input file
 * size: a pointer to an int to store the size
 *
 * POST-CONDITION: the integer whos address is passed in as size (int *) 
i * will now have the size (number of rows and cols) of the board being checked.
 */
void get_board_size(FILE *fptr, int *size) {      
	char *line = NULL;
	size_t len = 0;

	// 'man getline' to learn about <stdio.h> getline
	if ( getline(&line, &len, fptr) == -1 ) {
		printf("Error reading the input file.\n");
		free(line);
		exit(1);
	}

	char *size_chars = NULL;
	size_chars = strtok(line, DELIM);
	*size = atoi(size_chars);

	// free memory allocated for line 
	free(line);
	line = NULL;
}


/* TODO: COMPLETE THE MAIN FUNCTION
 * This program prints "valid" (without quotes) if the input file contains
 * a valid state of a Sudoku puzzle board wrt to rows and columns only.
 * It prints "invalid" (without quotes) if the input file is not valid.
 *
 * Usage: A single CLA that is the name of a file that contains board data.
 *
 * argc: the number of command line args (CLAs)
 * argv: the CLA strings, includes the program name
 *
 * Returns 0 if able to correctly output valid or invalid.
 * Exit with a non-zero result if unable to open and read the file given.
 */
int main( int argc, char **argv ) {              

	// TODO: Check if number of command-line arguments is correct.
	if (argc != 2){
		printf("Usage: %s check_board\n", *argv);
		exit(1);
	}

	// Open the file 
	FILE *fp = fopen(*(argv + 1), "r");
	if (fp == NULL) {
		printf("Can't open file for reading.\n");
		exit(1);
	}

	// will store the board's size, number of rows and columns
	int size;

	// TODO: Call get_board_size to read first line of file as the board size.	
	get_board_size(fp,&size);

	// TODO: Dynamically allocate a 2D array for given board size.
	// You must dyamically create a 1D array of pointers to other 1D arrays of ints
	int **board = malloc(size * sizeof(int *));
	if (board == NULL){
		printf("%s\n", "Memory allocation failed");
		exit(1);
	}
	for (int i = 0; i < size; i++){
		*(board+i) = malloc(size *  sizeof(int));
		if (*(board +i) == NULL){
			printf("%s\n", "Memory allocation failed");
			for (int i = 0; i < size; i++){
				free(*(board+i));
			}
			free(board); 
			exit(1);
		}
	}
 
	// Read the remaining lines.
	// Tokenize each line and store the values in your 2D array.
	char *line = NULL;
	size_t len = 0;
	char *token = NULL;
	for (int i = 0; i < size; i++) {

		// read the line
		if (getline(&line, &len, fp) == -1) {
			printf("Error while reading line %i of the file.\n", i+2);
			exit(1);
		}

		token = strtok(line, DELIM);
		for (int j = 0; j < size; j++) {
			// TODO: Complete the line of code below
			// to initialize elements of your 2D array.
			/* ADD ARRAY ACCESS CODE HERE */
			*(*(board+i)+j) = atoi(token); 
			token = strtok(NULL, DELIM);
		}
	}

	// TODO: Call valid_board and print the appropriate
	//       output depending on the function's return value.
	if(valid_board(board, size)){
		printf("%s\n", "valid");
	} else {
		printf("%s\n", "invalid");
	}

	// TODO: Free dynamically allocated memory.
	for (int i = 0; i < size; i++){	
		free(*(board+i));
	}
	free(board);

	//Close the file.
	if (fclose(fp) != 0) {
		printf("Error while closing the file.\n");
		exit(1);
	} 

	return 0;       
}       
