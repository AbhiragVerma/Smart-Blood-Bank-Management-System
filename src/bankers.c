#include <stdio.h>
#include <string.h>
#include "bankers.h"

#define PROCESSES 5
#define GROUPS 8

static const char *blood_groups[GROUPS] = {
    "A+", "A-", "B+", "B-", "AB+", "AB-", "O+", "O-"
};

static void calculate_need(const int max[PROCESSES][GROUPS],
                           const int allocation[PROCESSES][GROUPS],
                           int need[PROCESSES][GROUPS])
{
    int i, j;
    for (i = 0; i < PROCESSES; i++)
        for (j = 0; j < GROUPS; j++)
            need[i][j] = max[i][j] - allocation[i][j];
}

static int safety_check(const int allocation[PROCESSES][GROUPS],
                        const int need[PROCESSES][GROUPS],
                        const int available[GROUPS],
                        int safe_sequence[PROCESSES])
{
    int work[GROUPS], finished[PROCESSES] = {0};
    int i, j, count = 0, possible;

    memcpy(work, available, sizeof(work));

    while (count < PROCESSES) {
        possible = 0;
        for (i = 0; i < PROCESSES; i++) {
            if (finished[i]) continue;
            for (j = 0; j < GROUPS; j++)
                if (need[i][j] > work[j]) break;

            if (j == GROUPS) {
                for (j = 0; j < GROUPS; j++)
                    work[j] += allocation[i][j];
                safe_sequence[count++] = i;
                finished[i] = 1;
                possible = 1;
            }
        }
        if (!possible) return 0;
    }
    return 1;
}

static void print_matrix(const char *title, const int matrix[PROCESSES][GROUPS])
{
    int i, j;
    printf("\n%s\n", title);
    printf("Proc  ");
    for (j = 0; j < GROUPS; j++) printf("%-5s", blood_groups[j]);
    printf("\n");
    for (i = 0; i < PROCESSES; i++) {
        printf("P%-4d ", i);
        for (j = 0; j < GROUPS; j++) printf("%-5d", matrix[i][j]);
        printf("\n");
    }
}

void bankers_demo(void)
{
    /* Demo values: each column is one blood-group resource type. */
    int allocation[PROCESSES][GROUPS] = {
        {1,0,0,0,0,0,0,0}, {0,1,0,0,0,0,0,0},
        {0,0,1,0,0,0,0,0}, {0,0,0,1,0,0,0,0},
        {0,0,0,0,1,0,0,0}
    };
    int max[PROCESSES][GROUPS] = {
        {1,0,0,0,0,0,1,0}, {0,1,0,0,0,0,0,1},
        {0,0,1,0,0,0,1,0}, {0,0,0,1,0,0,0,1},
        {0,0,0,0,1,1,0,0}
    };
    int available[GROUPS] = {1,1,1,1,1,1,1,1};
    int need[PROCESSES][GROUPS];
    int safe_sequence[PROCESSES];
    int i, j;

    calculate_need(max, allocation, need);
    printf("\n========== BANKER'S ALGORITHM =========\n");
    printf("Resource columns: A+, A-, B+, B-, AB+, AB-, O+, O-\n");
    print_matrix("MAX MATRIX", max);
    print_matrix("ALLOCATION MATRIX", allocation);
    print_matrix("NEED MATRIX (Max - Allocation)", need);

    printf("\nAvailable: ");
    for (j = 0; j < GROUPS; j++) printf("%s=%d ", blood_groups[j], available[j]);
    printf("\n\nInitial safety check: ");
    if (safety_check(allocation, need, available, safe_sequence)) {
        printf("SAFE\nSafe sequence: ");
        for (i = 0; i < PROCESSES; i++) printf("P%d%s", safe_sequence[i], i == PROCESSES-1 ? "\n" : " -> ");
    } else {
        printf("UNSAFE; no safe sequence exists.\n");
        return;
    }

    /* Demonstrate a real request: P1 asks for one O- unit. */
    {
        int process = 1;
        int request[GROUPS] = {0,0,0,0,0,0,0,1};
        int can_grant = 1;
        printf("\nRequest test: P%d requests 1 unit of O-\n", process);
        for (j = 0; j < GROUPS; j++) {
            if (request[j] > need[process][j] || request[j] > available[j]) {
                can_grant = 0;
                break;
            }
        }
        if (!can_grant) {
            printf("Request DENIED: exceeds declared Need or Available resources.\n");
        } else {
            for (j = 0; j < GROUPS; j++) {
                available[j] -= request[j];
                allocation[process][j] += request[j];
                need[process][j] -= request[j];
            }
            if (safety_check(allocation, need, available, safe_sequence)) {
                printf("Request GRANTED: resulting state is SAFE.\nSafe sequence: ");
                for (i = 0; i < PROCESSES; i++) printf("P%d%s", safe_sequence[i], i == PROCESSES-1 ? "\n" : " -> ");
            } else {
                for (j = 0; j < GROUPS; j++) {
                    available[j] += request[j];
                    allocation[process][j] -= request[j];
                    need[process][j] += request[j];
                }
                printf("Request DENIED: it would make the system unsafe.\n");
            }
        }
    }
    printf("=======================================\n");
}
