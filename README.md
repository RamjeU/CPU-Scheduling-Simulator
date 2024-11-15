# CPU Scheduling Simulator in C

A C program that simulates three CPU scheduling algorithms:

- **First-Come, First-Served (FCFS)**
- **Shortest Job First (SJF)**
- **Round Robin (RR)**

## Introduction

This CPU scheduling simulator models the execution of processes using different scheduling algorithms commonly studied in operating systems. It allows users to observe how each algorithm schedules processes, manages CPU time, and impacts process wait times and turnaround times.

## Features

- **Multiple Scheduling Algorithms**: Simulate FCFS, SJF, and Round Robin scheduling.
- **Detailed Simulation Output**: Step-by-step execution details, including time units, active processes, burst times left, wait times, and turnaround times.
- **Performance Metrics**: Calculates and displays individual and average waiting times and turnaround times.
- **Process Management**: Handles multiple processes with customizable burst times.

## How the Program Works

### Overview

The simulator operates by reading process information from an input file, initializing process data structures, and then executing a main simulation loop that progresses time unit by time unit. At each time unit, the scheduler selects a process based on the chosen algorithm and simulates its execution.

### Process Initialization

- **Reading Input**: The program reads a CSV file where each line represents a process in the format `P<id>,<burst_time>`.
- **Process Structure**: Each process is represented by a `Process` struct containing:
  - `id`: Process identifier.
  - `burst_time`: Total CPU time required.
  - `remaining_time`: CPU time left to execute.
  - `arrival_time`: Time at which the process arrives in the ready queue (set to the process ID for simplicity).
  - `wait_time`: Total time the process has been waiting.
  - `turnaround_time`: Total time from arrival to completion.
  - `completed`: A flag indicating whether the process has finished execution.

### Simulation Loop

The main simulation loop continues until all processes are completed. At each time unit (`current_time`), the following steps occur:

1. **Process Selection**:
   - **FCFS**: Selects the next uncompleted process in the order they arrived.
   - **SJF**: Chooses the process with the shortest remaining burst time among those that have arrived.
   - **Round Robin**: Processes are selected in a circular fashion, each receiving a fixed time quantum.

2. **Process Execution**:
   - The selected process executes for one time unit (or up to the time quantum for Round Robin).
   - Its `remaining_time` is decremented.
   - If `remaining_time` reaches zero, the process is marked as `completed`.

3. **Updating Times**:
   - **Wait Times**: For all processes that have arrived but are not currently executing, `wait_time` is incremented.
   - **Turnaround Times**: For all processes that have arrived and not yet completed, `turnaround_time` is incremented.

4. **Output**:
   - The program prints the current time, the active process ID, burst time left, wait time, and turnaround time.

### Completion Check

The simulation continues until the `completed` flag for all processes is `true`.

### Final Statistics

After the simulation ends, the program calculates and prints:

- **Individual Process Statistics**:
  - Waiting time.
  - Turnaround time.
- **Average Statistics**:
  - Total average waiting time.
  - Total average turnaround time.

## Compiling and Running the Program

### Prerequisites

- **C Compiler**: Ensure you have a C compiler like `gcc` installed.

### Compilation

Compile the program using the following command:

```bash
gcc -o scheduler scheduler.c
```

### Running the Program

The program is executed from the command line and requires the following arguments:

```bash
./scheduler [algorithm] [options] <input_file>
```

- `[algorithm]`: The scheduling algorithm to use.
  - `-f`: First-Come, First-Served.
  - `-s`: Shortest Job First.
  - `-r <quantum>`: Round Robin with the specified time quantum.
- `[options]`: Additional options required by the algorithm.
  - `<quantum>`: An integer specifying the time quantum for Round Robin.
- `<input_file>`: Path to the input file containing process information.

### Examples

#### First-Come, First-Served (FCFS)

```bash
./scheduler -f processes.csv
```

#### Shortest Job First (SJF)

```bash
./scheduler -s processes.csv
```

