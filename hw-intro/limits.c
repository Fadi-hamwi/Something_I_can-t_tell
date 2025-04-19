#include <stdio.h>
#include <sys/resource.h>

int main() {
    struct rlimit lim;
    if(getrlimit(RLIMIT_STACK, &lim) == -1) {
        fprintf(stderr, "Couldn't use softlimits of stack.\n");
    }
    if(lim.rlim_cur == RLIM_INFINITY) {
        printf("Stack size of the process is unlimited.\n");
    }else {
        printf("stack size: %lud\n", (unsigned long)lim.rlim_cur);
    }

    if(getrlimit(RLIMIT_AS, &lim) == -1) {
        fprintf(stderr, "couldn't set the soft limit of process address space.\n");
    }
    if(lim.rlim_cur == RLIM_INFINITY) {
        printf("Process address is unlimited.\n");
    }else {
        printf("process limit: %lud\n", (unsigned long)lim.rlim_cur);
    }

    if(getrlimit(RLIMIT_NOFILE, &lim) == -1) {
        fprintf(stderr, "Couldn't set the max number of file descriptors.\n");
    }

    if(lim.rlim_cur == RLIM_INFINITY) {
        printf("Number of file descriptors is unlimited.\n");
    }else {
        printf("max file descriptors: %lud\n", (unsigned long)lim.rlim_cur);
    }
    return 0;
}
