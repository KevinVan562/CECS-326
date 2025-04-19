/**
 * Round-robin scheduling
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "task.h"
#include "list.h"
#include "cpu.h"

// Global task queue and TID counter
typedef struct node Node;
Node *taskList = NULL;
int tid_counter = 1;

// Default time quantum if not provided
#define DEFAULT_QUANTUM 10

// Add a task to the tail of the queue
void add(char *name, int priority, int burst) {
    Task *newTask = malloc(sizeof(Task));
    if (!newTask) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    newTask->name = name;
    newTask->priority = priority;
    newTask->burst = burst;
    newTask->tid = tid_counter++;

    // create new node
    Node *newNode = malloc(sizeof(Node));
    if (!newNode) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    newNode->task = newTask;
    newNode->next = NULL;

    // append to tail
    if (!taskList) {
        taskList = newNode;
    } else {
        Node *temp = taskList;
        while (temp->next) temp = temp->next;
        temp->next = newNode;
    }
}

// Round Robin scheduler
void schedule(int quantum) {
    printf("Round Robin Scheduling (quantum=%d)\n", quantum);
    printf("==============================\n");

    while (taskList) {
        // dequeue head task
        Node *node = taskList;
        Task *task = node->task;
        taskList = node->next;
        free(node);

        // run for quantum or remaining burst
        int slice = (task->burst < quantum) ? task->burst : quantum;
        run(task, slice);
        task->burst -= slice;

        if (task->burst > 0) {
            // requeue at tail
            Node *newNode = malloc(sizeof(Node));
            newNode->task = task;
            newNode->next = NULL;
            if (!taskList) {
                taskList = newNode;
            } else {
                Node *temp = taskList;
                while (temp->next) temp = temp->next;
                temp->next = newNode;
            }
        } else {
            printf("Task %s (tid=%d) finished.\n", task->name, task->tid);
            free(task->name);
            free(task);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Usage: %s <input_file> [quantum]\n", argv[0]);
        return 1;
    }

    int quantum = DEFAULT_QUANTUM;
    if (argc == 3) {
        quantum = atoi(argv[2]);
        if (quantum <= 0) {
            fprintf(stderr, "Invalid quantum: %s\n", argv[2]);
            return 1;
        }
    }

    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        perror("Error opening file");
        return 1;
    }

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        char *name_tok = strtok(line, ", \t\n");
        char *priority_tok = strtok(NULL, ", \t\n");
        char *burst_tok = strtok(NULL, ", \t\n");
        if (!name_tok || !priority_tok || !burst_tok) continue;
        add(strdup(name_tok), atoi(priority_tok), atoi(burst_tok));
    }
    fclose(fp);

    schedule(quantum);
    return 0;
}
