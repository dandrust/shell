#ifndef BUILTIN_H
#define BUILTIN_H

#include <sys/types.h>

#define BUILTIN_EXIT "exit"
#define BUILTIN_LOGOUT "logout"
#define BUILTIN_JOBS "jobs"
#define BUILTIN_BG "bg"
#define BUILTIN_FG "fg"

void builtin_jobs();
void builtin_exit();
void builtin_bg(int job_id);
void builtin_fg(int job_id);
void Tcsetpgrp(int fd, pid_t pid); // TODO: MOVE THIS ELSEWHERE

#endif // BUILTIN_H