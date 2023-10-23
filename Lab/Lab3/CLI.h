#ifndef CLI_H
#define CLI_H

#include <stdint.h>

void CLI_Transmit(uint8_t *pData, uint16_t Size);
void CLI_Receive(uint8_t *pData, uint16_t Size);
void CLI_Init(void);
void CLI_Prompt(void);
void CLI_Process(void);
void CLI_HandleCommand(void);

#endif // CLI_H
