#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <wait.h>

void sigusr1_handler(int signo, siginfo_t *info, void *context){
    // display info
    printf("SIGINFO\n");
	printf("SIGNO: %d, SIGCODE: %d PID: %d, UID: %d, STATUS: %d\n", info->si_signo, info->si_code, info->si_pid, info->si_uid, info->si_status);
}

void test_sigusr1(void){
	struct sigaction action;
	action.sa_sigaction = sigusr1_handler; 
	action.sa_flags = SA_SIGINFO;
	sigemptyset(&action.sa_mask);

	if(sigaction(SIGUSR1, &action, NULL) == -1){
		printf("Error when assigning action to signal\n");
		return;
	}

	kill(getpid(), SIGUSR1);
}

void sigchild_handler(int signo, siginfo_t *info, void *context){
	// display info
    printf("SIGCHILD\n");
	printf("SIGNO: %d, SIGCODE: %d PID: %d, UID: %d, STATUS: %d\n", info->si_signo, info->si_code, info->si_pid, info->si_uid, info->si_status);
}

int test_sigchild(void){
	struct sigaction action;
	action.sa_sigaction = sigchild_handler;
	action.sa_flags = SA_SIGINFO;
	sigemptyset(&action.sa_mask);

	if(sigaction(SIGCHLD, &action, NULL) == -1){
		printf("Error when assigning action to signal\n");
		return 1;
	}

    // create and kill child so SIGCHILD is generated
	pid_t pid = fork();
	if(pid < 0){
		printf("fork error\n");
		return 0;
	}else if(pid == 0){
		exit(0xCC);
	}

	wait(NULL);

	return 0;
}

void sigfpe_handler(int signo, siginfo_t *info, void *context){
    // display info
    printf("SIGFPE\n");
	printf("SIGNO: %d, SIGCODE: %d PID: %d, UID: %d, STATUS: %d\n", info->si_signo, info->si_code, info->si_pid, info->si_uid, info->si_status);
    exit(EXIT_FAILURE); // we exit as it is program breaking error
}

void test_sigfpe(void){
	struct sigaction action;
	action.sa_sigaction = sigfpe_handler;
	action.sa_flags = SA_SIGINFO;
	sigemptyset(&action.sa_mask);

	if(sigaction(SIGFPE, &action, NULL) == -1){
		printf("Cant catch SIGFPE\n");
		return;
	}
    
    // generate error, so signal is send
	int c = 42 / 0;
}

int main() {

	test_sigusr1();
	test_sigchild();
	test_sigfpe();

	return 0;
}