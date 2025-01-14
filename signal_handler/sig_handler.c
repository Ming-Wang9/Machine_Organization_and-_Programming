#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

void sig_handler(){
    printf("Beep\n");
    alarm(1);
}

int main(){
    struct sigaction sa;
    sa.sa_flags = 0;
    sa.sa_handler = sig_handler;
    if(sigaction(SIGALRM, &sa, NULL) != 0){
        printf("Failed to bind handler.\n");
        exit(1);
    }
    printf("Going into an infinite loop.\n");
    alarm(1);
    while(1);
    return 0;
}

