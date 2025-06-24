/*
*********************************************************************************************************
*                                            EXAMPLE CODE
*
*               This file is provided as an example on how to use Micrium products.
*
*               Please feel free to use any application code labeled as 'EXAMPLE CODE' in
*               your application products.  Example code may be used as is, in whole or in
*               part, or may be used as a reference only. This file can be modified as
*               required to meet the end-product requirements.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can find our product's user manual, API reference, release notes and
*               more information at https://doc.micrium.com.
*               You can contact us at www.micrium.com.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                              uC/OS-II
*                                          Application Hooks
*
* Filename : app_hooks.c
* Version  : V2.92.13
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include  <os.h>
#include <stdio.h>
#include <stdlib.h>

/*
*********************************************************************************************************
*                                      EXTERN  GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/



/*
*********************************************************************************************************
*********************************************************************************************************
**                                         GLOBAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/
void OutFileInit(void)
{
	//Create the file
	if ((Output_err = fopen_s(&Output_fp, OUTPUT_FILE_NAME, "w")) == 0)
		fclose(Output_fp);
    else
		printf("Error to clear output file");
}

// After reading all TaskParameter[], reverse
void ReverseTaskParameter() {
	int left = 0;
	int right = TASK_NUMBER - 1;
	while (left < right) {
		task_para_set temp = TaskParameter[left];
		TaskParameter[left] = TaskParameter[right];
		TaskParameter[right] = temp;
		left++;
		right--;
	}
}

int compareTasksInit(const void* a, const void* b) {
	task_para_set* taskA = (task_para_set*)a;
	task_para_set* taskB = (task_para_set*)b;
	int deadlineA = taskA->TaskArriveTime + taskA->TaskPeriodic;
	int deadlineB = taskB->TaskArriveTime + taskB->TaskPeriodic;
	if (taskA->TaskID == TASK_NUMBER)
		deadlineA = taskA->TaskArriveTime + taskA->CalculatedDeadline;
	if (taskB->TaskID == TASK_NUMBER)
		deadlineB = taskB->TaskArriveTime + taskB->CalculatedDeadline;
	// Sort by deadline (smaller deadlines first)
	if (deadlineA != deadlineB) {
		return deadlineA - deadlineB; // Deadline Smaller ones go first
	}
	else {
		return taskA->TaskID - taskB->TaskID; // The ID with the smallest number goes first
	}
}


int compareCUSTasksInit(const void* a, const void* b) {
	task_para_set* taskA = (task_para_set*)a;
	task_para_set* taskB = (task_para_set*)b;
	int deadlineA = taskA->TaskArriveTime;
	int deadlineB = taskB->TaskArriveTime;
	// Sort by deadline (smaller deadlines first)
	if (deadlineA != deadlineB) {
		return deadlineA - deadlineB; // Deadline Smaller ones go first
	}
	else {
		return taskA->ApeirodicTaskID - taskB->ApeirodicTaskID; // The ID with the smallest number goes first
	}
}


void PriorityAssignmentInit(task_para_set* TaskParameter, int TASK_NUMBER) {
	// Sort by deadline
	qsort(TaskParameter, TASK_NUMBER, sizeof(task_para_set), compareTasksInit);

	// Assign a unique priority (0 to TASK_NUMBER-1)
	for (int i = 0; i < TASK_NUMBER; i++) {
		TaskParameter[i].TaskPriority = i; // Priority starts at 0, with 0 being the highest priority.
	}

	// Use Bubble Sort to order structure by TaskID
	task_para_set temp;
	for (int i = 0; i < TASK_NUMBER - 1; i++) {
		for (int j = i + 1; j < TASK_NUMBER; j++) {
			if (TaskParameter[i].TaskID > TaskParameter[j].TaskID) {
				temp = TaskParameter[i];
				TaskParameter[i] = TaskParameter[j];
				TaskParameter[j] = temp;
			}
		}
	}
}

int opt_div(int a, int b) {
	float result = (float)(a) / (float)(b) * 100;
	return (int)result;
}

void GivePriorityForAperiodicTask(int aperiodic_task_number) {
	int serverUtility = TaskParameter[TASK_NUMBER - 1].ServerUtility;
	for (int i = 0; i < aperiodic_task_number; i++) {
		// Simulate a CUS deadline based on utility: Deadline = ArrivalTime + ExecutionTime / Utility
		AperiodicTaskParameter[i].TaskPeriodic = opt_div(AperiodicTaskParameter[i].TaskExecutionTime, serverUtility);
		AperiodicTaskParameter[i].CalculatedDeadline =
			AperiodicTaskParameter[i].TaskArriveTime + AperiodicTaskParameter[i].TaskPeriodic;
	}
	// Then sort according to the CalculatedDeadline arrival time
	task_para_set temp;
	for (int i = 0; i < aperiodic_task_number - 1; i++) {
		for (int j = i + 1; j < aperiodic_task_number; j++) {
			if (AperiodicTaskParameter[i].TaskArriveTime > AperiodicTaskParameter[j].TaskArriveTime) {
				temp = AperiodicTaskParameter[i];
				AperiodicTaskParameter[i] = AperiodicTaskParameter[j];
				AperiodicTaskParameter[j] = temp;
			}
		}
	}

	for (int i = 0; i < aperiodic_task_number; i++) {
		if (i == 0) {
			AperiodicTaskParameter[i].CalculatedDeadline =
				AperiodicTaskParameter[i].TaskArriveTime + AperiodicTaskParameter[i].TaskPeriodic;
		}
		else {
			// It is necessary to determine that the arrive time is less than or equal to the CUS deadline before the previous CUS deadline is used.
			if (AperiodicTaskParameter[i - 1].CalculatedDeadline >= AperiodicTaskParameter[i].TaskArriveTime) {
				int j = i - 1;
				while (j >= 0) {
					int prevDeadline = AperiodicTaskParameter[j].CalculatedDeadline;
					int prevAbsDeadline = AperiodicTaskParameter[j].AbsoluteDeadline;

					if (prevAbsDeadline >= prevDeadline) {
						AperiodicTaskParameter[i].CalculatedDeadline = prevDeadline + AperiodicTaskParameter[i].TaskPeriodic;
						break; // When you find the one that meets the conditions, it ends.
					}

					j--; // Continue to search for earlier tasks
				}
			}
		}
	}

}

void InitializeFirstAperiodicTask(void) {
	errno_t err;
	if ((err = fopen_s(&fp, APERIODIC_FILE_NAME, "r")) == 0) {}
	char str[MAX];
	char* ptr;
	char* pTmp = NULL;
	int TaskInfo[INFO], i, j = 0;
	APERIODIC_TASK_NUMBER = 0;
	while (!feof(fp))
	{
		i = 0;
		memset(str, 0, sizeof(str));
		fgets(str, sizeof(str) - 1, fp);
		ptr = strtok_s(str, " ", &pTmp);
		while (ptr != NULL)
		{
			TaskInfo[i] = atoi(ptr);
			ptr = strtok_s(NULL, " ", &pTmp);
			if (i == 0) {
				AperiodicTaskParameter[j].TaskID = TASK_NUMBER;
				AperiodicTaskParameter[j].ApeirodicTaskID = APERIODIC_TASK_NUMBER++;
			}
			else if (i == 1)
				AperiodicTaskParameter[j].TaskArriveTime = TaskInfo[i];
			else if (i == 2)
				AperiodicTaskParameter[j].TaskExecutionTime = TaskInfo[i];
			else if (i == 3)
				AperiodicTaskParameter[j].AbsoluteDeadline = TaskInfo[i];
			i++;
		}
		AperiodicTaskParameter[j].TaskPriority = j;
		j++;
	}
	qsort(AperiodicTaskParameter, APERIODIC_TASK_NUMBER, sizeof(task_para_set), compareCUSTasksInit);

	GivePriorityForAperiodicTask(APERIODIC_TASK_NUMBER);
}

task_para_set AperiodicTaskCrossFunctionParameter[OS_MAX_TASKS];

void InputFile(void)
{
	//Read File
	errno_t err;
	if ((err = fopen_s(&fp, INPUT_FILE_NAME, "r")) == 0)
	{
		//printf("The file 'TaskSet.txt' was opened\n");
	}
	else
	{
		printf("The file 'TaskSet.txt' was not opened\n");
	}
	char str[MAX];
	char* ptr;
	char* pTmp = NULL;
	int TaskInfo[INFO], i, j = 0;
	TASK_NUMBER = 0;
	while (!feof(fp))
	{
		i = 0;
		memset(str, 0, sizeof(str));
		fgets(str, sizeof(str) - 1, fp);
		ptr = strtok_s(str, " ", &pTmp);
		while (ptr != NULL)
		{
			TaskInfo[i] = atoi(ptr);
			ptr = strtok_s(NULL, " ", &pTmp);
			//print Info taks inf
			if (i == 0) {
				TASK_NUMBER++;
				TaskParameter[j].TaskID = TASK_NUMBER;
			}
			else if (i == 1)
				TaskParameter[j].TaskArriveTime = TaskInfo[i];
			else if (i == 2)
				TaskParameter[j].TaskExecutionTime = TaskInfo[i];
			else if (i == 3)
				TaskParameter[j].TaskPeriodic = TaskInfo[i];
			i++;
		}

		// Initial Task Counter
		TaskParameter[j].TaskNumber = 0;
		TaskParameter[j].TaskPriority = j;
		j++;
	}

	fclose(fp);
	if (TASK_NUMBER > 0) {
		TaskParameter[TASK_NUMBER - 1].ServerUtility = TaskParameter[TASK_NUMBER - 1].TaskArriveTime;
		InitializeFirstAperiodicTask();
		if (APERIODIC_TASK_NUMBER > 0) {
			TaskParameter[TASK_NUMBER - 1] = AperiodicTaskParameter[0];
		}
	}
	// Assign the initial priority for each task
	PriorityAssignmentInit(TaskParameter, TASK_NUMBER);

	for (int i = 0; i < APERIODIC_TASK_NUMBER; i++) {
		AperiodicTaskCrossFunctionParameter[i] = AperiodicTaskParameter[i];
	}
	
	// Make sure task order is start from lowest in TCB
	ReverseTaskParameter();
}


/*
*********************************************************************************************************
*********************************************************************************************************
**                                        uC/OS-II APP HOOKS
*********************************************************************************************************
*********************************************************************************************************
*/

