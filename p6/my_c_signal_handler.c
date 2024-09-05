///////////////////////////////////////////////////////////////////////////////
// Main File:        my_c_signal_handler.c
// This File:        my_c_signal_handler.c
// Other Files:      send_signal.c my_div0_handler.c
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
// 05/02/2024			modified strtol() method
// 
///////////////////////// OTHER SOURCES OF HELP ////////////////////////////// 
// Persons:          Identify persons by name, relationship to you, and email.
//                   Describe in detail the the ideas and help they provided.
// Date:   Name (email):   Helped with: (brief description)
// ---------------------------------------------------------------------------
// 
//////////////////////////// 80 columns wide ///////////////////////////////////
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int counter = 0;

void signal_handler(){
	time_t current_time;
	time(&current_time);
	printf("PID: %d CURRENT TIME: %s", getpid(),ctime(&current_time));
	alarm(5);
} 	

void break_handler(){
	printf("SIGINT handled.\n SIGUSR1 was handled %i times. Exiting now.\n", counter);
	exit(0);
	
} 

void user_sig_handler(){
	counter++;
	printf("SIGUSR1 handled and counted!\n");
}

int main(){
	struct sigaction act;
	memset(&act, 0, sizeof(act));
	act.sa_handler = &signal_handler;
	if (sigaction(SIGALRM, &act, NULL) != 0){
		printf("Failed to register a signal handler. \n");
	}
	act.sa_handler = &break_handler;
	if (sigaction(SIGINT, &act, NULL) != 0){
		printf("Failed to register a break handler when type Ctrl +C");
	}
	act.sa_handler = &user_sig_handler;
	if (sigaction(SIGUSR1, &act, NULL) != 0){
		printf("Failed to register user program signal");
	}
	
	printf("PID and time print every 5 seconds.\nType Ctrl-C to end the program.\n");
	alarm(5);
	while(1){
	}
return 0;
}
