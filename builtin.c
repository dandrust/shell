#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "builtin.h"
#include "job.h"
#include "background.h"

extern struct Job *foreground_job;

// TODO: MOVE THIS TO IT'S OWN MODULE
void Tcsetpgrp(int fd, pid_t pid) {
    struct sigaction default_action;
    struct sigaction ignore_action;
    ignore_action.sa_handler = SIG_IGN;
    default_action.sa_handler = SIG_DFL;
    
    sigaction(SIGTTOU, &ignore_action, NULL);           /* Ignore SIGTTOU */

    if (tcsetpgrp(fd, pid) == -1) {
        perror("tcsetpgrp");
        _exit(1);
    }
    sigaction(SIGTTOU, &default_action, NULL);          /* Reinstate default SIGTTOU disposition */
}

void print_and_prune_job(struct Job *j) {
    printJob(j);

    if (j->state == JOB_STATE_TERMINATED) {
        cleanupJob(j);
        remove_background_job(j);
    }
}

void builtin_jobs() {
    each_background_job(print_and_prune_job);
}

void kill_and_cleanup_job(struct Job *j) {
    kill(j->pgid * -1, SIGINT);
    cleanupJob(j);
}

void builtin_exit() {
    each_background_job(kill_and_cleanup_job);
    cleanupJob(foreground_job);
    exit(0);
}

void continue_job(struct Job *j) {
    if (!j) return;

    kill(j->pgid * -1, SIGCONT);
}

void builtin_bg(int job_id) {
    struct Job *j = find_background_job_by_jid(job_id);
    if (!j) return;

    continue_job(j);
}

void builtin_fg(int job_id) {
    // Find job
    struct Job *j = find_background_job_by_jid(job_id);
    if (!j) return;

    // Give job control of the terminal
    Tcsetpgrp(STDIN_FILENO, j->pgid);
    continue_job(j);

    // Set job as foreground job
    cleanupJob(foreground_job);
    remove_background_job(j);
    foreground_job = j;
}
