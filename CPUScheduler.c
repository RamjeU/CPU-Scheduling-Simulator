#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

// Maximum number of processes that can be handled
#define MAX_PROCESSES 100
// Maximum length of a line in the input file
#define MAX_LINE_LENGTH 256

// Structure to hold process information
typedef struct {
    int id;                 // Process ID
    int burst_time;         // Total CPU time required
    int remaining_time;     // Remaining CPU time
    int arrival_time;       // Arrival time (used for FCFS and SJF)
    int wait_time;          // Total time the process has waited
    int turnaround_time;    // Total time from arrival to completion
    bool completed;         // Flag to indicate if process has completed execution
} Process;

// Global variables  
Process processes[MAX_PROCESSES];  // Array to hold all processes
int num_processes = 0;             // Total number of processes read from input file

// Function prototypes
void read_input_file(const char* filename);
void simulate_fcfs();
void simulate_sjf();
void simulate_round_robin(int quantum);
void print_process_stats();
void print_final_stats();
bool all_processes_complete();
Process* get_next_sjf_process(int current_time);
void update_wait_times(int current_time, int active_process_id);
void update_turnaround_times(int current_time);

/**
 * Reads process information from input file and initializes process array
 * File format: P0,3 (Process 0 with burst time 3)
 * @param filename Name of the input CSV file
 */
void read_input_file(const char* filename) {
    // Open the file for reading
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open file %s\n", filename);
        exit(1);
    }

    char line[MAX_LINE_LENGTH];
    // Read each line from the file
    while (fgets(line, sizeof(line), file)) {
        char process_id[10];
        int burst_time;
        
        // Parse the line to extract process ID and burst time
        if (sscanf(line, "P%[^,],%d", process_id, &burst_time) == 2) {
            // Initialize the process structure
            processes[num_processes].id = num_processes;
            processes[num_processes].burst_time = burst_time;
            processes[num_processes].remaining_time = burst_time;
            processes[num_processes].arrival_time = num_processes;  // For simplicity, arrival time is process ID
            processes[num_processes].wait_time = 0;
            processes[num_processes].turnaround_time = 0;
            processes[num_processes].completed = false;
            num_processes++;
        }
    }
    fclose(file);
}

/**
 * Simulates First Come First Served scheduling algorithm
 * Non-preemptive: each process runs to completion
 */
void simulate_fcfs() {
    printf("First Come First Served\n");
    int current_time = 0;    // Simulation time
    int current_process = 0; // Index of the current process being executed

    // Loop until all processes are complete
    while (!all_processes_complete()) {
        // Find next uncompleted process
        while (current_process < num_processes && processes[current_process].completed) {
            current_process++;
        }

        if (current_process < num_processes) {
            // Process current process
            printf("T%d : P%d - Burst left %2d, Wait time %d, Turnaround time %d\n",
                current_time,
                processes[current_process].id,
                processes[current_process].remaining_time,
                processes[current_process].wait_time,
                processes[current_process].turnaround_time);

            // Decrease remaining time of current process
            processes[current_process].remaining_time--;
                
            // If process has finished execution
            if (processes[current_process].remaining_time == 0) {
                processes[current_process].completed = true;
            }
        }

        // Update wait times and turnaround times
        update_wait_times(current_time, current_process);
        update_turnaround_times(current_time);
        // Increment current time
        current_time++;
    }
}

/**
 * Simulates Shortest Job First scheduling algorithm
 * Preemptive: shorter processes can interrupt longer ones
 */
void simulate_sjf() {
    printf("Shortest Job First\n");
    int current_time = 0;      // Simulation time
    Process* current_process = NULL; // Pointer to the current process being executed

    // Loop until all processes are complete
    while (!all_processes_complete()) {
        // Get the next process with the shortest remaining time
        current_process = get_next_sjf_process(current_time);
        
        if (current_process != NULL) {
            // Process the current process
            printf("T%d : P%d - Burst left %2d, Wait time %d, Turnaround time %d\n",
                current_time,
                current_process->id,
                current_process->remaining_time,
                current_process->wait_time,
                current_process->turnaround_time);

            // Decrease remaining time
            current_process->remaining_time--;
                
            // If process has finished execution
            if (current_process->remaining_time == 0) {
                current_process->completed = true;
            }
        }

        // Update wait times and turnaround times
        update_wait_times(current_time, current_process ? current_process->id : -1);
        update_turnaround_times(current_time);
        // Increment current time
        current_time++;
    }
}

/**
 * Simulates Round Robin scheduling algorithm
 * Preemptive: processes get fixed time quantum then switch
 * @param quantum Time slice given to each process
 */
