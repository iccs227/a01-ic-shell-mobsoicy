#include "unistd.h"
#include "headers/icsh.h"
#include <sys/wait.h>

void sigint_handler(int signo) {
    if (fgpid > 0) {
        kill(fgpid, SIGINT);
    }
}

void sigtstp_handler(int signo) {
    if (fgpid > 0) {
        kill(fgpid, SIGTSTP);
    }
}