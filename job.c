#include "job.h"
#include "process.h"

#include <stdlib.h>
#include <stdio.h>

struct Job *newJob() {
    struct Job *j = malloc(sizeof(struct Job));
    *j = (struct Job){ .state = JOB_STATE_PENDING };
    return j;
}

void cleanupJob(struct Job *j) {
    for (int p = 0; p < j->pcount; p++) {
        free(j->pipeline[p]);
    }

    free(j);
}

void buildJobCmd(struct Job *j) {
    if (j->cmd[0]) return;

    int n;
    char *start = j->cmd;

    for (int x = 0; x < j->pcount; x++) {
        if (x > 0) {
            n = sprintf(start, " |");
            start += n;
        }

        struct Process *p = j->pipeline[x];

        for (int y = 0; p->args[y] != NULL; y++) {
            n = sprintf(start, " %s", p->args[y]);
            start += n;
        }
    }

    if (j->background) {
        sprintf(start, " &");
    }
}

void printJob(struct Job *j) {
    char *state;

    switch (j->state) {
        case JOB_STATE_PENDING:
            state = "pending";
            break;
        case JOB_STATE_RUNNING:
            state = "running";
            break;
        case JOB_STATE_STOPPED:
            state = "stopped";
            break;
        case JOB_STATE_TERMINATED:
            state = "terminated";
            break;
        default:
            state = "unknown";
    }

    buildJobCmd(j);

    printf("[%d]\t%s\t\t\t%s\n", j->id, state, j->cmd);
}