#### Round Robin with Time Quantum of 3

```bash
./scheduler -r 3 processes.csv
```

## Input File Format

The input file should be a text file with each line representing a process in the following format:

```
P<id>,<burst_time>
```

- `P<id>`: The process identifier, where `<id>` is a unique integer.
- `<burst_time>`: The total CPU time required by the process.

**Example `processes.txt`:**

```
P0,4
P1,3
P2,1
P3,2
```

**Note**: The arrival time of each process is set to its process ID (i.e., `P0` arrives at time 0, `P1` at time 1, etc.).

## Detailed Explanation of Scheduling Algorithms

### First-Come, First-Served (FCFS)

- **Type**: Non-preemptive.
- **Process Selection**: Processes are selected in the order of their arrival times.
- **Execution**: Once a process starts executing, it runs to completion before the next process is scheduled.
- **Characteristics**:
  - Simple to implement.
  - Can lead to the "convoy effect," where short processes wait behind long ones.

### Shortest Job First (SJF)

- **Type**: Preemptive (in this implementation).
- **Process Selection**: At each time unit, the process with the shortest remaining CPU burst time is selected.
- **Execution**: If a new process arrives with a shorter burst time, it can preempt the currently running process.
- **Characteristics**:
  - Aims to minimize average waiting time.
  - Can cause starvation for processes with longer burst times.

### Round Robin (RR)

- **Type**: Preemptive with fixed time quantum.
- **Process Selection**: Processes are placed in a circular queue and are given CPU time in slices defined by the time quantum.
- **Execution**: Each process executes for a maximum of one time quantum before the next process is scheduled.
- **Characteristics**:
  - Fair to all processes.
  - Time quantum selection is crucial; too small leads to excessive context switching, too large degrades to FCFS.

## Sample Output Explained

When running the simulator, you will see output similar to the following:

```
First Come First Served
T0 : P0 - Burst left  4, Wait time 0, Turnaround time 0
T1 : P0 - Burst left  3, Wait time 0, Turnaround time 1
T2 : P0 - Burst left  2, Wait time 0, Turnaround time 2
T3 : P0 - Burst left  1, Wait time 0, Turnaround time 3
T4 : P1 - Burst left  3, Wait time 0, Turnaround time 0
T5 : P1 - Burst left  2, Wait time 0, Turnaround time 1
...
```

**Explanation:**

- `T<current_time>`: The current time unit in the simulation.
- `P<id>`: The process currently executing.
- `Burst left`: CPU time remaining for the process.
- `Wait time`: Total time the process has been waiting in the ready queue.
- `Turnaround time`: Total time from process arrival to the current time.

At the end of the simulation, individual and average statistics are displayed:

```
P0
    Waiting time:          0
    Turnaround time:       4

P1
    Waiting time:          3
    Turnaround time:       6

...

Total average waiting time:    2.5
Total average turnaround time: 5.0
```

## Limitations and Assumptions

- **Arrival Times**: Processes arrive at times equal to their IDs for simplicity.
- **No I/O Operations**: The simulator assumes processes are CPU-bound with no I/O interruptions.
- **Fixed Number of Processes**: The maximum number of processes is set to 100 but can be adjusted by changing the `MAX_PROCESSES` macro.
- **No Priority Scheduling**: The simulator does not account for process priorities beyond burst times.

## Customization

- **Adjusting Arrival Times**: Modify the `arrival_time` assignment in the `read_input_file` function to change how arrival times are set.
- **Extending Functionality**: You can extend the `Process` struct and related functions to include additional scheduling algorithms or features like priority levels.

## Error Handling

The program includes basic error handling for:

- **File Operations**: Checks if the input file can be opened.
- **Command-Line Arguments**: Validates the number of arguments and the correctness of options provided.
- **Quantum Value**: Ensures the time quantum for Round Robin is a positive integer.

## License

This project is licensed under the MIT License. You are free to use, modify, and distribute this software. See the [LICENSE](LICENSE) file for more details.
