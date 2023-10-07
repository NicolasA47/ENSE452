#include "stm32f10x.h"  // This include might differ based on your specific STM32 library setup.
#include "CLI.h"
#include "util.h"

int main(void) {
    // Initialize the CLI and other necessary initializations
		clockInit();
    serial_open();
		led_on();

    // Show the initial prompt
    CLI_Prompt();

    while (1) {
        CLI_Process();
    }
}