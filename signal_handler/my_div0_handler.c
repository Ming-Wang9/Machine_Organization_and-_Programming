////////////////////////////////////////////////////////////////////////////////
// Main File:        my_c_signal_handler.c
// This File:        my_div0_handler.c
// Other Files:      send_signal.c
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
// 04/29/2024 sigaction in c           https://man7.org/linux/man-pages/man2/sigaction.2.html
// 04/29/2024 alarm in singal handler  https://pubs.opengroup.org/onlinepubs/007904875/functions/alarm.html
// 04/29/2024 how to use fgets()       https://www.digitalocean.com/community/tutorials/fgets-and-gets-in-c-programming
// 04/30/2024 strtol() in c            https://www.tutorialspoint.com/c_standard_library/c_function_strtol.htm
// 04/30/2024 send signal to handler   https://www.tutorialspoint.com/c_standard_library/c_function_raise.htm
// AI chats: save a transcript.  No need to submit, just have available 
// if asked.
/////////////////////////// COMMIT LOG  ////////////////////////////// 
//  Date and label your work sessions, include date and brief description
//  Date:   Commit Message: 
//  -------------------------------------------------------------------
// 04/29/2024           complete basic frame work of my_c_signal_handler.c 
// 04/29/2024           finished send_signal.c
// 04/30/2024           finished my_div0_handler.c and my_c_signal_handler.c
// 05/02/2024           modified strtol() method
// 
///////////////////////// OTHER SOURCES OF HELP ////////////////////////////// 
// Persons:          Identify persons by name, relationship to you, and email.
//                   Describe in detail the the ideas and help they provided.
// Date:   Name (email):   Helped with: (brief description)
// ---------------------------------------------------------------------------
// 
//////////////////////////// 80 columns wide ///////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <string.h>

int counter = 0;

void div_zero_handler(){
	printf("Error: a division by 0 operation was attempted.\n");
	printf("Total number of operations completed successfully: %i\n", counter);
	printf("The program will be terminated.\n");
	exit(0);
}

void terminate_handler(){
	printf("Total number of operations completed successfully: %i\n", counter);
	printf("The program will be terminated.\n");
	exit(0);
}

int main(){
	int first_int;
	int second_int;
	struct sigaction act;
	memset(&act, 0, sizeof(act));
	act.sa_handler = &div_zero_handler;
	if (sigaction(SIGFPE, &act, NULL) != 0){
		printf("Failed to register a divide zero handler. \n");
	}
	act.sa_handler = &terminate_handler;
	if (sigaction(SIGINT, &act, NULL) != 0){
		printf("Failed to register a termination handler. \n");
	}
	while (1){
		char string[20];
		char *endptr;
		printf("Enter first integer: ");
		fgets(string, sizeof(string), stdin);
		first_int = strtol(string, &endptr, 10);
		if (string == endptr){
			first_int = 0;
		}
		printf("Enter second integer: ");
		fgets(string, sizeof(string), stdin);
		second_int = strtol(string, &endptr, 10);
		
		
		printf("%i / %i is %i  with a reminder of %i \n", first_int, second_int, first_int/second_int, first_int%second_int);
		counter++;
		
	}
	return 0;
}
