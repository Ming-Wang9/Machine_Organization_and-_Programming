////////////////////////////////////////////////////////////////////////////////
// Main File:        my_c_signal_handler.c
// This File:        send_signal.c
// Other Files:      my_div0_handler.c
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
#include <unistd.h>
#include <signal.h>

int main(int argc, char **argv) {
	if(argc != 3){
		printf("Usage: send_signal <signal type> <pid> \n");
		return 1;
	}

	int pid;
	pid = atoi(argv[2]);

	if (argv[1][0] != '-'){
		printf("Invalid signal type (-) \n");
		return 1;
	} else {
		if(argv[1][1] == 'u'){
			kill(pid, SIGUSR1);
		} else if (argv[1][1] == 'i'){
			kill(pid, SIGINT);
		} else {
			printf("Invalid signal type (letter) \n");
		}
	}
	return 1;
}

