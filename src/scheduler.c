<<<<<<< Updated upstream
/* scheduler.c - CPU Scheduler for RaktSetu Mini OS Prototype
 * Owner: Aditya Upadhyay (Group 2)
 *
 * Algorithms (both non-preemptive in this prototype):
 *   FCFS     : earliest arrival first, ties by PID
 *   PRIORITY : CRITICAL > URGENT > NORMAL, ties by arrival, then PID
 *
 * At every decision point the scheduler only considers processes that
 * have already arrived. If none has arrived, the CPU stays IDLE until
 * the next arrival.
 *
 * Output: ASCII Gantt chart, per-process table (AT, BT, ST, CT, WT, TAT)
 *         and average waiting / turnaround time.
 */
#include <stdio.h>
=======
/* scheduler.c - CPU scheduling (Unit 2)
 * Owner: Aditya Upadhyay (Group 2)
 *
 * Algorithms
 *   FCFS      non-preemptive, earliest arrival first
 *   SJF       non-preemptive, shortest burst first
 *   SRTF      preemptive, shortest remaining time first
 *   PRIORITY  non-preemptive, CRITICAL > URGENT > NORMAL
 *   PRIORITY* preemptive, a critical request interrupts a running one
 *   RR        round robin with a configurable time quantum
 *   MLQ       Q0 CRITICAL (priority), Q1 URGENT (RR), Q2 NORMAL (FCFS), with aging
 *   MLFQ      three feedback levels; demote on quantum expiry, promote on aging
 *
 * The whole simulator is one tick-driven loop. A policy decides which
 * process holds the CPU during a tick; a timeline records the holder of
 * every tick and is compressed into a Gantt chart afterwards.
 */
#include <stdio.h>
#include <string.h>
>>>>>>> Stashed changes
#include <ctype.h>
#include "pcb.h"
#include "blood.h"
#include "scheduler.h"

<<<<<<< Updated upstream
#define MAX_SLOTS (MAX_PROCESSES * 2)

typedef struct { int pid; int start; int end; } GanttSlot;   /* pid 0 = IDLE */

static SchedAlgo current_algo = SCHED_FCFS;

/* ---------- configuration ---------- */

void scheduler_set(SchedAlgo algo) {
    current_algo = algo;
    printf("Scheduler: %s\n", scheduler_name(algo));
}

SchedAlgo scheduler_get(void) { return current_algo; }

const char *scheduler_name(SchedAlgo algo) {
    return algo == SCHED_PRIORITY ? "PRIORITY" : "FCFS";
}

static int equals_ignore_case(const char *a, const char *b) {
=======
#define MAX_TICKS 4096
#define LEVELS    3
#define AGE_LIMIT 12          /* ticks a process may wait before promotion */

static SchedAlgo current_algo = SCHED_FCFS;
static int quantum = 2;

/* ---------------- configuration ---------------- */

const char *scheduler_name(SchedAlgo a)
{
    switch (a) {
        case SCHED_FCFS:       return "FCFS";
        case SCHED_SJF:        return "SJF";
        case SCHED_SRTF:       return "SRTF";
        case SCHED_PRIORITY:   return "PRIORITY";
        case SCHED_PRIORITY_P: return "PRIORITY-P";
        case SCHED_RR:         return "RR";
        case SCHED_MLQ:        return "MLQ";
        case SCHED_MLFQ:       return "MLFQ";
        default:               return "?";
    }
}

void scheduler_set(SchedAlgo a)
{
    current_algo = a;
    if (a == SCHED_RR) printf("Scheduler: RR (quantum = %d)\n", quantum);
    else               printf("Scheduler: %s\n", scheduler_name(a));
}

SchedAlgo scheduler_get(void)      { return current_algo; }
void scheduler_set_quantum(int q)  { if (q > 0) quantum = q; }
int  scheduler_get_quantum(void)   { return quantum; }

static int ieq(const char *a, const char *b)
{
>>>>>>> Stashed changes
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) return 0;
        a++; b++;
    }
    return *a == *b;
}

<<<<<<< Updated upstream
int scheduler_parse(const char *text, SchedAlgo *out) {
    if (!text) return -1;
    if (equals_ignore_case(text, "fcfs"))     { *out = SCHED_FCFS;     return 0; }
    if (equals_ignore_case(text, "priority")) { *out = SCHED_PRIORITY; return 0; }
    return -1;
}

/* ---------- selection ---------- */

