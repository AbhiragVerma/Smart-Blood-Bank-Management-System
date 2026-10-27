#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>

#include "mlq_scheduler.h"
#include "pcb.h"
#include "blood.h"
#include "scheduler.h"
#include "logger.h"

#include "sync.h"
#include "memory.h"
#include "paging.h"
#include "virtual_memory.h"
#include "file_allocation.h"
#include "disk_scheduling.h"
#include "bankers.h"


/* -------------------------------------------------
 * Existing process creation function
 * ------------------------------------------------- */
extern int create_process(
    const char *patient,
    const char *blood_group,
    int units,
    Priority priority
);

/* -------------------------------------------------
 * Display process table
 * ------------------------------------------------- */
static void show_processes(void)
{
    if (process_count == 0)
    {
        printf("\nNo processes available.\n");
        return;
    }

    printf("\n");
    printf("==============================================\n");
    printf("              PROCESS TABLE\n");
    printf("==============================================\n");

    printf(
        "%-6s %-15s %-8s %-6s %-12s %-12s\n",
        "PID",
        "PATIENT",
        "GROUP",
        "UNITS",
        "PRIORITY",
        "STATE"
    );

    printf("----------------------------------------------\n");

    for (int i = 0; i < process_count; i++)
    {
        PCB *p = &process_table[i];

        printf(
            "%-6d %-15s %-8s %-6d %-12s %-12s\n",
            p->pid,
            p->patient,
            p->blood_group,
            p->units,
            priority_name(p->priority),
            state_name(p->state)
        );
    }

    printf("==============================================\n");
}

/* -------------------------------------------------
 * Help
 * ------------------------------------------------- */
static void print_help(void)
{
    printf("\n");
    printf("========== RaktSetu Commands ==========\n");

    printf("help                         Show commands\n");

    printf("request <patient> <group> <units> <priority>\n");
    printf("                             Create blood request\n");

    printf("ps                           Show process table\n");
    printf("blood                        Show blood inventory\n");

    printf("schedule <fcfs|priority|mlq> Select scheduler\n");
    printf("run                          Execute READY processes\n");

    printf("logs                         Show process state logs\n");


    printf("mutex <on|off>               Run mutex race demo\n");
    printf("lockvariable                 Run Lock Variable demo\n");
    printf("strict                       Run Strict Alternation demo\n");
    printf("peterson                     Run Peterson's algorithm\n");
    printf("bakery                       Run Lamport Bakery algorithm\n");
    printf("testandset                   Run Test-and-Set algorithm\n");
    printf("semaphore                    Run binary/counting semaphore demo\n");
    printf("monitor                      Run monitor demo\n");
    printf("boundedbuffer                Run producer-consumer demo\n");
    printf("philosophers                 Run Dining Philosophers demo\n");
    printf("readerwriter                 Run Reader-Writer demo\n");
    printf("barber                       Run Sleeping Barber demo\n");

    printf("memfit                       Compare First/Best/Worst Fit\n");
    printf("memfixed                     Demonstrate fixed partitions\n");
    printf("paging                       Demonstrate paging\n");
    printf("pagingcompare                Compare FIFO and LRU page faults\n");
    printf("bankers                      Run Banker's Algorithm safety demo\n");
    printf("segmentation                 Demonstrate segmentation\n");
    printf("segpaging                    Demonstrate segmented paging\n");
    printf("demandpaging                 Demonstrate demand paging\n");
    printf("fifo                         FIFO page replacement\n");
    printf("lru                          LRU page replacement\n");
    printf("optimal                      Optimal page replacement\n");
    printf("lfu                          LFU page replacement\n");
    printf("belady                       Demonstrate Belady's anomaly\n");
    printf("thrashing                    Demonstrate thrashing\n");
    printf("contiguous                   Contiguous file allocation\n");
    printf("linked                       Linked file allocation\n");
    printf("indexed                      Indexed file allocation\n");
    printf("fcfs                         FCFS disk scheduling\n");
    printf("sstf                         SSTF disk scheduling\n");
    printf("scan                         SCAN disk scheduling\n");
    printf("cscan                        C-SCAN disk scheduling\n");
    printf("look                         LOOK disk scheduling\n");


    printf("exit                         Exit program\n");

    printf("========================================\n");
}

/* -------------------------------------------------
 * Parse priority
 * ------------------------------------------------- */
