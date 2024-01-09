#ifndef PROCESS_H
#define PROCESS_H

#include <sys/wait.h>

#define PROCESS__MAX_ARGS 128

struct Process {
    pid_t pid;
    char *args[PROCESS__MAX_ARGS];
};

struct Process *newProcess();
void cleanupProcess(struct Process *);

#endif // PROCESS_H