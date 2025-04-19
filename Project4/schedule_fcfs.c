/**
 * FCFS scheduling
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "task.h"
#include "list.h"
#include "cpu.h"

// Global task list and TID counter
typedef struct node Node;
Node *taskList = NULL;
int tid_counter = 1;

// Add a task to the list
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
    insert(&taskList, newTask);
}

// Pick the next task (oldest inserted = FIFO)
Task *pickNextTask() {
    if (!taskList) return NULL;

    Node *temp = taskList;
    Node *prev = NULL;

    // Traverse to the last node
    while (temp->next) {
        prev = temp;
        temp = temp->next;
    }

    // Remove the last node
    if (!prev) {
        taskList = NULL;
    } else {
        prev->next = NULL;
    }

    Task *task = temp->task;
    free(temp);
    return task;
}

// Schedule all tasks
void schedule() {
    printf("FCFS Scheduling\n");
    printf("===============\n");

    Task *task;
    while ((task = pickNextTask()) != NULL) {
        run(task, task->burst);
        printf("Task %s (tid=%d) finished.\n", task->name, task->tid);
        free(task->name);
        free(task);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        perror("Error opening file");
        return 1;
    }

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        // Tokenize by comma and whitespace
        char *name_tok = strtok(line, ", \t\n");
        char *priority_tok = strtok(NULL, ", \t\n");
        char *burst_tok = strtok(NULL, ", \t\n");
        if (!name_tok || !priority_tok || !burst_tok) continue;
        add(strdup(name_tok), atoi(priority_tok), atoi(burst_tok));
    }
    fclose(fp);

    schedule();
    return 0;
}