static Priority parse_priority(const char *text)
{
    if (strcasecmp(text, "critical") == 0)
        return PRIO_CRITICAL;

    if (strcasecmp(text, "urgent") == 0)
        return PRIO_URGENT;

    if (strcasecmp(text, "normal") == 0)
        return PRIO_NORMAL;

    return PRIO_NORMAL;
}

/* -------------------------------------------------
 * Mutex
 * ------------------------------------------------- */
static void handle_mutex(char *mode)
{
    if (strcasecmp(mode, "on") == 0)
        run_mutex_demo(1);
    else if (strcasecmp(mode, "off") == 0)
        run_mutex_demo(0);
    else
        printf("Usage: mutex <on|off>\n");
}

/* -------------------------------------------------
 * Synchronization demos
 * ------------------------------------------------- */
static void handle_lock_variable(void)
{
    run_lock_variable_demo();
}

static void handle_strict_alternation(void)
{
    run_strict_alternation_demo();
}

static void handle_peterson(void)
{
    run_peterson_demo();
}

static void handle_bakery(void)
{
    run_bakery_demo();
}

static void handle_test_and_set(void)
{
    run_test_and_set_demo();
}

static void handle_semaphore(void)
{
    run_semaphore_demo();
}

static void handle_monitor(void)
{
    run_monitor_demo();
}

static void handle_bounded_buffer(void)
{
    run_bounded_buffer_demo();
}

static void handle_philosophers(void)
{
    run_philosophers_demo();
}

static void handle_reader_writer(void)
{
    run_reader_writer_demo();
}

static void handle_sleeping_barber(void)
{
    run_sleeping_barber_demo();
}

/* -------------------------------------------------
 * Main Shell
 * ------------------------------------------------- */
