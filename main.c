// gcc process.c job.c parse.c background.c execute.c builtin.c main.c -o dansh

#include <stdio.h>
#include <wait.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "job.h"
#include "process.h"
#include "background.h"
#include "execute.h"
#include "parse.h"


void ignore_signal() {
    return;
}

void put_in_background(struct Job *j) {
    add_job_to_background(j);
    buildJobCmd(j);
    printJob(j);
}

void wait_for_job(struct Job *j) {
    int status;
    pid_t child_pid;

    while ((child_pid = waitpid(j->pgid * -1, &status, WUNTRACED)) > -1) {
        if (WIFEXITED(status) || WIFSIGNALED(status)) {
            j->state = JOB_STATE_TERMINATED;
            cleanupJob(j);
        }

        if (WIFSTOPPED(status)) { // TSTP, doesn't end while loop
            j->state = JOB_STATE_STOPPED;
            put_in_background(j);
            break;
        }
    }
}

struct Job *foreground_job;

int main () {
    signal(SIGINT, ignore_signal);
    signal(SIGCHLD, manage_background_job_state);
    setbuf(stdout, NULL); // Write to STDOUT immediately; don't buffer

    char buffer[1024];
    
    while (1) {
        printf("> ");
        fgets(buffer, 1024, stdin);

        foreground_job = newJob();
        parse_command(buffer, foreground_job);

        switch (execute(foreground_job)) {
            case EXECUTE_EXTERNAL_BG:
                put_in_background(foreground_job);
                break;
            case EXECUTE_BUILTIN_JOBS:
            case EXECUTE_BUILTIN_BG:
                cleanupJob(foreground_job);
                break;
            default:
                wait_for_job(foreground_job);
                Tcsetpgrp(STDIN_FILENO, getpid());
        }

        foreground_job = NULL;
    }
}