/* returns 1 if a should run before b */
static int runs_before(const PCB *a, const PCB *b, SchedAlgo algo) {
    if (algo == SCHED_PRIORITY && a->priority != b->priority)
        return a->priority < b->priority;
    if (a->arrival_time != b->arrival_time)
        return a->arrival_time < b->arrival_time;
    return a->pid < b->pid;
}

/* ---------- output ---------- */

static void print_gantt(const GanttSlot *s, int n) {
    int i;
    printf("\nGantt Chart:\n");
    for (i = 0; i < n; i++) printf("+-------");
    printf("+\n");
    for (i = 0; i < n; i++) {
        if (s[i].pid == 0) printf("| IDLE  ");
        else               printf("| P%-5d", s[i].pid);
    }
    printf("|\n");
    for (i = 0; i < n; i++) printf("+-------");
    printf("+\n");
    for (i = 0; i < n; i++) printf("%-8d", s[i].start);
    printf("%d\n", s[n - 1].end);
}

static void print_table(PCB **done, int n) {
    int i;
    double total_wt = 0, total_tat = 0;
    printf("\nPID   Patient      Group  Units  Priority   AT   BT   ST   CT   WT   TAT\n");
    printf("--------------------------------------------------------------------------\n");
    for (i = 0; i < n; i++) {
        PCB *p = done[i];
        printf("%-5d %-12s %-6s %-6d %-9s %4d %4d %4d %4d %4d %5d\n",
               p->pid, p->patient, p->blood_group, p->units, priority_name(p->priority),
               p->arrival_time, p->burst_time, p->start_time, p->completion_time,
               p->waiting_time, p->turnaround_time);
        total_wt  += p->waiting_time;
        total_tat += p->turnaround_time;
    }
    printf("--------------------------------------------------------------------------\n");
    printf("Average Waiting Time    : %.2f\n", total_wt / n);
    printf("Average Turnaround Time : %.2f\n", total_tat / n);
}

/* ---------- execution ---------- */

