#ifndef JOB_H
#define JOB_H

#include <wait.h>

#define JOB__MAX_PROCESSES_PER_PIPELINE 16

enum JobState {
    JOB_STATE_PENDING,
    JOB_STATE_RUNNING,
    JOB_STATE_STOPPED,
    JOB_STATE_TERMINATED
};

struct Job {
    int id;
    pid_t pgid;
    pid_t last_child_pid;
    int pcount;
    struct Process *pipeline[JOB__MAX_PROCESSES_PER_PIPELINE];
    enum JobState state;
    char cmd[1024];
    int background;
};

struct Job *newJob();
void cleanupJob(struct Job *);
void printJob(struct Job *);
void buildJobCmd(struct Job *);

#endif // JOB_H