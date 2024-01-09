#ifndef EXECUTE_H
#define EXECUTE_H

#include <unistd.h>
#include "job.h"

enum ExecuteStatus {
    EXECUTE_ERROR,
    EXECUTE_BUILTIN_FG,
    EXECUTE_BUILTIN_BG,
    EXECUTE_BUILTIN_JOBS,
    EXECUTE_EXTERNAL_FG,
    EXECUTE_EXTERNAL_BG
};

enum ExecuteStatus execute(struct Job *);
void Tcsetpgrp(int fd, pid_t pid); // TODO: Move this! Should be defined in some other module

#endif // EXECUTE_H