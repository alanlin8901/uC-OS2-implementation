# RTOS Scheduler and Resource Management Experiments on μC/OS-II

This repository contains three assignments exploring real-time scheduling and resource management techniques using μC/OS-II. These experiments are designed to deepen understanding of how task schedulers and synchronization mechanisms affect real-time system behavior.

## Environment

- **RTOS**: μC/OS-II
- **Language**: C
- **Target**: Real-time task simulation with trace logging

---

## Assignment 1: RM and FIFO Scheduling

### Description

This experiment implements and compares two scheduling algorithms:

- **Rate Monotonic (RM)**: A fixed-priority scheduling algorithm where tasks with shorter periods are given higher priority.
- **First-In-First-Out (FIFO)**: Tasks are scheduled in the order they are created, without preemption based on deadlines or period.

### Features

- Fixed task periods and execution times
- Task metadata logging:
  - Start and end times
  - Response time
  - Preemption count
  - Deadline miss count

### Objective

- Compare preemptive (RM) and non-preemptive (FIFO) scheduling policies.
- Evaluate real-time task behaviors such as latency, preemption, and deadline satisfaction.

---

## Assignment 2: CUS and EDF Scheduling

### Description

This experiment adds:

- **CUS (Custom Scheduler)**: A user-defined scheduler to test alternative priority schemes (e.g., random, manually assigned).
- **Earliest Deadline First (EDF)**: A dynamic-priority scheduling algorithm that prioritizes tasks with the earliest absolute deadlines.

### Features

- Dynamic task set with varying deadlines
- EDF deadline management using system time
- Real-time priority assignment and reordering

### Objective

- Implement and compare static (CUS) vs dynamic (EDF) scheduling.
- Study deadline satisfaction and flexibility under system load.

---

## Assignment 3: NPCS and CPP Synchronization

### Description

This experiment focuses on synchronization in shared-resource scenarios using:

- **NPCS (Non-Preemptible Critical Section)**: Prevents task preemption during critical section execution.
- **CPP (Priority Inheritance Protocol)**: Boosts priority of lower-priority tasks holding a resource needed by a higher-priority task.

### Features

- Resource locking and release mechanisms
- Detection and resolution of priority inversion
- Task trace logs for timing and resource usage

### Objective

- Analyze and mitigate priority inversion problems.
- Understand trade-offs between responsiveness and data integrity.

---