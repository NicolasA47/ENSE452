#include "CLI.h"
#include "util.h"
#define CLI_BUFFER_SIZE 50

static char cli_buffer[CLI_BUFFER_SIZE];
static int cli_index = 0;

void CLI_Transmit(uint8_t *pData, uint16_t Size) {
    for (uint16_t i = 0; i < Size; i++) {
        sendbyte(pData[i]);
    }
}

void CLI_Receive(uint8_t *pData, uint16_t Size) {
    for (uint16_t i = 0; i < Size; i++) {
        pData[i] = getbyte();

        // Echo the character back to the host
        sendbyte(pData[i]);
    }
}

void CLI_Prompt(void) {
    const char *prompt = "\n\rWelcome to the CLI' type 'help' for a list of commmands\n\r> ";
    CLI_Transmit((uint8_t *)prompt, strlen(prompt));
}

void CLI_Process(void) {
    char ch = getbyte();

    // Handle backspace key
    if (ch == 0x08 || ch == 0x7F) {
        if (cli_index > 0) {
            cli_index--;
            sendbyte(0x08);  // Cursor backward
            sendbyte(' ');   // Erase
            sendbyte(0x08);  // Cursor backward again
        }
    } else {
        sendbyte(ch);  // Echo character

        if (ch == '\n' || ch == '\r' || cli_index == CLI_BUFFER_SIZE - 1) {
            cli_buffer[cli_index] = '\0';  // Null terminate
            CLI_HandleCommand();
            cli_index = 0;
        } else {
            cli_buffer[cli_index++] = ch;
        }
    }
}

void CLI_HandleCommand(void) {
    if (strcmp(cli_buffer, "led on") == 0) {
        led_on();
        const char *response = "\n\rLED turned ON";
        CLI_Transmit((uint8_t *)response, strlen(response));
    } else if (strcmp(cli_buffer, "led off") == 0) {
        led_off();
        const char *response = "\n\rLED turned OFF";
        CLI_Transmit((uint8_t *)response, strlen(response));
    } else if (strcmp(cli_buffer, "led state") == 0) {
        if (led_get_state()) {
            const char *response = "\n\rLED is ON";
            CLI_Transmit((uint8_t *)response, strlen(response));
        } else {
            const char *response = "\n\rLED is OFF";
            CLI_Transmit((uint8_t *)response, strlen(response));
        }
    } else if (strcmp(cli_buffer, "help") == 0) {
        const char *response = "\n\rAvailable Commands:\n\rled on - Turn LED ON\n\rled off - Turn LED OFF\n\rled state - Get LED state\n\rhelp - Display this help\n\r";
        CLI_Transmit((uint8_t *)response, strlen(response));
    } else {
        const char *response = "\n\rInvalid command. Type 'help' for a list of commands.\n\r";
        CLI_Transmit((uint8_t *)response, strlen(response));
    }

    CLI_Prompt();  // Display prompt again after command processing
}
