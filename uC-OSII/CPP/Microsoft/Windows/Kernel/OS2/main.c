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
*                                            EXAMPLE CODE
*
* Filename : main.c
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include  <cpu.h>
#include  <lib_mem.h>
#include  <os.h>

#include <stdarg.h>
#include  "app_cfg.h"


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

static  OS_STK  StartupTaskStk[APP_CFG_STARTUP_TASK_STK_SIZE];
# define TASK_STACKSIZE 2048
// OS Shared resources
OS_EVENT* R1;
OS_EVENT* R2;

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  StartupTask (void  *p_arg);
static void dynamic_task(void* p_arg);
static void task1(void* p_arg);
static void task2(void* p_arg);

/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Arguments   : none
*
* Returns     : none
*
* Notes       : none
*********************************************************************************************************
*/

int  main (void)
{
#if OS_TASK_NAME_EN > 0u
    CPU_INT08U  os_err;
#endif


    CPU_IntInit();

    Mem_Init();                                                 /* Initialize Memory Managment Module                   */
    CPU_IntDis();                                               /* Disable all Interrupts                               */
    CPU_Init();                                                 /* Initialize the uC/CPU services                       */
    OSInit();                                                   /* Initialize uC/OS-II                                  */
	//Initialze the output file
	OutFileInit();

    InputFile();
    // Create Mutexes

    INT8U err;
    R1 = OSMutexCreate(63 + R1_PRIO, &err);
    R2 = OSMutexCreate(64 + R2_PRIO, &err);

	// Dynamically create the stack size
	Task_STK = malloc(TASK_NUMBER * sizeof(int*));
    // For each pointer, allocate storage for an array of ints
    int n;
	for (n = 0; n < TASK_NUMBER; n++)
	{
		Task_STK[n] = malloc(TASK_STACKSIZE * sizeof(int));
	}
    for (int i = 0; i < TASK_NUMBER; i++) {
        // Create a dynamic task with the specified parameters
        OSTaskCreateExt(dynamic_task,
            &TaskParameter[i],
            &Task_STK[i][TASK_STACKSIZE - 1],
            TaskParameter[i].TaskPriority,
            TaskParameter[i].TaskID,
            &Task_STK[i][0],
            TASK_STACKSIZE,
            &TaskParameter[i],
            (OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
    }
    OSStart();                                                  /* Start multitasking (i.e. give control to uC/OS-II)   */

    while (DEF_ON) {                                            /* Should Never Get Here.                               */
        ;
    }
}

void dynamic_task(void* p_arg) {
    task_para_set* task_data = (task_para_set*)p_arg;
    INT8U err;
    while (1) {
        OSMutexPend(R1, 0, &err);
        OSMutexPend(R2, 0, &err);
        OSMutexPost(R2);
        OSMutexPost(R1);
        OSTimeDly(task_data->TaskPriority);
    }
}


/*
*********************************************************************************************************
*                                            STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'StartupTask()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

//static  void  StartupTask (void *p_arg)
//{
//   (void)p_arg;
//
//    OS_TRACE_INIT();                                            /* Initialize the uC/OS-II Trace recorder               */
//
//#if OS_CFG_STAT_TASK_EN > 0u
//    OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running            */
//#endif
//
//#ifdef CPU_CFG_INT_DIS_MEAS_EN
//    CPU_IntDisMeasMaxCurReset();
//#endif
//    
//    APP_TRACE_DBG(("uCOS-III is Running...\n\r"));
//
//    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.       */
//        OSTimeDlyHMSM(0u, 0u, 1u, 0u);
//		APP_TRACE_DBG(("Time: %d\n\r", OSTimeGet()));
//    }
//}

