#ifndef BACKGROUND_H
#define BACKGROUND_H

#include "job.h"

struct Job *find_background_job_by_pgid(pid_t);
struct Job *find_background_job_by_last_child_pid(pid_t);
struct Job *find_background_job_by_jid(int);
void remove_background_job(struct Job *);
void add_job_to_background(struct Job *);
void each_background_job(void f(struct Job *));

void manage_background_job_state();

#endif // BACKGROUND_H