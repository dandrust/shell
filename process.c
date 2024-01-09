#include "process.h"
#include <stdlib.h>

struct Process *newProcess() {
    struct Process *p = malloc(sizeof(struct Process));
    *p = (struct Process){ 0 };

    return p;
}
void cleanupProcess(struct Process *p) {
    free(p);
}
