#ifndef LOGGER_H
#define LOGGER_H

void log_state_change(int pid, const char *old_state, const char *new_state);
void show_logs(void);

#endif