#include <stdint.h>
#include "main.h"
#include "Elevator.h"

void CLI_Transmit(uint8_t *pData);
void CLI_Receive(uint8_t *pData, uint16_t size);
void promptUser(void);
void processCommand(uint8_t *user_input);
void prepareCLI();
void moveCursorToLine(unsigned line, unsigned column);
void CLI_Transmit_Status_Window(uint8_t *pData);
void CLI_UpdateElevatorStatus();