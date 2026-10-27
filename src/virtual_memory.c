
#include <stdio.h>
#include "virtual_memory.h"

#define MAX_PAGES 30
#define MAX_FRAMES 10

static void print_frames(const int frames[], int count)
{
    for (int i = 0; i < count; i++)
    {
        if (frames[i] == -1)
            printf("- ");
        else
            printf("%d ", frames[i]);
    }
    printf("\n");
}

/* Demand Paging */
void demand_paging_demo(void)
{
    int page_table[] = {1, -1, 3, -1, 2};
    int page_count = 5;

    printf("\n--- DEMAND PAGING DEMO ---\n");

    for (int page = 0; page < page_count; page++)
    {
        if (page_table[page] == -1)
            printf("Page %d: Page fault (not in memory)\n", page);
        else
            printf("Page %d: Present in frame %d\n",
                   page, page_table[page]);
    }
}

/* FIFO Page Replacement */
void fifo_demo(void)
{
    int pages[] = {1, 2, 3, 1, 4, 5};
    int n = sizeof(pages) / sizeof(pages[0]);
    int frames[3] = {-1, -1, -1};
    int next = 0, faults = 0;

    printf("\n--- FIFO PAGE REPLACEMENT ---\n");

    for (int i = 0; i < n; i++)
    {
        int found = 0;

        for (int j = 0; j < 3; j++)
            if (frames[j] == pages[i])
                found = 1;

        if (!found)
        {
            frames[next] = pages[i];
            next = (next + 1) % 3;
            faults++;
        }

        printf("Page %d: ", pages[i]);
        print_frames(frames, 3);
    }

    printf("Total page faults: %d\n", faults);
}

/* LRU Page Replacement */
void lru_demo(void)
{
    int pages[] = {1, 2, 3, 1, 4, 5};
    int n = sizeof(pages) / sizeof(pages[0]);
    int frames[3] = {-1, -1, -1};
    int last_used[3] = {0, 0, 0};
    int faults = 0;

    printf("\n--- LRU PAGE REPLACEMENT ---\n");

    for (int i = 0; i < n; i++)
    {
        int found = -1;

        for (int j = 0; j < 3; j++)
            if (frames[j] == pages[i])
                found = j;

        if (found != -1)
        {
            last_used[found] = i + 1;
        }
        else
        {
            int replace = -1;

            for (int j = 0; j < 3; j++)
                if (frames[j] == -1)
                {
                    replace = j;
                    break;
                }

            if (replace == -1)
            {
                replace = 0;

                for (int j = 1; j < 3; j++)
                    if (last_used[j] < last_used[replace])
                        replace = j;
            }

            frames[replace] = pages[i];
            last_used[replace] = i + 1;
            faults++;
        }

        printf("Page %d: ", pages[i]);
        print_frames(frames, 3);
    }

    printf("Total page faults: %d\n", faults);
}

/* Optimal Page Replacement */
void optimal_demo(void)
{
    int pages[] = {1, 2, 3, 1, 4, 5};
    int n = sizeof(pages) / sizeof(pages[0]);
    int frames[3] = {-1, -1, -1};
    int faults = 0;

    printf("\n--- OPTIMAL PAGE REPLACEMENT ---\n");

    for (int i = 0; i < n; i++)
    {
        int found = 0;

        for (int j = 0; j < 3; j++)
            if (frames[j] == pages[i])
                found = 1;

        if (!found)
        {
            int replace = -1;

            for (int j = 0; j < 3; j++)
                if (frames[j] == -1)
                {
                    replace = j;
                    break;
                }

            if (replace == -1)
            {
                int farthest = -1;

                for (int j = 0; j < 3; j++)
                {
                    int k;

                    for (k = i + 1; k < n; k++)
                        if (pages[k] == frames[j])
                            break;

                    if (k == n)
                    {
                        replace = j;
                        break;
                    }

                    if (k > farthest)
                    {
                        farthest = k;
                        replace = j;
                    }
                }
            }

            frames[replace] = pages[i];
            faults++;
        }

        printf("Page %d: ", pages[i]);
        print_frames(frames, 3);
    }

    printf("Total page faults: %d\n", faults);
}

/* LFU Page Replacement */
void lfu_demo(void)
{
    int pages[] = {1, 2, 1, 3, 2, 4, 1, 5};
    int n = sizeof(pages) / sizeof(pages[0]);
    int frames[3] = {-1, -1, -1};
    int frequency[3] = {0, 0, 0};
    int faults = 0;

    printf("\n--- LFU PAGE REPLACEMENT ---\n");

    for (int i = 0; i < n; i++)
    {
        int found = -1;

        for (int j = 0; j < 3; j++)
            if (frames[j] == pages[i])
                found = j;

        if (found != -1)
        {
            frequency[found]++;
        }
        else
        {
            int replace = -1;

            for (int j = 0; j < 3; j++)
                if (frames[j] == -1)
                {
                    replace = j;
                    break;
                }

            if (replace == -1)
            {
                replace = 0;

                for (int j = 1; j < 3; j++)
                    if (frequency[j] < frequency[replace])
                        replace = j;
            }

            frames[replace] = pages[i];
            frequency[replace] = 1;
            faults++;
        }

        printf("Page %d: ", pages[i]);
        print_frames(frames, 3);
    }

    printf("Total page faults: %d\n", faults);
}

