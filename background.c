#include <stdlib.h>
#include <unistd.h>
#include "background.h"
#include "job.h"

#define MAX_BACKGROUND_JOB_COUNT 16

static struct Job *jobs[MAX_BACKGROUND_JOB_COUNT] = { NULL };
static int count = 0;
static int next_id = 1;

struct Job *find_background_job_by_pgid(pid_t pgid) {
    for(int i = 0; i < MAX_BACKGROUND_JOB_COUNT; i++) {
        if (jobs[i] && jobs[i]->pgid == pgid) {
            return jobs[i];
        }
    }

    return NULL;
}

struct Job *find_background_job_by_last_child_pid(pid_t pid) {
    for(int i = 0; i < MAX_BACKGROUND_JOB_COUNT; i++) {
        if (jobs[i] && jobs[i]->last_child_pid == pid) {
            return jobs[i];
        }
    }

    return NULL;
}

struct Job *find_background_job_by_jid(int jid) {
    for(int i = 0; i < MAX_BACKGROUND_JOB_COUNT; i++) {
        if (jobs[i] && jobs[i]->id == jid) {
            return jobs[i];
        }
    }

    return NULL;
}

void remove_background_job(struct Job *j) {
    for(int i = 0; i < MAX_BACKGROUND_JOB_COUNT; i++) {
        if (jobs[i] == j) {
            jobs[i] = NULL;
            count--;
            if (!count) {
                next_id = 1;
            }
            return;
        }
    }
}

void add_job_to_background(struct Job *j) {
    j->id = next_id++;
    count++;

    for(int i = 0; i < MAX_BACKGROUND_JOB_COUNT; i++) {
        if (!jobs[i]) {
            jobs[i] = j;
            break;
        }
    }
}

void each_background_job(void f(struct Job *)) {
    for(int i = 0; i < MAX_BACKGROUND_JOB_COUNT; i++) {
        if (jobs[i]) f(jobs[i]);
    }
}

void manage_background_job_state() {
    pid_t pid;
    int status;
    struct Job *j;

    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED | WCONTINUED )) > 0) {
        if (WIFEXITED(status) || WIFSIGNALED(status)) { 
            j = find_background_job_by_last_child_pid(pid);
            if (j) j->state = JOB_STATE_TERMINATED;
            continue;
        }

        if (WIFSTOPPED(status)) {
            j = find_background_job_by_pgid(getpgid(pid));
            if (j) j->state = JOB_STATE_STOPPED;
            continue;
        }

        if (WIFCONTINUED(status)) {
            j = find_background_job_by_pgid(getpgid(pid));
            if (j) j->state = JOB_STATE_RUNNING;
        }
    }
}