#if (OS_APP_HOOKS_EN > 0)

/*
*********************************************************************************************************
*                                  TASK CREATION HOOK (APPLICATION)
*
* Description : This function is called when a task is created.
*
* Argument(s) : ptcb   is a pointer to the task control block of the task being created.
*
* Note(s)     : (1) Interrupts are disabled during this call.
*********************************************************************************************************
*/

void  App_TaskCreateHook (OS_TCB *ptcb)
{
#if (APP_CFG_PROBE_OS_PLUGIN_EN == DEF_ENABLED) && (OS_PROBE_HOOKS_EN > 0)
    OSProbe_TaskCreateHook(ptcb);
#endif
}


/*
*********************************************************************************************************
*                                  TASK DELETION HOOK (APPLICATION)
*
* Description : This function is called when a task is deleted.
*
* Argument(s) : ptcb   is a pointer to the task control block of the task being deleted.
*
* Note(s)     : (1) Interrupts are disabled during this call.
*********************************************************************************************************
*/

void  App_TaskDelHook (OS_TCB *ptcb)
{
    (void)ptcb;
}


/*
*********************************************************************************************************
*                                    IDLE TASK HOOK (APPLICATION)
*
* Description : This function is called by OSTaskIdleHook(), which is called by the idle task.  This hook
*               has been added to allow you to do such things as STOP the CPU to conserve power.
*
* Argument(s) : none.
*
* Note(s)     : (1) Interrupts are enabled during this call.
*********************************************************************************************************
*/

