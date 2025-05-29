typedef enum {
    ADD_JOB,
    UPDATE_JOB,
    REMOVE_JOB,
    LIST_JOBS,
} Job_Action;

int buildin_handler(char *buffer, int *last_status, char *last_command, char *expanded);
void job_handler(Job_Action action, pid_t pid, char *command, char *status);