/*
 *
 *   MIT License
 *
 *   Copyright (c) 2017 Sidhin S Thomas
 *
 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in all
 *   copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *   SOFTWARE.
 */



#include <stdio.h>
#include <conio.h>
#include <stdlib.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

#define PRINT_LINE() printf("\n================\n");

#define REPORTER_BLOCK_TIME     pdMS_TO_TICKS(500U)
#define REPORTER_PRIORITY configMAX_PRIORITIES - 1


#define HIGH_PRIORITY       configMAX_PRIORITIES - 1
#define MEDIUM_PRIORITY     HIGH_PRIORITY - 2
#define LOW_PRIORITY        MEDIUM_PRIORITY - 2

#define NUMBER_HIGH_TASK    3
#define NUMBER_MEDIUM_TASK  5
#define NUMBER_LOW_TASK     8
 
#define NUMBER_TASKS    NUMBER_HIGH_TASK + NUMBER_MEDIUM_TASK + NUMBER_LOW_TASK


typedef struct DemoTaskParameters {
    int index;
    int priority;
    char * name;
    TickType_t start_time;
    TickType_t deadline;
    TickType_t end_time;
    uint_fast8_t is_finished;

    TaskHandle_t handle;

    int number;
} DemoTaskParameters;

DemoTaskParameters *task_list_parameters[NUMBER_TASKS] = { 0 };

DemoTaskParameters * initialize_parameter(int index, char * name, int priority, int number) {
    DemoTaskParameters *params = (DemoTaskParameters *) pvPortMalloc(sizeof(DemoTaskParameters));
    params->is_finished = FALSE;
    params->index = index;
    params->name = name;
    params->priority = priority;
    params->start_time = xTaskGetTickCount();
    params->deadline = (TickType_t) (params->start_time +
        DEADLINE_CONSTANT - (int) (params->priority)*PRIORITY_DEADLINE_MULTIPLIER);
    params->end_time = 0;
    params->number = number;
    params->handle = NULL;
    return params;
}

void reporting_task( void *pvParameters );
void demo_task( void *pvParameters );

void main_experiment() {

    xTaskCreate( reporting_task,
                "RT",
                configMINIMAL_STACK_SIZE,
                NULL,
                REPORTER_PRIORITY,
                NULL );
    int j = 0;
    for ( int i = 0; i < NUMBER_HIGH_TASK; ++i ) {
        char *name = pvPortMalloc( sizeof( char ) * 10 );
        sprintf( name, "HD%d", i );
        DemoTaskParameters *pv = initialize_parameter( i, name, HIGH_PRIORITY, rand()%100 );
        task_list_parameters[j++] = pv;
        xTaskCreate( demo_task,
            name,
            configMINIMAL_STACK_SIZE,
            pv,
            HIGH_PRIORITY,
            &pv->handle );
        
    }
    for ( int i = 0; i < NUMBER_MEDIUM_TASK; ++i ) {
        char *name = pvPortMalloc(sizeof(char)*10);        
        sprintf( name, "MD%d", i );
        DemoTaskParameters *pv = initialize_parameter( i, name, MEDIUM_PRIORITY, rand()%100 );
        task_list_parameters[j++] = pv;
        xTaskCreate( demo_task,
            name,
            configMINIMAL_STACK_SIZE,
            pv,
            MEDIUM_PRIORITY,
            &pv->handle );
    }
    for ( int i = 0; i < NUMBER_LOW_TASK; ++i ) {
        char *name = pvPortMalloc( sizeof( char ) * 10 );
        sprintf( name, "LD%d", i );
        DemoTaskParameters *pv = initialize_parameter( i, name, LOW_PRIORITY, rand()%100 );
        task_list_parameters[j++] = pv;
        xTaskCreate( demo_task,
            name,
            configMINIMAL_STACK_SIZE,
            pv,
            LOW_PRIORITY,
            &pv->handle );
    }
    vTaskStartScheduler();
}

void reporting_task( void *pvParameters ) {
    TickType_t xNextWakeTime;
    xNextWakeTime = xTaskGetTickCount();

    for ( ; ; ) {
        PRINT_LINE();
        printf( "Starting Reporter Task at %ld.\n", xNextWakeTime );
        uint8_t tasks_pending = FALSE;
        for ( int i = 0; i < NUMBER_TASKS; ++i ) {
            if ( task_list_parameters[i]->is_finished != TRUE ) {
                tasks_pending = TRUE;
                break;
            }
        }

        if ( tasks_pending == TRUE ) {
            printf( "Tasks are pending.\nGoing to Sleep.\n" );
            vTaskDelayUntil(&xNextWakeTime, REPORTER_BLOCK_TIME);
        }
        else {
            fflush( stdout );
            PRINT_LINE();
            PRINT_LINE();
            printf( "All Tasks Finished Executing.\n" );
            int success = 0;
            for ( int i = 0; i < NUMBER_TASKS; ++i ) {
                DemoTaskParameters *pv = task_list_parameters[i];
                if ( pv->end_time < pv->deadline ) {
                    success++;
                    printf( "Tasks %d: %s executed successfully.\nDeadline:%ld\tFinish Time:%ld\n", 
                        i, pv->name, pv->deadline, pv->end_time );
                }
                else {
                    fprintf( stderr, "Tasks %d: %s Failed to meet deadline.\nDeadline:%ld\tFinish Time:%ld\n",
                        i, pv->name, pv->deadline, pv->end_time );
                }
            }
            PRINT_LINE();
            printf( "Number of Successfull Deadline meets: %d\n", success );
            printf( "Total Number of Tasks: %d\n", NUMBER_TASKS );
            break;
        }

    }
    for ( ;; ); //end of line
}


void demo_task( void *params ) {
    PRINT_LINE();
    DemoTaskParameters *pv = (DemoTaskParameters*) params;
    printf( "Starting Task %d:%s with priority:%d\n", pv->index, pv->name, pv->priority );

    int n = pv->number;

    int flag = TRUE;
    for ( int i = 2; i < n-1; ++i ) {
        printf( "Task %d\t| Priority: %d\t| Processing...\n", pv->index, pv->priority );
        if ( i % n == 0 ) {
            flag = FALSE;
            break;
        }
    }

    if ( flag = TRUE ) {
        PRINT_LINE();
        printf( "Finished.\nTask %d:%s with priority:%d and Number=%d.\nResult: Prime\n", pv->index, pv->name, pv->priority,n);
    }
    else {
        PRINT_LINE();
        printf( "Finished.\nTask %d:%s with priority:%d and Number=%d.\nResult: Not Prime\n", pv->index, pv->name, pv->priority, n );
    }
    pv->end_time = xTaskGetTickCount();
    pv->is_finished = TRUE;
    vTaskDelete( pv->handle );
}