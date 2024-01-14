/******************************************************************************

* FreeRTOS V202107.00

* Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.

*

* Permission is hereby granted, free of charge, to any person obtaining a copy of

* this software and associated documentation files (the "Software"), to deal in

* the Software without restriction, including without limitation the rights to

* use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of

* the Software, and to permit persons to whom the Software is furnished to do so,

* subject to the following conditions:

*

* The above copyright notice and this permission notice shall be included in all

* copies or substantial portions of the Software.

*

* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR

* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS

* FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR

* COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER

* IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN

* CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*

* https://www.FreeRTOS.org

* https://github.com/FreeRTOS

*

******************************************************************************/

#include <stdio.h>
#include <pthread.h>
#include <stdint.h>
#include <string.h>
/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

/* Local includes. */
#include "console.h"

/* Priorities at which the tasks are created. */
#define mainQUEUE_RECEIVE_TASK_PRIORITY    ( tskIDLE_PRIORITY + 2 )
#define mainQUEUE_SEND_TASK_PRIORITY       ( tskIDLE_PRIORITY + 1 )
#define mainTEMPERATURE_CONVERSION_TASK_PRIORITY  ( tskIDLE_PRIORITY + 3 )
#define mainMULTIPLICATION_TASK_PRIORITY   ( tskIDLE_PRIORITY + 4 )
#define mainBINARY_SEARCH_TASK_PRIORITY      ( tskIDLE_PRIORITY + 5 )

/* The rate at which data is sent to the queue.  The times are converted from
 * milliseconds to ticks using the pdMS_TO_TICKS() macro. */
#define mainTASK_SEND_FREQUENCY_MS         pdMS_TO_TICKS(200UL )
#define mainTIMER_SEND_FREQUENCY_MS        pdMS_TO_TICKS(400UL )
#define mainTEMPERATURE_CONVERSION_FREQUENCY_MS   pdMS_TO_TICKS(800UL )  
#define mainMULTIPLICATION_TASK_PERIOD_MS   pdMS_TO_TICKS(1600UL)
#define mainBINARY_SEARCH_TASK_PERIOD_MS      pdMS_TO_TICKS(2000UL) 
/* The number of items the queue can hold at once. */
#define mainQUEUE_LENGTH                   (5)

/* The values sent to the queue receive task from the queue send task and the
 * queue send software timer respectively. */
#define mainVALUE_SENT_FROM_TASK           (100UL )
#define mainVALUE_SENT_FROM_TIMER          (200UL )

/* Fixed Fahrenheit temperature value to be converted. */
#define mainFIXED_FAHRENHEIT_TEMPERATURE   104.6

/* Two long integers to be multiplied */
#define mainBIG_NUMBER_1   5678
#define mainBIG_NUMBER_2   3456

/* For task 4 */
static const char* binaryWord = "tasktoexecute";
static const char searchElement = 'x';


/*-----------------------------------------------------------*/

/* The tasks as described in the comments at the top of this file. */
static void prvQueueReceiveTask(void *pvParameters);
static void prvQueueSendTask(void *pvParameters);
static void prvTemperatureConversionTask(void *pvParameters);
static void prvMultiplicationTask(void *pvParameters);
static void prvBinarySearchTask(void *pvParameters);
/* The callback function executed when the software timer expires. */
static void prvQueueSendTimerCallback(TimerHandle_t xTimerHandle);

/*-----------------------------------------------------------*/

/* The queue used by both tasks. */
static QueueHandle_t xQueue = NULL;

/* A software timer that is started from the tick hook. */
static TimerHandle_t xTimer = NULL;

/*-----------------------------------------------------------*/

/*** SEE THE COMMENTS AT THE TOP OF THIS FILE ***/