#if OS_VERSION >= 251
void  App_TaskIdleHook (void)
{
}
#endif


/*
*********************************************************************************************************
*                                  STATISTIC TASK HOOK (APPLICATION)
*
* Description : This function is called by OSTaskStatHook(), which is called every second by uC/OS-II's
*               statistics task.  This allows your application to add functionality to the statistics task.
*
* Argument(s) : none.
*********************************************************************************************************
*/

void  App_TaskStatHook (void)
{
}


/*
*********************************************************************************************************
*                                   TASK RETURN HOOK (APPLICATION)
*
* Description: This function is called if a task accidentally returns.  In other words, a task should
*              either be an infinite loop or delete itself when done.
*
* Arguments  : ptcb      is a pointer to the task control block of the task that is returning.
*
* Note(s)    : none
*********************************************************************************************************
*/


#if OS_VERSION >= 289
void  App_TaskReturnHook (OS_TCB  *ptcb)
{
    (void)ptcb;
}
#endif


/*
*********************************************************************************************************
*                                   TASK SWITCH HOOK (APPLICATION)
*
* Description : This function is called when a task switch is performed.  This allows you to perform other
*               operations during a context switch.
*
* Argument(s) : none.
*
* Note(s)     : (1) Interrupts are disabled during this call.
*
*               (2) It is assumed that the global pointer 'OSTCBHighRdy' points to the TCB of the task that
*                   will be 'switched in' (i.e. the highest priority task) and, 'OSTCBCur' points to the
*                  task being switched out (i.e. the preempted task).
*********************************************************************************************************
*/

#if OS_TASK_SW_HOOK_EN > 0
void  App_TaskSwHook (void)
{
#if (APP_CFG_PROBE_OS_PLUGIN_EN > 0) && (OS_PROBE_HOOKS_EN > 0)
    OSProbe_TaskSwHook();
#endif
	
}
#endif


/*
*********************************************************************************************************
*                                   OS_TCBInit() HOOK (APPLICATION)
*
* Description : This function is called by OSTCBInitHook(), which is called by OS_TCBInit() after setting
*               up most of the TCB.
*
* Argument(s) : ptcb    is a pointer to the TCB of the task being created.
*
* Note(s)     : (1) Interrupts may or may not be ENABLED during this call.
*********************************************************************************************************
*/

#if OS_VERSION >= 204
void  App_TCBInitHook (OS_TCB *ptcb)
{
    (void)ptcb;
}
#endif


/*
*********************************************************************************************************
*                                       TICK HOOK (APPLICATION)
*
* Description : This function is called every tick.
*
* Argument(s) : none.
*
* Note(s)     : (1) Interrupts may or may not be ENABLED during this call.
*********************************************************************************************************
*/

#if OS_TIME_TICK_HOOK_EN > 0
void  App_TimeTickHook (void)
{
#if (APP_CFG_PROBE_OS_PLUGIN_EN == DEF_ENABLED) && (OS_PROBE_HOOKS_EN > 0)
    OSProbe_TickHook();
#endif
}
#endif
#endif
