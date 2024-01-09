#include "parse.h"
#include "process.h"
#include "job.h"
#include <stdlib.h>
#include <string.h>

void parse_command(char *buf, struct Job *j) {
    int job_no = 0;
    int arg_no = 0;

    struct Process *p = newProcess();

    p->args[arg_no] = strtok(buf, " \t\n");
    char *last_token = p->args[arg_no];

    while (last_token != NULL) { // until we reach end of string
        p->args[++arg_no] = strtok(NULL, " \t\n"); // get the next token
        last_token = p->args[arg_no];

        if (p->args[arg_no] != NULL && strcmp(TOKEN_PIPE, p->args[arg_no]) == 0) { // if the next token is a pipe
            p->args[arg_no] = NULL; //      write NULL to args[i]
            
            // Add job to the pipeline
            j->pipeline[job_no] = p;

            // Pointer accounting (point to next job)
            job_no++;   //      increment the job index
            p = newProcess();
            arg_no = -1;   //      set arg index to zero
        }

        if (p->args[arg_no] != NULL && strcmp(TOKEN_AMP, p->args[arg_no]) == 0) { // if the next token is an `&`
            // If we find `&` assume that it's the last token and get out
            p->args[arg_no] = NULL;
            j->background = 1;
            break;
        }
    }

    j->pipeline[job_no] = p;
    j->pcount = job_no + 1;
}