void main_blinky(void)
{
    const TickType_t xTimerPeriod = mainTIMER_SEND_FREQUENCY_MS;

    /* Create the queue. */
    xQueue = xQueueCreate(mainQUEUE_LENGTH, sizeof(uint32_t));

    if (xQueue != NULL)
    {
        /* Start the two tasks as described in the comments at the top of this
         * file. */
        xTaskCreate(prvQueueReceiveTask, "Rx", configMINIMAL_STACK_SIZE, NULL, mainQUEUE_RECEIVE_TASK_PRIORITY, NULL);
        
        xTaskCreate(prvQueueSendTask, "TX", configMINIMAL_STACK_SIZE, NULL, mainQUEUE_SEND_TASK_PRIORITY, NULL);
        
        xTaskCreate(prvTemperatureConversionTask,"TempConv",configMINIMAL_STACK_SIZE,NULL,mainTEMPERATURE_CONVERSION_TASK_PRIORITY,NULL);
                    
         xTaskCreate(prvMultiplicationTask,"Multiplication",configMINIMAL_STACK_SIZE,NULL,mainMULTIPLICATION_TASK_PRIORITY,NULL);
         
          xTaskCreate(prvBinarySearchTask, "BinarySearch", configMINIMAL_STACK_SIZE, NULL, mainBINARY_SEARCH_TASK_PRIORITY, NULL);

        /* Create the software timer, but don't start it yet. */
        xTimer = xTimerCreate("Timer", xTimerPeriod, pdTRUE, NULL, prvQueueSendTimerCallback);

        if (xTimer != NULL)
        {
            xTimerStart(xTimer, 0);
        }

        /* Start the tasks and timer running. */
        vTaskStartScheduler();
    }

    /* If all is well, the scheduler will now be running, and the following
     * line will never be reached.  If the following line does execute, then
     * there was insufficient FreeRTOS heap memory available for the idle and/or
     * timer tasks	to be created.  See the memory management section on the
     * FreeRTOS web site for more details. */
    for (;;)
    {
    }
}

/*-----------------------------------------------------------*/

/* Temperature */
static void prvTemperatureConversionTask(void *pvParameters)
{
    const TickType_t xTaskFrequency = mainTEMPERATURE_CONVERSION_FREQUENCY_MS;
    TickType_t xNextWakeTime = xTaskGetTickCount();

    (void)pvParameters;

    for (;;)
    {
    	clock_t start_time = clock();
        vTaskDelayUntil(&xNextWakeTime, xTaskFrequency);


        double celsius = (mainFIXED_FAHRENHEIT_TEMPERATURE - 32) * 5 / 9;
        console_print("Temperature: %.2f°F = %.2f°C\n", mainFIXED_FAHRENHEIT_TEMPERATURE, celsius);
        clock_t end_time = clock();
        double execution_time_temp = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
        double WCET_temp=0.25;
        
       if (execution_time_temp > WCET_temp)
        {
    	console_print("Temperature time of execution: %.6f seconds, WCET: %.6f seconds: WCET exceeded. \n", execution_time_temp, WCET_temp);
	console_print("System not schedulable. Stopping...\n");
        vTaskEndScheduler();
        }
      else
        {
        console_print("Temperature time of execution: %.6f seconds, WCET: %.6f seconds: Task successfully completed. \n", execution_time_temp, WCET_temp);
 
            
        }
       

    }
}
/* Big Number*/
static void prvMultiplicationTask(void *pvParameters)
{
    const TickType_t xTaskFrequency = mainMULTIPLICATION_TASK_PERIOD_MS;
    TickType_t xNextWakeTime = xTaskGetTickCount();

    (void)pvParameters;

    for (;;)
    {
    	clock_t start_time = clock();
        vTaskDelayUntil(&xNextWakeTime, xTaskFrequency);

        long int result = mainBIG_NUMBER_1 * mainBIG_NUMBER_2;
        console_print("Multiplication Result: %ld\n", result);
        clock_t end_time = clock();
        double execution_time_bignumber = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
        double WCET_num=0.40;
        
       if (execution_time_bignumber > WCET_num)
        {
    	console_print("Bignumber time of execution: %.6f seconds, WCET: %.6f seconds: WCET exceeded. \n", execution_time_bignumber, WCET_num);
	console_print("System not schedulable. Stopping...\n");
        vTaskEndScheduler();
        }
      else
        {
        console_print("Bignumber time of execution: %.6f seconds, WCET: %.6f seconds: Task successfully completed. \n", execution_time_bignumber, WCET_num);
 
            
        }
    }
}

