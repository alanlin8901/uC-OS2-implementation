# Scheduler and Resource Management Experiments on μC/OS-II

This repository contains six scheduling exploring real-time scheduling.

## Environment

- **System**: μC/OS-II
- **Language**: C
---
### 🧠 Scheduling & Synchronization Techniques

| **Technique**                 | **Type**                    | **Key Feature**                                       | **Objective**                                  |
|------------------------------|-----------------------------|-------------------------------------------------------|------------------------------------------------|
| **Rate Monotonic (RM)**      | Fixed-Priority Scheduling   | Priority based on task period (shorter = higher)      | Analyze static preemptive scheduling behavior   |
| **FIFO**                     | Non-Preemptive Scheduling   | Executes tasks in creation order                     | Evaluate performance without preemption         |
| **Custom Scheduler (CUS)**   | User-Defined Scheduling     | Arbitrary or manual priority assignment               | Explore flexibility in custom policies          |
| **Earliest Deadline First (EDF)** | Dynamic-Priority Scheduling | Chooses task with nearest deadline                    | Maximize CPU utilization and meet deadlines     |
| **NPCS**                     | Synchronization Mechanism   | Disables preemption during critical sections         | Protect shared resources from race conditions   |
| **Priority Inheritance (CPP)** | Synchronization Mechanism   | Temporarily raises priority to avoid inversion        | Resolve priority inversion problems effectively |

---