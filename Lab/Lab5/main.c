#include "stm32f10x.h"  // This include might differ based on your specific STM32 library setup.
#include "CLI.h"
#include "util.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define mainBLINKY_TASK_PRIORITY (tskIDLE_PRIORITY+1)

QueueHandle_t xQueue;
QueueHandle_t xCLIQueue;

static void vBlinkTask( void * parameters);

unsigned frequency = 5;


int main(void)
{
    clockInit();
    usart_init();

    xQueue = xQueueCreate(1, sizeof(char));
    if (xQueue == NULL)
    {
        while(1);
    }

    xTaskCreate(vBlinkTask, "Blinky", configMINIMAL_STACK_SIZE, NULL, mainBLINKY_TASK_PRIORITY, NULL);

    // Start the scheduler.
    vTaskStartScheduler();

}

static void vBlinkTask( void * parameters)
{
	char message;
	
    for (;;)
    {
			if (xQueueReceive(xQueue, &message, portMAX_DELAY) == pdPASS)
					{
						 if (message == '1') {
								frequency += 1;
							}

						else if (message == '2' && frequency > 1) {
								frequency -= 1;
						}
	
						xQueueSend(xCLIQueue, &message, 0);
					}
					GPIOA->ODR |= (1u << 5);
					vTaskDelay(1000);
					GPIOA->ODR &= ~(1u << 5);
					vTaskDelay(1000);
		}
}


/*int main(void) {

    // Initialize the CLI and other necessary initializations
		clockInit();
    serial_open();
		led_on();

    // Initialize Terminal
    CLI_InitTerminal();

    while (1) {
        CLI_Process();
    }
}
*/