int main(void)
{
    char input[256];

    /*
     * 0 = Existing FCFS / Priority scheduler
     * 1 = MLQ scheduler
     */
    int use_mlq = 0;

    printf("\n");
    printf("====================================\n");
    printf("       RAKTSETU BLOOD BANK OS\n");
    printf("====================================\n");
    printf("Type 'help' to see available commands.\n");

    while (1)
    {
        printf("\nraktsetu> ");

        if (fgets(input, sizeof(input), stdin) == NULL)
            break;

        input[strcspn(input, "\n")] = '\0';

        if (strlen(input) == 0)
            continue;

        if (strcmp(input, "help") == 0)
        {
            print_help();
        }
        else if (strcmp(input, "exit") == 0)
        {
            printf("Exiting RaktSetu...\n");
            break;
        }
        else if (strcmp(input, "ps") == 0)
        {
            show_processes();
        }
        else if (strcmp(input, "blood") == 0)
        {
            print_inventory();
        }
        else if (strcmp(input, "logs") == 0)
        {
            show_logs();
        }
        else if (strncmp(input, "mutex ", 6) == 0)
        {
            handle_mutex(input + 6);
        }
        else if (strcmp(input, "lockvariable") == 0)
        {
            handle_lock_variable();
        }
        else if (strcmp(input, "strict") == 0)
        {
            handle_strict_alternation();
        }
        else if (strcmp(input, "peterson") == 0)
        {
            handle_peterson();
        }
        else if (strcmp(input, "bakery") == 0)
        {
            handle_bakery();
        }
        else if (strcmp(input, "testandset") == 0)
        {
            handle_test_and_set();
        }
        else if (strcmp(input, "semaphore") == 0)
        {
            handle_semaphore();
        }
        else if (strcmp(input, "monitor") == 0)
        {
            handle_monitor();
        }
        else if (strcmp(input, "boundedbuffer") == 0)
        {
            handle_bounded_buffer();
        }
        else if (strcmp(input, "philosophers") == 0)
        {
            handle_philosophers();
        }
        else if (strcmp(input, "readerwriter") == 0)
        {
            handle_reader_writer();
        }
        else if (strcmp(input, "barber") == 0)
        {
            handle_sleeping_barber();
        }
        else if (strcmp(input, "memfit") == 0)
        {
            memory_fit_demo();
        }
       else if (strcmp(input, "memfixed") == 0)
        {
            memory_fixed_demo();
        }

        else if (strcmp(input, "paging") == 0)
        {
            paging_demo();
        }
        else if (strcmp(input, "segmentation") == 0)
        {
            segmentation_demo();
        }
        else if (strcmp(input, "segpaging") == 0)
        {
            segmented_paging_demo();
        }
        else if (strcmp(input, "demandpaging") == 0)
        {
            demand_paging_demo();
        }
        else if (strcmp(input, "fifo") == 0)
        {
            fifo_demo();
        }
        else if (strcmp(input, "lru") == 0)
        {
            lru_demo();
        }
        else if (strcmp(input, "pagingcompare") == 0)
        {
            paging_compare_demo();
        }
        else if (strcmp(input, "bankers") == 0)
        {
            bankers_demo();
        }
        else if (strcmp(input, "optimal") == 0)
        {
            optimal_demo();
        }
        else if (strcmp(input, "lfu") == 0)
        {
            lfu_demo();
        }
        else if (strcmp(input, "belady") == 0)
        {
            belady_anomaly_demo();
        }
        else if (strcmp(input, "thrashing") == 0)
        {
            thrashing_demo();
        }
        else if (strcmp(input, "contiguous") == 0)
        {
            contiguous_allocation_demo();
        }
        else if (strcmp(input, "linked") == 0)
        {
            linked_allocation_demo();
        }
        else if (strcmp(input, "indexed") == 0)
        {
            indexed_allocation_demo();
        }
        else if (strcmp(input, "fcfs") == 0)
        {
            fcfs_disk_demo();
        }
        else if (strcmp(input, "sstf") == 0)
        {
            sstf_disk_demo();
        }
        else if (strcmp(input, "scan") == 0)
        {
            scan_disk_demo();
        }
        else if (strcmp(input, "cscan") == 0)
        {
            cscan_disk_demo();
        }
        else if (strcmp(input, "look") == 0)
        {
            look_disk_demo();
        }


        else if (strncmp(input, "request ", 8) == 0)
        {
            char patient[32];
            char group[4];
            char priority_text[20];
            int units;

            int result = sscanf(
                input + 8,
                "%31s %3s %d %19s",
                patient,
                group,
                &units,
                priority_text
            );

            if (result != 4)
            {
                printf(
                    "Usage: request <patient> <group> <units> <priority>\n"
                );

                printf(
                    "Priority: critical | urgent | normal\n"
                );

                continue;
            }

            Priority priority = parse_priority(priority_text);

            int pid = create_process(
                patient,
                group,
                units,
                priority
            );

            if (pid >= 0)
                printf(
                    "Process created successfully. PID: %d\n",
                    pid
                );
            else
                printf("Failed to create process.\n");
        }

        /* -------------------------------------------------
         * Scheduler selection
         * ------------------------------------------------- */
        else if (strncmp(input, "schedule ", 9) == 0)
        {
            const char *name = input + 9;

            /*
             * MLQ uses the separate mlq_scheduler.c module.
             */
            if (strcasecmp(name, "mlq") == 0)
            {
                use_mlq = 1;

                printf("\n");
                printf("Scheduler set to MLQ.\n");
                printf("Q0: CRITICAL -> Priority\n");
                printf("Q1: URGENT   -> Round Robin (q=2)\n");
                printf("Q2: NORMAL   -> FCFS\n");
                printf("Aging enabled: threshold = 6\n");
            }
            else
            {
                SchedAlgo algo;

                if (scheduler_parse(name, &algo) == 0)
                {
                    /*
                     * Switch back to existing scheduler.
                     */
                    use_mlq = 0;

                    scheduler_set(algo);

                    printf(
                        "Scheduler set to %s.\n",
                        scheduler_name(algo)
                    );
                }
                else
                {
                    printf(
                        "Usage: schedule <fcfs|priority|mlq>\n"
                    );
                }
            }
        }

        /* -------------------------------------------------
         * Run selected scheduler
         * ------------------------------------------------- */
        else if (strcmp(input, "run") == 0)
        {
            int executed;

            if (use_mlq)
            {
                /*
                 * Run Amulya's MLQ + Aging scheduler.
                 */
                executed = mlq_scheduler_run(1);

                printf(
                    "MLQ Scheduler executed %d process(es).\n",
                    executed
                );
            }
            else
            {
                /*
                 * Run existing FCFS / Priority scheduler.
                 */
                executed = scheduler_run(1);

                printf(
                    "Scheduler executed %d process(es).\n",
                    executed
                );
            }
        }
        else
        {
            printf("Unknown command: %s\n", input);
            printf("Type 'help' to see available commands.\n");
        }
    }

    return 0;
}
// ..