void simulate_round_robin(int quantum) {
    printf("Round Robin with Quantum %d\n", quantum);
    int current_time = 0;        // Simulation time
    int current_process = 0;     // Index of the current process being executed
    int time_in_quantum = 0;     // Time spent on the current process in the current quantum

    // Loop until all processes are complete
    while (!all_processes_complete()) {
        // Find next uncompleted process that has arrived
        while (current_process < num_processes && 
               (processes[current_process].completed || 
                processes[current_process].arrival_time > current_time)) {
            // Move to the next process
            current_process = (current_process + 1) % num_processes;
            time_in_quantum = 0;  // Reset quantum time when switching processes
        }

        // If the process is ready to execute
        if (!processes[current_process].completed && 
            processes[current_process].arrival_time <= current_time) {
            
            // Process the current process
            printf("T%d : P%d - Burst left %2d, Wait time %d, Turnaround time %d\n",
                current_time,
                processes[current_process].id,
                processes[current_process].remaining_time,
                processes[current_process].wait_time,
                processes[current_process].turnaround_time);

            // Decrease remaining time and quantum time
            processes[current_process].remaining_time--;
            time_in_quantum++;

            // If process has finished execution
            if (processes[current_process].remaining_time == 0) {
                processes[current_process].completed = true;
                time_in_quantum = 0;
                // Move to the next process
                current_process = (current_process + 1) % num_processes;
            }
            // If time quantum has been reached
            else if (time_in_quantum == quantum) {
                time_in_quantum = 0;
                // Move to the next process
                current_process = (current_process + 1) % num_processes;
            }
        }

        // Update wait times and turnaround times
        update_wait_times(current_time, current_process);
        update_turnaround_times(current_time);
        // Increment current time
        current_time++;
    }
}

/**
 * Finds the process with shortest remaining time that has arrived
 * @param current_time Current simulation time
 * @return Pointer to process with shortest remaining time, or NULL if none available
 */
Process* get_next_sjf_process(int current_time) {
    Process* shortest = NULL;       // Pointer to the shortest process
    int shortest_time = INT_MAX;    // Shortest remaining time found so far

    // Iterate over all processes
    for (int i = 0; i < num_processes; i++) {
        // Check if process is not completed and has arrived
        if (!processes[i].completed && 
            processes[i].arrival_time <= current_time) {
            // Find the process with the shortest remaining time
            if (processes[i].remaining_time < shortest_time) {
                shortest_time = processes[i].remaining_time;
                shortest = &processes[i];
            }
        }
    }

    return shortest;  // Return the process with shortest remaining time
}

/**
 * Updates wait times for all processes in ready queue
 * @param current_time Current simulation time
 * @param active_process_id ID of currently running process (-1 if none)
 */
void update_wait_times(int current_time, int active_process_id) {
    for (int i = 0; i < num_processes; i++) {
        // If the process is in the ready queue (arrived but not completed and not the active process)
        if (!processes[i].completed && 
            processes[i].arrival_time <= current_time && 
            i != active_process_id) {
            processes[i].wait_time++;  // Increment wait time
        }
    }
}

/**
 * Updates turnaround times for all active processes
 * @param current_time Current simulation time
 */
void update_turnaround_times(int current_time) {
    for (int i = 0; i < num_processes; i++) {
        // If the process has arrived and not yet completed
        if (!processes[i].completed && 
            processes[i].arrival_time <= current_time) {
            processes[i].turnaround_time++;  // Increment turnaround time
        }
    }
}

/**
 * Checks if all processes have completed execution
 * @return true if all processes are complete, false otherwise
 */
bool all_processes_complete() {
    for (int i = 0; i < num_processes; i++) {
        if (!processes[i].completed) {
            return false;  // At least one process is not completed
        }
    }
    return true;  // All processes are completed
}

/**
 * Prints final statistics for all processes
 * Includes individual process stats and overall averages
 */
void print_final_stats() {
    double total_wait_time = 0;         // Sum of wait times for all processes
    double total_turnaround_time = 0;   // Sum of turnaround times for all processes

    // Iterate over all processes to print their statistics
    for (int i = 0; i < num_processes; i++) {
        printf("\nP%d\n", i);
        printf("\tWaiting time:\t\t%3d\n", processes[i].wait_time);
        printf("\tTurnaround time:\t%3d\n", processes[i].turnaround_time);
        
        total_wait_time += processes[i].wait_time;
        total_turnaround_time += processes[i].turnaround_time;
    }
    // Print average statistics
    printf("\nTotal average waiting time:\t%.1f\n", total_wait_time / num_processes);
    printf("Total average turnaround time:\t%.1f\n", total_turnaround_time / num_processes);
}

/**
 * Main function - Entry point of the program
 * Handles command line arguments and runs selected scheduling algorithm
 */
int main(int argc, char *argv[]) {
    // Check for minimum number of arguments
    if (argc < 3) {
        printf("Usage: %s [-f|-s|-r <quantum>] <input_file>\n", argv[0]);
        return 1;
    }

    const char* algorithm = argv[1];  // Scheduling algorithm option
    const char* filename;             // Input file name
    int quantum = 0;                  // Time quantum for Round Robin

    // Handle Round Robin specific argument
    if (strcmp(algorithm, "-r") == 0) {
        if (argc < 4) {
            printf("Error: Round Robin requires a time quantum\n");
            return 1;
        }
        // Parse quantum from command line argument
        quantum = atoi(argv[2]);
        if (quantum <= 0) {
            printf("Error: Time quantum must be positive\n");
            return 1;
        }
        filename = argv[3];  // Input file name is the fourth argument
    } else {
        filename = argv[2];  // Input file name is the third argument
    }

    // Read processes from input file
    read_input_file(filename);

    // Run the selected scheduling algorithm
    if (strcmp(algorithm, "-f") == 0) {
        simulate_fcfs();
    } else if (strcmp(algorithm, "-s") == 0) {
        simulate_sjf();
    } else if (strcmp(algorithm, "-r") == 0) {
        simulate_round_robin(quantum);
    } else {
        printf("Error: Invalid algorithm option\n");
        return 1;
    }

    // Print final statistics
    print_final_stats();
    return 0;
}
