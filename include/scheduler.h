<<<<<<< Updated upstream
/* scheduler.h - CPU scheduler interface
=======
/* scheduler.h - CPU scheduling (Unit 2)
 * Owner: Aditya Upadhyay
>>>>>>> Stashed changes
 */
#ifndef SCHEDULER_H
#define SCHEDULER_H

<<<<<<< Updated upstream
typedef enum { SCHED_FCFS, SCHED_PRIORITY } SchedAlgo;
=======
typedef enum {
    SCHED_FCFS,
    SCHED_SJF,
    SCHED_SRTF,
    SCHED_PRIORITY,     /* non-preemptive */
    SCHED_PRIORITY_P,   /* preemptive     */
    SCHED_RR,
    SCHED_MLQ,
    SCHED_MLFQ,
    SCHED_COUNT
} SchedAlgo;

/* per-run statistics (Unit 2: scheduling criteria) */
typedef struct {
    double avg_wt;
    double avg_tat;
    double avg_rt;
    int    completed;
    int    switches;      /* context switches                 */
    int    idle_ticks;    /* ticks with no process on the CPU */
    int    total_ticks;   /* makespan                         */
    double cpu_util;      /* percent                          */
    double throughput;    /* processes completed per 10 ticks */
} SchedStats;
>>>>>>> Stashed changes

void        scheduler_set(SchedAlgo algo);
SchedAlgo   scheduler_get(void);
const char *scheduler_name(SchedAlgo algo);
<<<<<<< Updated upstream

/* Parse "fcfs" / "priority" (case-insensitive). Returns 0 on success, -1 if unknown. */
int scheduler_parse(const char *text, SchedAlgo *out);

/* Execute all READY processes with the selected algorithm.
 *   commit = 1 : real run - allocates blood, changes process states
 *   commit = 0 : preview  - works on copies, nothing is changed,
 *                           resource check skipped (for comparing algorithms)
 * Returns number of processes executed. */
int scheduler_run(int commit);
=======
int         scheduler_parse(const char *text, SchedAlgo *out);
void        scheduler_set_quantum(int q);
int         scheduler_get_quantum(void);

/* commit = 1 : real run (allocates blood, changes states)
 * commit = 0 : preview (works on copies, nothing changes)       */
int  scheduler_run(int commit);

/* run every algorithm on the current READY set and print the table */
void scheduler_compare(void);

/* preview one algorithm: Gantt chart and per-process table, nothing changed */
void scheduler_gantt(SchedAlgo algo);

/* generate a larger synthetic workload for the statistics study */
void scheduler_workload(int n, unsigned seed);

/* write the comparison results as CSV for the report charts */
int  scheduler_export(const char *path);
>>>>>>> Stashed changes

#endif
