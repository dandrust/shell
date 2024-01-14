A toy shell implementation to explore `fork`/`exec`, processes, signals, pipes, and job control.

To compile:
```
gcc process.c job.c parse.c background.c execute.c builtin.c main.c
```

The shell includes a basic REPL that forks and execs the program provided at the prompt, supports pipes, and supports job control.

An infinite loop that parses command line input (implemented in `parse.c`) and executes (implemented in `execute.c`) lays the foundation of the shell in `main.c`'s `main` function.  Builtin functions are implemented in `builtin.c`, and code related to job control and background job tracking is included in `background.c`.  At it's core, the shell operates on processes - individual executables to be `exec`'d -- and jobs (which could inlcude a pipeline of processes).  Those are implemented in `process.c` and `job.c`, respectively