int scheduler_run(int commit) {
    PCB  copies[MAX_PROCESSES];
    PCB *cand[MAX_PROCESSES];
    PCB *done[MAX_PROCESSES];
    int  picked[MAX_PROCESSES] = {0};
    GanttSlot slots[MAX_SLOTS];
    int n = 0, n_done = 0, n_slots = 0, n_waiting = 0;
    int i, left, clock;

    /* collect READY processes */
    for (i = 0; i < process_count; i++) {
        if (process_table[i].state != READY) continue;
        if (commit) {
            cand[n] = &process_table[i];
        } else {
            copies[n] = process_table[i];
            cand[n] = &copies[n];
        }
        n++;
    }

    printf("\n========== Scheduler: %s %s==========\n",
           scheduler_name(current_algo), commit ? "" : "(preview) ");
    if (n == 0) {
        printf("No READY processes to schedule.\n");
        return 0;
    }
    if (!commit)
        printf("Preview mode: blood check skipped, process states unchanged.\n");

    /* CPU clock starts at the earliest arrival */
    clock = cand[0]->arrival_time;
    for (i = 1; i < n; i++)
        if (cand[i]->arrival_time < clock) clock = cand[i]->arrival_time;

    left = n;
    while (left > 0) {
        int best = -1;
        PCB *p;

        /* pick best among arrived, not yet picked */
        for (i = 0; i < n; i++) {
            if (picked[i] || cand[i]->arrival_time > clock) continue;
            if (best < 0 || runs_before(cand[i], cand[best], current_algo)) best = i;
        }

        /* nothing arrived yet -> CPU idle until next arrival */
        if (best < 0) {
            int next = -1;
            for (i = 0; i < n; i++)
                if (!picked[i] && (next < 0 || cand[i]->arrival_time < next))
                    next = cand[i]->arrival_time;
            slots[n_slots].pid = 0;
            slots[n_slots].start = clock;
            slots[n_slots].end = next;
            n_slots++;
            clock = next;
            continue;
        }

        p = cand[best];
        picked[best] = 1;
        left--;

        if (commit) {
=======
int scheduler_parse(const char *t, SchedAlgo *out)
{
    if (!t) return -1;
    if (ieq(t, "fcfs"))      { *out = SCHED_FCFS;       return 0; }
    if (ieq(t, "sjf"))       { *out = SCHED_SJF;        return 0; }
    if (ieq(t, "srtf"))      { *out = SCHED_SRTF;       return 0; }
    if (ieq(t, "priority"))  { *out = SCHED_PRIORITY;   return 0; }
    if (ieq(t, "priorityp") || ieq(t, "ppriority") || ieq(t, "prio-p"))
                             { *out = SCHED_PRIORITY_P; return 0; }
    if (ieq(t, "rr"))        { *out = SCHED_RR;         return 0; }
    if (ieq(t, "mlq"))       { *out = SCHED_MLQ;        return 0; }
    if (ieq(t, "mlfq"))      { *out = SCHED_MLFQ;       return 0; }
    return -1;
}

/* ---------------- simulation state ---------------- */

typedef struct {
    PCB *p;
    int  rem;          /* remaining time                      */
    int  queued;       /* already placed in a queue           */
    int  done;
    int  level;        /* MLQ / MLFQ level                    */
    int  wait_ticks;   /* consecutive ticks spent waiting     */
} Task;

static Task task[MAX_PROCESSES];
static int  ntask;

static int queue[LEVELS][MAX_PROCESSES * 4];
static int qhead[LEVELS], qtail[LEVELS];

static void q_reset(void)
{
    int i;
    for (i = 0; i < LEVELS; i++) qhead[i] = qtail[i] = 0;
}
static void q_push(int lvl, int t) { queue[lvl][qtail[lvl]++] = t; }
static int  q_empty(int lvl)       { return qhead[lvl] == qtail[lvl]; }
static int  q_pop(int lvl)         { return queue[lvl][qhead[lvl]++]; }

static int uses_queues(SchedAlgo a)
{
    return a == SCHED_RR || a == SCHED_MLQ || a == SCHED_MLFQ;
}

static int is_preemptive(SchedAlgo a)
{
    return a == SCHED_SRTF || a == SCHED_PRIORITY_P;
}

/* non-queue policies: returns the task index that should hold the CPU,
 * or -1 when nothing has arrived yet                                   */
static int pick_scan(SchedAlgo a, int clock)
{
    int i, best = -1;

    for (i = 0; i < ntask; i++) {
        Task *t = &task[i];
        if (t->done || t->p->arrival_time > clock) continue;
        if (best < 0) { best = i; continue; }

        switch (a) {
            case SCHED_SJF:
                if (t->rem < task[best].rem) best = i;
                else if (t->rem == task[best].rem &&
                         t->p->arrival_time < task[best].p->arrival_time) best = i;
                break;
            case SCHED_SRTF:
                if (t->rem < task[best].rem) best = i;
                else if (t->rem == task[best].rem &&
                         t->p->arrival_time < task[best].p->arrival_time) best = i;
                break;
            case SCHED_PRIORITY:
            case SCHED_PRIORITY_P:
                if (t->p->priority < task[best].p->priority) best = i;
                else if (t->p->priority == task[best].p->priority &&
                         t->p->arrival_time < task[best].p->arrival_time) best = i;
                break;
            default: /* FCFS */
                if (t->p->arrival_time < task[best].p->arrival_time) best = i;
                break;
        }
    }
    return best;
}

/* ---------------- Gantt chart ---------------- */

static void print_gantt(const int *timeline, int offset, int end)
{
    int i, n = 0, k;
    int spid[MAX_TICKS], sstart[MAX_TICKS], send[MAX_TICKS];

    for (i = 0; i < end; i++) {
        if (n > 0 && spid[n - 1] == timeline[i]) { send[n - 1] = offset + i + 1; continue; }
        spid[n] = timeline[i]; sstart[n] = offset + i; send[n] = offset + i + 1; n++;
    }
    if (n == 0) return;

    printf("\nGantt Chart:\n");
    for (k = 0; k < n; k++) printf("+-------");
    printf("+\n");
    for (k = 0; k < n; k++) {
        if (spid[k] == 0) printf("| IDLE  ");
        else              printf("| P%-5d", spid[k]);
    }
    printf("|\n");
    for (k = 0; k < n; k++) printf("+-------");
    printf("+\n");
    for (k = 0; k < n; k++) printf("%-8d", sstart[k]);
    printf("%d\n", send[n - 1]);
}

static void print_table(Task *done[], int n)
{
    int i;
    double wt = 0, tat = 0, rt = 0;

    printf("\nPID   Patient      Group  Units  Priority    AT   BT   CT   WT  TAT   RT\n");
    printf("------------------------------------------------------------------------\n");
    for (i = 0; i < n; i++) {
        PCB *p = done[i]->p;
        printf("%-5d %-12s %-6s %-6d %-10s %4d %4d %4d %4d %4d %4d\n",
               p->pid, p->patient, p->blood_group, p->units,
               priority_name(p->priority), p->arrival_time, p->burst_time,
               p->completion_time, p->waiting_time, p->turnaround_time,
               p->response_time);
        wt += p->waiting_time; tat += p->turnaround_time; rt += p->response_time;
    }
    printf("------------------------------------------------------------------------\n");
    printf("Average Waiting Time    : %.2f\n", wt / n);
    printf("Average Turnaround Time : %.2f\n", tat / n);
    printf("Average Response Time   : %.2f\n", rt / n);
}

/* ---------------- the simulator ---------------- */

/* Runs the current task set under the given algorithm.
 * Returns the number of processes completed and fills st with the
 * scheduling criteria: waiting, turnaround and response time, context
 * switches, CPU utilisation and throughput.                          */
static int simulate(SchedAlgo algo, int verbose, SchedStats *st)
{
    int timeline[MAX_TICKS];
    Task *done[MAX_PROCESSES];
    int ndone = 0;
    int clock, start_clock, remaining = ntask;
    int cur = -1, prev = -1, qleft = 0;
    int switches = 0, idle = 0;
    int i;

    memset(st, 0, sizeof *st);
    if (ntask == 0) return 0;

    q_reset();
    start_clock = task[0].p->arrival_time;
    for (i = 1; i < ntask; i++)
        if (task[i].p->arrival_time < start_clock) start_clock = task[i].p->arrival_time;

    for (i = 0; i < ntask; i++) {
        task[i].rem        = task[i].p->burst_time;
        task[i].done       = 0;
        task[i].queued     = 0;
        task[i].wait_ticks = 0;
        task[i].level      = (algo == SCHED_MLFQ) ? 0 : task[i].p->queue_level;
        task[i].p->response_time = -1;
        task[i].p->started       = 0;
    }

    clock = start_clock;
    while (remaining > 0 && clock - start_clock < MAX_TICKS - 1) {

        /* 1. admit arrivals */
        if (uses_queues(algo)) {
            for (i = 0; i < ntask; i++)
                if (!task[i].queued && !task[i].done &&
                    task[i].p->arrival_time <= clock) {
                    task[i].queued = 1;
                    q_push(algo == SCHED_RR ? 0 : task[i].level, i);
                }
        }

        /* 2. aging: promote anything that has waited too long */
        if (algo == SCHED_MLQ || algo == SCHED_MLFQ) {
            for (i = 0; i < ntask; i++) {
                Task *t = &task[i];
                if (t->done || i == cur || !t->queued) continue;
                if (t->wait_ticks >= AGE_LIMIT && t->level > 0) {
                    t->level--;
                    t->wait_ticks = 0;
                    if (verbose)
                        printf("        [aging] P%d promoted to queue %d\n",
                               t->p->pid, t->level);
                }
            }
        }

        /* 3. choose who holds the CPU this tick */
        if (cur >= 0 && task[cur].done) cur = -1;

        if (uses_queues(algo)) {
            int lvl, preempt = 0;

            if (cur >= 0 && algo != SCHED_RR) {
                /* a higher queue preempts a lower one */
                for (lvl = 0; lvl < task[cur].level; lvl++)
                    if (!q_empty(lvl)) { preempt = 1; break; }
            }
            if (cur >= 0 && (qleft == 0 || preempt)) {
                if (algo == SCHED_MLFQ && qleft == 0 &&
                    task[cur].level < LEVELS - 1)
                    task[cur].level++;              /* used its full quantum */
                task[cur].queued = 1;
                q_push(task[cur].level, cur);
                cur = -1;
            }
            if (cur < 0) {
                for (lvl = 0; lvl < LEVELS; lvl++) {
                    while (!q_empty(lvl)) {
                        int c = q_pop(lvl);
                        if (task[c].done) continue;
                        cur = c;
                        break;
                    }
                    if (cur >= 0) break;
                }
                if (cur >= 0) {
                    int lv = task[cur].level;
                    qleft = (algo == SCHED_RR) ? quantum
                          : (algo == SCHED_MLFQ) ? quantum * (1 << lv)
                          : (lv == 1 ? quantum : 1 << 20);   /* MLQ: Q1 is RR */
                    task[cur].wait_ticks = 0;
                }
            }
        } else {
            if (cur < 0 || is_preemptive(algo)) {
                int c = pick_scan(algo, clock);
                if (c >= 0 && c != cur) {
                    if (verbose && cur >= 0 && !task[cur].done)
                        printf("        [preempt] P%d -> P%d at t=%d\n",
                               task[cur].p->pid, task[c].p->pid, clock);
                    cur = c;
                }
            }
        }

        /* 4. run one tick */
        if (cur != prev && cur >= 0 && prev >= 0) switches++;
        prev = cur;

        if (cur < 0) {
            timeline[clock - start_clock] = 0;              /* CPU idle */
            idle++;
        } else {
            PCB *p = task[cur].p;

            if (!p->started) {
                p->started = 1;
                p->response_time = clock - p->arrival_time;
            }
            timeline[clock - start_clock] = p->pid;
            task[cur].rem--;
            if (qleft > 0) qleft--;

            if (task[cur].rem == 0) {
                task[cur].done = 1;
                remaining--;
                p->completion_time = clock + 1;
                p->turnaround_time = p->completion_time - p->arrival_time;
                p->waiting_time    = p->turnaround_time - p->burst_time;
                done[ndone++] = &task[cur];
                cur = -1;
            }
        }

        /* 5. everyone else waited one more tick */
        for (i = 0; i < ntask; i++)
            if (!task[i].done && i != cur && task[i].p->arrival_time <= clock)
                task[i].wait_ticks++;

        clock++;
    }

    st->completed   = ndone;
    st->switches    = switches;
    st->idle_ticks  = idle;
    st->total_ticks = clock - start_clock;

    if (ndone > 0) {
        double w = 0, t = 0, r = 0;
        for (i = 0; i < ndone; i++) {
            w += done[i]->p->waiting_time;
            t += done[i]->p->turnaround_time;
            r += done[i]->p->response_time;
        }
        st->avg_wt = w / ndone; st->avg_tat = t / ndone; st->avg_rt = r / ndone;
    }
    if (st->total_ticks > 0) {
        st->cpu_util   = 100.0 * (st->total_ticks - idle) / st->total_ticks;
        st->throughput = 10.0 * ndone / st->total_ticks;
    }

    if (verbose) {
        printf("\nExecution Order: ");
        for (i = 0; i < ndone; i++)
            printf("P%d%s", done[i]->p->pid, i < ndone - 1 ? " -> " : "\n");
        print_gantt(timeline, start_clock, clock - start_clock);
        print_table(done, ndone);
        printf("Context Switches        : %d\n", st->switches);
        printf("CPU Utilisation         : %.1f%%  (%d idle tick(s))\n",
               st->cpu_util, st->idle_ticks);
        printf("Throughput              : %.2f request(s) per 10 ticks\n",
               st->throughput);
    }
    return ndone;
}

/* collect the READY processes into the task array */
static int collect(PCB *storage, int use_copies)
{
    int i;
    ntask = 0;
    for (i = 0; i < process_count; i++) {
        if (process_table[i].state != READY) continue;
        if (use_copies) {
            storage[ntask] = process_table[i];
            task[ntask].p  = &storage[ntask];
        } else {
            task[ntask].p  = &process_table[i];
        }
        ntask++;
    }
    return ntask;
}

int scheduler_run(int commit)
{
    static PCB copies[MAX_PROCESSES];
    SchedStats st;
    int n, i, blocked = 0;

    n = collect(copies, !commit);

    printf("\n========== Scheduler: %s%s ==========\n",
           scheduler_name(current_algo), commit ? "" : " (preview)");
    if (n == 0) { printf("No READY processes to schedule.\n"); return 0; }
    if (!commit)
        printf("Preview mode: no blood allocated, process states unchanged.\n");

    /* in a real run the resource is claimed before the process executes */
    if (commit) {
        int kept = 0;
        for (i = 0; i < n; i++) {
            PCB *p = task[i].p;
>>>>>>> Stashed changes
            set_state(p, RUNNING);
            if (!allocate_blood(p->blood_group, p->units)) {
                set_state(p, WAITING);
                printf("        Reason: RESOURCE_UNAVAILABLE (%s x %d units)\n",
                       p->blood_group, p->units);
<<<<<<< Updated upstream
                n_waiting++;
=======
                blocked++;
>>>>>>> Stashed changes
                continue;
            }
            printf("        %d unit(s) of %s allocated to P%d\n",
                   p->units, p->blood_group, p->pid);
<<<<<<< Updated upstream
        }

        p->start_time      = clock;
        clock             += p->burst_time;
        p->remaining_time  = 0;
        p->completion_time = clock;
        p->turnaround_time = p->completion_time - p->arrival_time;
        p->waiting_time    = p->start_time - p->arrival_time;

        slots[n_slots].pid = p->pid;
        slots[n_slots].start = p->start_time;
        slots[n_slots].end = p->completion_time;
        n_slots++;
        done[n_done++] = p;

        if (commit) set_state(p, TERMINATED);
    }

    if (n_done > 0) {
        printf("\nExecution Order: ");
        for (i = 0; i < n_done; i++)
            printf("P%d%s", done[i]->pid, i < n_done - 1 ? " -> " : "\n");
        print_gantt(slots, n_slots);
        print_table(done, n_done);
    } else {
        printf("No process could be executed.\n");
    }
    if (n_waiting > 0)
        printf("\n%d process(es) moved to WAITING due to blood shortage.\n", n_waiting);

    return n_done;
=======
            set_state(p, READY);
            task[kept++] = task[i];
        }
        ntask = kept;
        if (ntask == 0) {
            printf("\nNo process could run; every request is waiting for blood.\n");
            return 0;
        }
    }

    n = simulate(current_algo, 1, &st);

    if (commit)
        for (i = 0; i < ntask; i++)
            if (task[i].p->state == READY) {
                set_state(task[i].p, RUNNING);
                set_state(task[i].p, TERMINATED);
            }

    if (blocked > 0)
        printf("\n%d process(es) moved to WAITING due to blood shortage.\n", blocked);
    return n;
}

/* the algorithms compared, in syllabus order */
static const SchedAlgo compare_order[] = {
    SCHED_FCFS, SCHED_SJF, SCHED_SRTF, SCHED_PRIORITY,
    SCHED_PRIORITY_P, SCHED_RR, SCHED_MLQ, SCHED_MLFQ
};
#define NCOMPARE ((int)(sizeof compare_order / sizeof compare_order[0]))

/* results of the last compare, kept so that export can write them out */
static SchedStats last_stats[NCOMPARE];
static int        last_n;
static int        have_results = 0;

static void label_for(SchedAlgo a, char *buf, size_t n)
{
    if (a == SCHED_RR) snprintf(buf, n, "RR (q=%d)", quantum);
    else               snprintf(buf, n, "%s", scheduler_name(a));
}

void scheduler_compare(void)
{
    static PCB copies[MAX_PROCESSES];
    SchedStats st;
    double best_wt = 1e9, best_rt = 1e9, best_tp = -1, worst_tp = 1e9;
    const char *wt_name = "", *rt_name = "", *tp_name = "";
    char label[32];
    int k, n;

    n = collect(copies, 1);
    if (n == 0) {
        printf("\nNo READY processes. Load the sample set first ('sample').\n");
        return;
    }

    printf("\n===================== ALGORITHM COMPARISON =====================\n");
    printf("Sample set: %d requests, time quantum = %d\n", n, quantum);
    printf("WT = waiting, TAT = turnaround, RT = response, CS = context switches\n\n");
    printf("%-18s %7s %7s %7s %5s %7s %8s\n",
           "Algorithm", "Avg WT", "AvgTAT", "Avg RT", "CS", "CPU %", "Thrpt");
    printf("---------------------------------------------------------------\n");

    for (k = 0; k < NCOMPARE; k++) {
        collect(copies, 1);
        simulate(compare_order[k], 0, &st);
        last_stats[k] = st;

        label_for(compare_order[k], label, sizeof label);
        printf("%-18s %7.2f %7.2f %7.2f %5d %7.1f %8.2f\n",
               label, st.avg_wt, st.avg_tat, st.avg_rt,
               st.switches, st.cpu_util, st.throughput);

        if (st.avg_wt < best_wt) { best_wt = st.avg_wt; wt_name = scheduler_name(compare_order[k]); }
        if (st.avg_rt < best_rt) { best_rt = st.avg_rt; rt_name = scheduler_name(compare_order[k]); }
        if (st.throughput > best_tp) { best_tp = st.throughput; tp_name = scheduler_name(compare_order[k]); }
        if (st.throughput < worst_tp) worst_tp = st.throughput;
    }

    last_n = n;
    have_results = 1;

    printf("---------------------------------------------------------------\n");
    printf("Lowest average waiting time  : %s (%.2f)\n", wt_name, best_wt);
    printf("Best average response time   : %s (%.2f)\n", rt_name, best_rt);
    if (best_tp - worst_tp < 0.01)
        printf("Throughput                   : %.2f for every algorithm\n"
               "                               (same total work, no idle CPU, so the\n"
               "                               makespan cannot change)\n", best_tp);
    else
        printf("Highest throughput           : %s (%.2f per 10 ticks)\n", tp_name, best_tp);
    printf("Emergency served first       : PRIORITY, PRIORITY-P, MLQ\n");
    printf("Fewest context switches      : the non-preemptive algorithms\n");
    printf("\nUse 'gantt <algo>' for the chart of one algorithm, or\n");
    printf("'export results.csv' to save this table for the report.\n");
    printf("===============================================================\n");
}

/* Preview a single algorithm: Gantt chart and per-process table.
 * Nothing is allocated and no process state changes.               */
void scheduler_gantt(SchedAlgo algo)
{
    static PCB copies[MAX_PROCESSES];
    SchedStats st;
    char label[32];
    int n;

    n = collect(copies, 1);
    if (n == 0) {
        printf("\nNo READY processes. Load the sample set first ('sample').\n");
        return;
    }

    label_for(algo, label, sizeof label);
    printf("\n========== %s (preview) ==========\n", label);
    printf("%d request(s); no blood allocated, process states unchanged.\n", n);
    simulate(algo, 1, &st);
    printf("=============================================\n");
}

/* A larger synthetic workload, so the comparison can be repeated on
 * 20, 50 or 100 requests instead of the 7-request demonstration set. */
void scheduler_workload(int n, unsigned seed)
{
    static const char *groups[] = { "A+","A-","B+","B-","AB+","AB-","O+","O-" };
    unsigned r = seed ? seed : 12345u;
    int i, arrival = 0;
    char name[16];

    if (n < 1)   n = 20;
    if (n > MAX_PROCESSES) n = MAX_PROCESSES;

    reset_processes();

    for (i = 0; i < n; i++) {
        int units, prio, gap;

        r = r * 1103515245u + 12345u;        /* reproducible generator */
        units = (int)((r >> 16) % 5) + 1;    /* 1 to 5 units           */
        r = r * 1103515245u + 12345u;
        prio  = (int)((r >> 16) % 3) + 1;    /* CRITICAL .. NORMAL     */
        r = r * 1103515245u + 12345u;
        gap   = (int)((r >> 16) % 3);        /* 0 to 2 ticks apart     */

        snprintf(name, sizeof name, "Patient%d", i + 1);
        arrival += gap;
        create_process_at(name, groups[i % 8], units,
                          (Priority)prio, arrival, units * 3);
    }

    printf("\nWorkload generated: %d requests, seed %u.\n", n, seed ? seed : 12345u);
    printf("Burst = units x 3, arrival gaps 0-2 ticks, mixed priorities.\n");
    printf("Run 'compare' to measure every algorithm on this workload.\n");
}

/* Write the last comparison as CSV, for the charts in the report. */
int scheduler_export(const char *path)
{
    FILE *f;
    char label[32];
    int k;

    if (!have_results) {
        printf("Run 'compare' first; there is nothing to export yet.\n");
        return -1;
    }
    if (path == NULL || *path == '\0') path = "results.csv";

    f = fopen(path, "w");
    if (!f) { printf("Could not write %s\n", path); return -1; }

    fprintf(f, "algorithm,requests,quantum,avg_wt,avg_tat,avg_rt,"
               "context_switches,cpu_utilisation,throughput_per_10_ticks\n");
    for (k = 0; k < NCOMPARE; k++) {
        label_for(compare_order[k], label, sizeof label);
        fprintf(f, "%s,%d,%d,%.2f,%.2f,%.2f,%d,%.1f,%.2f\n",
                label, last_n, quantum,
                last_stats[k].avg_wt, last_stats[k].avg_tat, last_stats[k].avg_rt,
                last_stats[k].switches, last_stats[k].cpu_util,
                last_stats[k].throughput);
    }
    fclose(f);

    printf("Exported %d rows to %s\n", NCOMPARE, path);
    printf("Open it in Excel to draw the charts for the report.\n");
    return 0;
>>>>>>> Stashed changes
}
