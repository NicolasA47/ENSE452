#include "main.h"



Elevator elevator; 
unsigned frequency = 5;
uint8_t receive_buffer_index = 0;
uint8_t receive_buffer[RECEIVE_BUFFER_SIZE];

QueueHandle_t xQueue;
QueueHandle_t xCLIQueue;
QueueHandle_t xMoveQueue;
TaskHandle_t moveTaskHandle;


int main(void) {
		clockInit();
		InitializeElevator(&elevator);
	
    serial_open();
		button_init();
		enable_TIM2();
		enableUserLED();
	
		prepareCLI(elevator);

    xMoveQueue = xQueueCreate(10, sizeof(int));
    xCLIQueue = xQueueCreate(10, sizeof(char));

    // Check for Queue creation failure
    if(xCLIStatusWindowQueue == NULL || xMoveQueue == NULL || xCLIQueue == NULL) {
        // Handle Queue creation failure (maybe by LED indication or reset)
    }



    // Create Tasks
    xTaskCreate(vMoveTask, "MoveTask", 256, NULL, MOVE_TASK_PRIORITY, &moveTaskHandle);
    xTaskCreate(vCLITask, "CLITask", 256, NULL, CLI_TASK_PRIORITY, NULL);


    // Start Scheduler
    vTaskStartScheduler();

    // In case the scheduler returns
    while (1) {
    }

    return 0;
}

static void vCLITask(void * parameters) {
		char command;
    for(;;){
        if (xQueueReceive(xCLIQueue, &command, portMAX_DELAY) == pdTRUE) {
            CLI_UpdateElevatorStatus();
        }
    }
}



void addTravelRequest(int requestFloor, int destinationFloor) {
    xQueueSend(xMoveQueue, &requestFloor, portMAX_DELAY);
    xQueueSend(xMoveQueue, &destinationFloor, portMAX_DELAY);
}


void vMoveTask(void *parameters) {
    int targetFloor;
    for (;;) {
        // Wait for a new target floor from the queue
					if (xQueueReceive(xMoveQueue, &targetFloor, portMAX_DELAY) == pdPASS) {
							if(elevator.state != MAINTENANCE && elevator.state != EMERGENCY){
								// Update the target floor and determine the direction
								elevator.targetFloor = targetFloor;
								elevator.direction = (targetFloor > elevator.currentFloor) ? UP : DOWN;

								// Move the elevator until the target floor is reached
								while (elevator.currentFloor != targetFloor) {
										// Simulate elevator movement with a delay
										vTaskDelay(pdMS_TO_TICKS(1000));

										// Update the current floor based on the direction
										if (elevator.direction == UP) {
												elevator.currentFloor++;
										} else if (elevator.direction == DOWN) {
												elevator.currentFloor--;
										}

										// Update the elevator state
										elevator.state = MOVING;
										
										xQueueSend(xCLIQueue, &elevator, 0);

								}
								vTaskDelay(pdMS_TO_TICKS(1000));
								elevator.state = DOOR_OPEN;
								elevator.direction = NONE;
								xQueueSend(xCLIQueue, &elevator, 0);
								vTaskDelay(pdMS_TO_TICKS(1000));
								elevator.state = DOOR_CLOSED;
								xQueueSend(xCLIQueue, &elevator, 0);
							}
    }
	}
}


void InitializeElevator(Elevator *elevator) {
    elevator->currentFloor = 1;
    elevator->targetFloor = 1;
    elevator->state = DOOR_CLOSED;
    elevator->direction = NONE;
}

void USART2_IRQHandler(void) {	
    if (USART2->SR & USART_SR_RXNE) {
        uint8_t received_byte = USART2->DR;
        
        // Check if Enter key is pressed, or the max buffer size was reached
        if (received_byte == '\r' || received_byte == '\n' 
            || receive_buffer_index == RECEIVE_BUFFER_SIZE) {
            sendByte('\n', 1000);
            sendByte('\r', 1000);
            processCommand(receive_buffer);
            // Clear buffer
            memset(receive_buffer, 0, sizeof(receive_buffer));
            receive_buffer_index = 0;
        // Check for backspaces, ASCII values 8 and 127
        } else if (received_byte == 8 || received_byte == 127) {
            if (receive_buffer_index > 0) {
                receive_buffer_index--;
                receive_buffer[receive_buffer_index] = '\0';
                
                sendByte('\b', 1000);   
                sendByte(' ', 1000);    
                sendByte('\b', 1000);   
            }
        } else {
            if (receive_buffer_index < RECEIVE_BUFFER_SIZE - 1) {
                receive_buffer[receive_buffer_index] = received_byte;
                sendByte(received_byte, 1000);
                receive_buffer_index++;
            }
        }
    }
}

void EXTI15_10_IRQHandler(void) {
    if (EXTI->PR & EXTI_PR_PR13) {
        if (elevator.state == EMERGENCY) {            
            elevator.state = DOOR_CLOSED;
            xQueueSend(xCLIQueue, &elevator, 0);
            // Resume tasks related to elevator movement
            vTaskResume(moveTaskHandle);
            // Other tasks that need to be resumed
        }
        else {
            elevator.state = EMERGENCY;
            xQueueSend(xCLIQueue, &elevator, 0);
            CLI_Transmit_Status_Window((uint8_t*)"Emergency Services Have Been Contacted, Please Stand By");
            // Suspend tasks related to elevator movement
            vTaskSuspend(moveTaskHandle);
            // Other tasks that need to be suspended
        }
        EXTI->PR |= EXTI_PR_PR13; // Clear the interrupt
    }
}



