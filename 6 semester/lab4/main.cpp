#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <wait.h>

int counter = 0;
volatile sig_atomic_t stop_flag = 0;

void signal_handler(int signum) {
    stop_flag = 1;
	
    if (stop_flag) {    
	pid_t child_pid = fork();
		
	if (child_pid > 0) {
	    printf("\n=========================\n");
	    printf("Child process created. Counter: %d\n", counter);
	    wait(NULL);
	}		
		
	else if (child_pid == 0) {
	    printf("Parent process stopped. Counter: %d\n", counter);
	    printf("=========================\n");
	}
			
	else if (child_pid < 0) {
	    fprintf(stderr, "Error: Fork failed.\n");
	    exit(1);
	}
			
	stop_flag = 0;
    }
}

int main() {
    signal(SIGINT, signal_handler);

    while (counter < 30) {
	printf("Running... Counter: %d\n", counter);
	counter++;

	sleep(1);
    }
    
    return 0;
}

