# FreeRTOS-Source
> This is a Visual Studio Project

>The Windows Simulator and the Kernel of FreeRTOS

This is a modified version of FreeRTOS kernel with an enhanced scheduler. Developing an adaptive scheduler for the FreeRTOS
based on **Ant Colony Optimisation**.

The project aims to introduce **Ant Colony Optimisation** into the scheduler of FreeRTOS
such that during overloaded situation, the scheduling strategy can be switched.  **Ant Colony Optimisation** is an evolutionary algorithm developed after studying the colony of ants, which
gives us an algorithm that can be used to compute shortest distance path with relative ease.
The optimisation will be used in the adaptive scheduler which will select the default strategy
or the Ant Colony Optimisation based on the loading factor of the system.

Along with this has been created a dummy test environment to analyse the performance of each scheduler.

## Problem With Traditional Scheduler
Suppose we have n task with high priority, and one task with priority value one less than
that of the n tasks.Because FreeRTOS uses priority based preemptive scheduling, only the
high priority tasks will get executed even though the difference in priority is not that much.
This will lead to starvation for low priority task.  This is a major drawback of FreeRTOS
scheduler.

Other than above, if we have a task with high priority and takes a lot of time for completion, and we have
n  other  tasks  which  have  less  time  of  execution,  the  default  scheduler  will  schedule  only
the high priority one.  This will lead to the increase in the average completion time of all
the tasks.  Because the low priority tasks have to wait longer.  This is also another major
drawback of the scheduler

## Adaptive Scheduler

Adaptive scheduling basically means the scheduling algorithm will adapt to the changes in
system. Depending upon the load, we will either use traditional or ACO.

## Algorithm

The scheduling algorithm is required to execute when a new task arrives or presently running
task completes.
1.  Construct tour of different ants and produce the task execution sequence
2.  Analyze the task execution sequences generated for available number of processors (
one in our case ).
3.  Update the value of pheromone.
4.  Decide probability of each task and select the task with maximum priority for execu-
tion.

## Project set up

All additional code is hosted within the macro definition `USE_ACO`. Example:
```c
#if ( USE_ACO == 1 )
  // Do Stuff
#endif
```
### Tools and Requirements

* **Microsoft Visual C++**

* **Visual Studio**

  Compatible with 2013+, we used 2017 while developing it.

## Tuning the algorithm

Constants are defined in file `FreeRTOSconfig.h`.

```c
#define ACO_DEBUG                               1
#define ACO_PATHS                               2

#define ACO_PHEROMONE_INIT_VALUE                1
#define acoHVALUE                               12
#define acoALPHA                                1
#define acoBETA                                 1
#define acoPHEROMONE_CONST                      0.2
#define acoPHEROMONE_EVAPORATION_CONST          0.4

#define PERFORMANCE_COEFFECIENT_PRIORITY        1
#define PERFORMANCE_COEFFECIENT_WAIT_TIME       1
#define PERFORMANCE_COEFFECIENT_RANK            1

#define PRIORITY_DEADLINE_MULTIPLIER            20
#define MINIMUM_DEADLINE                        200
#define DEADLINE_CONSTANT configMAX_PRIORITIES * PRIORITY_DEADLINE_MULTIPLIER + MINIMUM_DEADLINE
```

Vary the performance to get results.





