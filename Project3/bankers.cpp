#include <iostream>
using namespace std;

const int n = 5; // Number of processes
const int m = 3; // Number of resource types

// Function to print a matrix
void printMatrix(int arr[][m], int rows) {
    for (int i = 0; i < rows; i++) {
        cout << " [";
        for (int j = 0; j < m; j++) {
            cout << arr[i][j];
            if (j < m - 1) cout << ", ";
        }
        cout << "]";
        if (i < rows - 1) cout << ","; // Add a comma only between rows
        cout << endl;
    }
}

// Function to calculate the need matrix
void findNeed(int need[n][m], int maxm[n][m], int allot[n][m]) {
    // Need for each process
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            need[i][j] = maxm[i][j] - allot[i][j];
        }
    }
}

// Function to check if system is in a safe state
bool safe(int processes[], int available[], int maxm[][m], int allot[][m]) {
    int need[n][m];
    findNeed(need, maxm, allot);

    cout << "Max matrix = [\n";
    printMatrix(maxm, n);  // Print Max matrix
    cout << "Allocation matrix = [\n";
    printMatrix(allot, n);  // Print Allocation matrix
    cout << "Need matrix = [\n";
    printMatrix(need, n);  // Print Need matrix
    cout << "]" << endl;

    bool finish[n] = {false};
    int safeSeq[n];

    int work[m];
    for (int i = 0; i < m; i++) {
        work[i] = available[i];
    }

    int count = 0;
    while (count < n) {
        bool found = false;

        for (int p = 0; p < n; p++) {
            if (!finish[p]) {
                int j;
                for (j = 0; j < m; j++) {
                    if (need[p][j] > work[j]) {
                        break;
                    }
                }

                if (j == m) {
                    for (int k = 0; k < m; k++) {
                        work[k] += allot[p][k];
                    }
                    safeSeq[count++] = p;
                    finish[p] = true;
                    found = true;
                }
            }
        }

        if (!found) {
            cout << "System is not in a safe state\n";
            return false;
        }
    }

    cout << "System is in a safe state.\n";
    cout << "Safe sequence is: [";
    for (int i = 0; i < n; i++) {
        cout << safeSeq[i];
        if (i < n - 1) cout << ", ";  
    }
    cout << "]" << endl;
    return true;
}

// Function to request resources
bool request_resources(int p, int request[], int available[], int maxm[][m], int allot[][m], int processes[]) {
    int need[n][m];
    findNeed(need, maxm, allot);

    // Check if the request is less than the need
    for (int i = 0; i < m; i++) {
        if (request[i] > need[p][i]) {
            cout << "Error: Process has exceeded its maximum claim\n";
            return false;
        }
    }

    // Check if available resources are sufficient
    for (int i = 0; i < m; i++) {
        if (request[i] > available[i]) {
            cout << "Error: Not enough resources available\n";
            return false;
        }
    }

    // Allocate resources
    for (int i = 0; i < m; i++) {
        available[i] -= request[i];
        allot[p][i] += request[i];
    }

    // Check if the system is still in a safe state after allocation
    return safe(processes, available, maxm, allot);
}

int main() {
    int processes[] = {0, 1, 2, 3, 4}; 
    int available[] = {3, 3, 2};

    int maxm[][m] = {{7, 5, 3},
                     {3, 2, 2},
                     {9, 0, 2},
                     {2, 2, 2},
                     {4, 3, 3}};

    int allot[][m] = {{0, 1, 0},
                      {2, 0, 0},
                      {3, 0, 2},
                      {2, 1, 1},
                      {0, 0, 2}};
    
    // Test Case 1 (Safe check)
    cout << "Test Case 1\n";
    cout << "Available = [";
    for (int i = 0; i < m; i++) {
        cout << available[i];
        if (i < m - 1) cout << ", ";
    }
    cout << "]\n";
    
    cout << "Max = [\n";
    printMatrix(maxm, n);
    
    cout << "Allocation = [\n";
    printMatrix(allot, n);

    safe(processes, available, maxm, allot);

    // Test Case 2 (Process 1 requests resources)
    cout << "\nTest Case 2\n";
    int request[] = {1, 0, 2}; // Process 1's resource request
    cout << "Request = [";
    for (int i = 0; i < m; i++) {
        cout << request[i];
        if (i < m - 1) cout << ", ";
    }
    cout << "]\n";

    // Request resources for process 1
    if (request_resources(1, request, available, maxm, allot, processes)) {
        cout << "Resources allocated to process 1.\n";
    } else {
        cout << "Resource request failed.\n";
    }

    // Test Case 3 (Process 4 requests more resources than available)
    cout << "\nTest Case 3\n";
    int request2[] = {3, 3, 1}; // Process 4's resource request
    cout << "Request = [";
    for (int i = 0; i < m; i++) {
        cout << request2[i];
        if (i < m - 1) cout << ", ";
    }
    cout << "]\n";

    // Request resources for process 4
    if (request_resources(4, request2, available, maxm, allot, processes)) {
        cout << "Resources allocated to process 4.\n";
    } else {
        cout << "Resource request failed.\n";
    }

    return 0;
}