/* Binary search*/
static void prvBinarySearchTask(void *pvParameters)
{
    const TickType_t xTaskFrequency = mainBINARY_SEARCH_TASK_PERIOD_MS;
    TickType_t xNextWakeTime = xTaskGetTickCount();

    (void)pvParameters;

    for (;;)
    {
        clock_t start_time = clock();

        int length = strlen(binaryWord);
        int result = -1;

        for (int i = 0; i < length; ++i)
        {
            if (binaryWord[i] == searchElement)
            {
                result = i;
                break;
            }
        }

        // Mesurer le temps de fin d'exécution.
        clock_t end_time = clock();

        if (result != -1)
        {
            console_print("Binary search: Element '%c' found at position %d in the word '%s'\n", searchElement, result, binaryWord);
        }
        else
        {
            console_print("Binary search: Element '%c' not found in the word '%s'\n", searchElement, binaryWord);
        }

        double execution_time_binarysearch = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
        double WCET_bs = 0.45;

        if (execution_time_binarysearch > WCET_bs)
        {
            console_print("Binary search time of execution: %.6f seconds, WCET: %.6f seconds: WCET exceeded. \n", execution_time_binarysearch, WCET_bs);
            console_print("System not schedulable. Stopping...\n");
            vTaskEndScheduler();
        }
        else
        {
            console_print("Binary search time of execution: %.6f seconds, WCET: %.6f seconds: Task successfully completed. \n", execution_time_binarysearch, WCET_bs);
        }

        vTaskDelayUntil(&xNextWakeTime, xTaskFrequency);
    }
}


static void prvQueueSendTask(void *pvParameters)
{
    TickType_t xNextWakeTime;
    const TickType_t xBlockTime = mainTASK_SEND_FREQUENCY_MS;
    const uint32_t ulValueToSend = mainVALUE_SENT_FROM_TASK;

    /* Prevent the compiler warning about the unused parameter. */
    (void)pvParameters;

    /* Initialise xNextWakeTime - this only needs to be done once. */
    xNextWakeTime = xTaskGetTickCount();

    for (;;)
    {
        /* Place this task in the blocked state until it is time to run again.
         * The block time is specified in ticks, pdMS_TO_TICKS() was used to
         * convert a time specified in milliseconds into a time specified in ticks.
         * While in the Blocked state this task will not consume any CPU time. */
        vTaskDelayUntil(&xNextWakeTime, xBlockTime);

        /* Send to the queue - causing the queue receive task to unblock and
         * write to the console.  0 is used as the block time so the send operation
         * will not block - it shouldn't need to block as the queue should always
         * have at least one space at this point in the code. */
        xQueueSend(xQueue, &ulValueToSend, 0U);
    }
}

static void prvQueueSendTimerCallback(TimerHandle_t xTimerHandle)
{
    const uint32_t ulValueToSend = mainVALUE_SENT_FROM_TIMER;

    /* This is the software timer callback function.  The software timer has a
     * period of two seconds and is reset each time a key is pressed.  This
     * callback function will execute if the timer expires, which will only happen
     * if a key is not pressed for two seconds. */

    /* Avoid compiler warnings resulting from the unused parameter. */
    (void)xTimerHandle;

    /* Send to the queue - causing the queue receive task to unblock and
     * write out a message.  This function is called from the timer/daemon task, so
     * must not block.  Hence the block time is set to 0. */
    xQueueSend(xQueue, &ulValueToSend, 0U);
}

static void prvQueueReceiveTask(void *pvParameters)
{
    uint32_t ulReceivedValue;

    /* Prevent the compiler warning about the unused parameter. */
    (void)pvParameters;

    for (;;)
    {
        /* Wait until something arrives in the queue - this task will block
         * indefinitely provided INCLUDE_vTaskSuspend is set to 1 in
         * FreeRTOSConfig.h.  It will not use any CPU time while it is in the
         * Blocked state. */
        xQueueReceive(xQueue, &ulReceivedValue, portMAX_DELAY);

        /* To get here something must have been received from the queue, but
         * is it an expected value?  Normally calling printf() from a task is not
         * a good idea.  Here there is lots of stack space and only one task is
         * using console IO so it is ok.  However, note the comments at the top of
         * this file about the risks of making Linux system calls (such as
         * console output) from a FreeRTOS task. */
        if (ulReceivedValue == mainVALUE_SENT_FROM_TASK)
        {
            console_print("Working\n");
        }
        else if (ulReceivedValue == mainVALUE_SENT_FROM_TIMER)
        {
            console_print("Good Working\n");
        }
        else
        {
            console_print("Unexpected message\n");
        }
    }
}








