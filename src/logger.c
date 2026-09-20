#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "logger.h"


/* -------------------------------------------------
 * Log a message to raktsetu.log
 * ------------------------------------------------- */

static void write_log(const char *message)
{
    int fd;

    /*
     * O_WRONLY  -> open file for writing
     * O_CREAT   -> create file if it doesn't exist
     * O_APPEND  -> add new data at the end
     */

    fd = open("raktsetu.log",
              O_WRONLY | O_CREAT | O_APPEND,
              0644);

    if (fd == -1) {
        perror("Unable to open log file");
        return;
    }

    write(fd, message, strlen(message));
    write(fd, "\n", 1);

    close(fd);
}


/* -------------------------------------------------
 * Log process state transition
 * ------------------------------------------------- */

void log_state_change(int pid,
                      const char *old_state,
                      const char *new_state)
{
    char message[200];

    snprintf(message,
             sizeof(message),
             "[P%d] %s -> %s",
             pid,
             old_state,
             new_state);

    write_log(message);
}


/* -------------------------------------------------
 * Display complete log
 * ------------------------------------------------- */

void show_logs(void)
{
    FILE *file;
    char line[256];

    file = fopen("raktsetu.log", "r");

    if (file == NULL) {
        printf("\nNo logs available yet.\n");
        return;
    }

    printf("\n");
    printf("========== RAKTSETU OS LOG ==========\n");

    while (fgets(line, sizeof(line), file) != NULL) {
        printf("%s", line);
    }

    printf("======================================\n");

    fclose(file);
}