#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "execute.h"
#include "process.h"
#include "background.h"
#include "builtin.h"

extern struct Job *foreground_job;

pid_t fork_children(struct Job *job, int idx, pid_t pgid, int input_fd) {
    struct Process *p = job->pipeline[idx];
    int downstream_exists = (idx + 1) < job->pcount;
    int pipefd[2];
    int is_leader = pgid ? 0 : 1;

    if (downstream_exists) { // If there is a downstream process
        if (pipe(pipefd) == -1) { // Create a pipe to send output to
            perror("pipe");
            exit(1);
        };
    }

    pid_t child_pid = fork();

    if (child_pid == -1) {
        perror("fork");
        return -1;
    }

    if (child_pid == 0) {
        // PIPE STUFF (CHILD)
        if (input_fd != STDIN_FILENO) {
            dup2(input_fd, STDIN_FILENO);
            close(input_fd);
        }

        if (downstream_exists) {
            dup2(pipefd[1], STDOUT_FILENO);
            close(pipefd[1]);
        }

        // TERMINAL CONTROL STUFF (CHILD)
        // child
        setpgid(0, pgid); /* This will be zero for the leader, or the correct value for other processes in the job */

        if (is_leader && !job->background) { // This is the first process in the job and it's not a background job
            Tcsetpgrp(STDIN_FILENO, getpid());
        }

        // EXEC
        execvp(p->args[0], p->args);
    }

    // PIPE STUFF (PARENT)
    if (downstream_exists) {
        if (input_fd) (pipefd[0]); // Only close if input_fd ISN'T stdin (0)
        close(pipefd[1]);
    }

    // TERMINAL CONTROL STUFF (PARENT)
    if (setpgid(child_pid, pgid ? pgid : child_pid) == -1 && errno != EACCES) {
        perror("setpgid - parent");
    }
    if (is_leader && !job->background) { // This is the first process in the job
        Tcsetpgrp(STDIN_FILENO, child_pid);
    }

    if (!pgid) { // This is the first process in the job
        job->pgid = child_pid;
        job->state = JOB_STATE_RUNNING;
    }

    p->pid = child_pid;
    
    if (!downstream_exists) { 
        return child_pid;
    }
    
    return fork_children(job, ++idx, pgid ? pgid : child_pid, pipefd[0]);
}

enum ExecuteStatus execute(struct Job *j) {
    struct Process *p = j->pipeline[0];

    if ((strcmp(BUILTIN_EXIT, p->args[0])) == 0 || (strcmp(BUILTIN_LOGOUT, p->args[0])) == 0) {
        builtin_exit();
    }

    if ((strcmp(BUILTIN_JOBS, p->args[0])) == 0) {
        builtin_jobs();
        return EXECUTE_BUILTIN_JOBS;
    }

    if ((strcmp(BUILTIN_BG, p->args[0])) == 0) {
        builtin_bg(atoi(p->args[1]));
        return EXECUTE_BUILTIN_BG;
    }

    if ((strcmp(BUILTIN_FG, p->args[0])) == 0) {
        builtin_fg(atoi(p->args[1]));
        return EXECUTE_BUILTIN_FG;
    }

    j->last_child_pid = fork_children(j, 0, 0, 0);

    return j->background ? EXECUTE_EXTERNAL_BG : EXECUTE_EXTERNAL_FG;
}