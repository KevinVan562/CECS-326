#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define NUM_PHILOSOPHERS 5

pthread_mutex_t mutex;
pthread_cond_t cond_var[NUM_PHILOSOPHERS];

// Array representing the forks, 0 means available, 1 means taken
int forks[NUM_PHILOSOPHERS] = {0};  

// Array to track which philosopher is holding each fork, -1 means no one is holding it
int fork_owner[NUM_PHILOSOPHERS] = {-1}; 

// Print the current status of the forks
void print_fork_status() {
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        if (fork_owner[i] != -1) {  // Only print fork if it's held by a philosopher
            printf("Fork #%d is with %d\n", i, fork_owner[i]);
        }
    }
}

void pickup_forks(int philosopher_number) {
    int left_fork = philosopher_number;
    int right_fork = (philosopher_number + 1) % NUM_PHILOSOPHERS;

    pthread_mutex_lock(&mutex);  // Lock the mutex to access the shared forks

    // Wait for both forks to be available
    while (forks[left_fork] == 1 || forks[right_fork] == 1) {
        pthread_cond_wait(&cond_var[philosopher_number], &mutex);  // Wait if either fork is taken
    }

    // Pick up the forks only if both are available
    forks[left_fork] = 1;
    forks[right_fork] = 1;
    fork_owner[left_fork] = philosopher_number;
    fork_owner[right_fork] = philosopher_number;

    // Print current fork status
    print_fork_status();

    pthread_mutex_unlock(&mutex);  // Unlock the mutex after picking up forks
}

void return_forks(int philosopher_number) {
    int left_fork = philosopher_number;
    int right_fork = (philosopher_number + 1) % NUM_PHILOSOPHERS;

    pthread_mutex_lock(&mutex);  // Lock the mutex to access the shared forks

    // Return the forks
    forks[left_fork] = 0;
    forks[right_fork] = 0;
    fork_owner[left_fork] = -1;
    fork_owner[right_fork] = -1;

    // Signal all philosophers to check if they can now pick up forks
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_cond_signal(&cond_var[i]);
    }

    pthread_mutex_unlock(&mutex);  // Unlock the mutex after returning forks
}

void* philosopher(void* arg) {
    int philosopher_number = *((int*)arg);  // Get philosopher number
    
    srand(time(NULL) + philosopher_number);

    while (1) {
        printf("Philosopher #%d is thinking...\n", philosopher_number);

        int thinking_time = rand() % 3000 + 1000; // Random time between 1000ms and 3000ms
        usleep(thinking_time * 1000);

        printf("Philosopher #%d took %dms thinking\n", philosopher_number, thinking_time);

        pickup_forks(philosopher_number);

        printf("Philosopher #%d is eating...\n", philosopher_number);

        int eating_time = rand() % 3000 + 1000; // Random time between 1000ms and 3000ms
        usleep(eating_time * 1000);

        printf("Philosopher #%d took %dms eating\n", philosopher_number, eating_time);

        // Philosopher returns forks after eating
        return_forks(philosopher_number);
    }

    return NULL;
}

int main() {
    pthread_t threads[NUM_PHILOSOPHERS];
    int philosopher_numbers[NUM_PHILOSOPHERS];

    srand(time(NULL));

    // Initialize mutex and condition variables
    pthread_mutex_init(&mutex, NULL);
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_cond_init(&cond_var[i], NULL);
    }

    // Create philosopher threads
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        philosopher_numbers[i] = i;
        pthread_create(&threads[i], NULL, philosopher, (void*)&philosopher_numbers[i]);
        usleep(100000);  // 100ms delay
    }

    // Keep the main thread alive indefinitely
    while (1) pause(); 

    return 0;
}

