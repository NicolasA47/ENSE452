#ifndef MAIN_H
#define MAIN_H

#include "stm32f10x.h"
#include "CLI.h"
#include "util.h"
#include "Elevator.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

// Constants
#define MAX_COMMAND_LENGTH 50
#define MOVE_TASK_PRIORITY (tskIDLE_PRIORITY + 3)
#define CLI_TASK_PRIORITY (tskIDLE_PRIORITY + 1)
#define CLI_STATUS_WINDOW_TASK_PRIORITY (tskIDLE_PRIORITY + 2)
#define RECEIVE_BUFFER_SIZE 50

// Global Variables
extern unsigned frequency;
extern Elevator elevator;
extern uint8_t receive_buffer[RECEIVE_BUFFER_SIZE];
extern uint8_t receive_buffer_index;

extern QueueHandle_t xQueue;
extern QueueHandle_t xCLIQueue;
extern QueueHandle_t xCLIStatusWindowQueue;
extern QueueHandle_t xMoveQueue;
extern TaskHandle_t moveTaskHandle;

// Function Prototypes
void USART2_IRQHandler(void);
void TIM2_IRQHandler(void);

void vCLITask(void * parameters);
void vMoveTask(void * parameters);
void addTravelRequest(int currentFloor, int destinationFloor);
void InitializeElevator(Elevator *elevator);
void updateElevatorMovement();


#endif // MAIN_H
