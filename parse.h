#ifndef PARSE_H
#define PARSE_H

#include "builtin.h"
#include "job.h"

#define TOKEN_EXIT "exit"
#define TOKEN_LOGOUT "logout"
#define TOKEN_JOBS "jobs"
#define TOKEN_BG "bg"
#define TOKEN_FG "fg"
#define TOKEN_PIPE "|"
#define TOKEN_AMP "&"

void parse_command(char *, struct Job *);

#endif // PARSE_H