/* Belady's Anomaly demonstration */
static int fifo_fault_count(const int pages[], int n, int capacity)
{
    int frames[MAX_FRAMES];

    for (int i = 0; i < capacity; i++)
        frames[i] = -1;

    int next = 0, faults = 0;

    for (int i = 0; i < n; i++)
    {
        int found = 0;

        for (int j = 0; j < capacity; j++)
            if (frames[j] == pages[i])
                found = 1;

        if (!found)
        {
            frames[next] = pages[i];
            next = (next + 1) % capacity;
            faults++;
        }
    }

    return faults;
}

void belady_anomaly_demo(void)
{
    int pages[] = {1, 2, 3, 4, 1, 2, 5, 1, 2, 3, 4, 5};
    int n = sizeof(pages) / sizeof(pages[0]);

    printf("\n--- BELADY'S ANOMALY ---\n");

    for (int frames = 3; frames <= 4; frames++)
    {
        printf("%d frames -> %d page faults\n",
               frames, fifo_fault_count(pages, n, frames));
    }

    printf("Belady's anomaly: FIFO can produce more faults\n");
    printf("even when the number of frames is increased.\n");
}

/* Thrashing demonstration */
void thrashing_demo(void)
{
    int allocated_frames = 2;
    int working_set_pages = 6;

    printf("\n--- THRASHING DEMO ---\n");
    printf("Working set pages: %d\n", working_set_pages);
    printf("Allocated frames: %d\n", allocated_frames);

    if (allocated_frames < working_set_pages)
    {
        printf("Warning: Memory is insufficient for the working set.\n");
        printf("Frequent page faults may cause thrashing.\n");
    }
    else
    {
        printf("Allocated memory may be sufficient for the working set.\n");
    }
}

/* FIFO and LRU Page Fault Comparison */
void paging_compare_demo(void)
{
    int pages[] = {101, 102, 103, 101, 104, 105, 102, 106, 103, 104};
    int n = sizeof(pages) / sizeof(pages[0]);
    int frame_count = 3;

    int fifo_frames[3] = {-1, -1, -1};
    int lru_frames[3] = {-1, -1, -1};
    int last_used[3] = {0, 0, 0};

    int fifo_next = 0;
    int fifo_faults = 0;
    int lru_faults = 0;

    printf("\n--- FIFO AND LRU PAGE FAULT COMPARISON ---\n");
    printf("Reference string: ");

    for (int i = 0; i < n; i++)
        printf("%d ", pages[i]);

    printf("\nNumber of frames: %d\n", frame_count);

    /* FIFO Page Replacement */
    printf("\n--- FIFO ---\n");

    for (int i = 0; i < n; i++)
    {
        int found = 0;

        for (int j = 0; j < frame_count; j++)
        {
            if (fifo_frames[j] == pages[i])
            {
                found = 1;
                break;
            }
        }

        if (!found)
        {
            fifo_frames[fifo_next] = pages[i];
            fifo_next = (fifo_next + 1) % frame_count;
            fifo_faults++;
            printf("Page %d: FAULT  ", pages[i]);
        }
        else
        {
            printf("Page %d: HIT    ", pages[i]);
        }

        print_frames(fifo_frames, frame_count);
    }

    /* LRU Page Replacement */
    printf("\n--- LRU ---\n");

    for (int i = 0; i < n; i++)
    {
        int found = -1;

        for (int j = 0; j < frame_count; j++)
        {
            if (lru_frames[j] == pages[i])
            {
                found = j;
                break;
            }
        }

        if (found != -1)
        {
            last_used[found] = i + 1;
            printf("Page %d: HIT    ", pages[i]);
        }
        else
        {
            int replace = -1;

            for (int j = 0; j < frame_count; j++)
            {
                if (lru_frames[j] == -1)
                {
                    replace = j;
                    break;
                }
            }

            if (replace == -1)
            {
                replace = 0;

                for (int j = 1; j < frame_count; j++)
                {
                    if (last_used[j] < last_used[replace])
                        replace = j;
                }
            }

            lru_frames[replace] = pages[i];
            last_used[replace] = i + 1;
            lru_faults++;

            printf("Page %d: FAULT  ", pages[i]);
        }

        print_frames(lru_frames, frame_count);
    }

    /* Compare total page faults */
    printf("\n--- FINAL COMPARISON ---\n");
    printf("FIFO page faults: %d\n", fifo_faults);
    printf("LRU page faults : %d\n", lru_faults);

    if (fifo_faults < lru_faults)
        printf("FIFO performed better for this reference string.\n");
    else if (lru_faults < fifo_faults)
        printf("LRU performed better for this reference string.\n");
    else
        printf("Both algorithms produced equal page faults.\n");
}
