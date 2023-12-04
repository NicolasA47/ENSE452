#include "CLI.h"
#include "util.h"
#include "Elevator.h"

#define SCROLLABLE_REGION_START_LINE 11
#define ELEVATOR_STATUS_LINE 8

static uint8_t clearScreenSeq[] = "\x1b[2J";
static uint8_t scrollSeq[] = "\x1b[11;r";
static uint8_t clearLineSeq[] = "\x1b[K";
static uint8_t cursorPositionSeq[] = "\x1b[11;6H";


static unsigned currentLine = SCROLLABLE_REGION_START_LINE;

void CLI_Transmit(uint8_t *pData) {
    unsigned size = strlen((char *)pData);
    for (unsigned i = 0; i < size; i++) {
        sendByte(pData[i], 1000);
    }
}

void processCommand(uint8_t *user_input) {
    uint8_t *message;
    char command[10];
		char messageBuffer[100];
    int currentFloor, destinationFloor;
		if(elevator.state != EMERGENCY){
			if (strcmp((char *)user_input, "Help") == 0 || strcmp((char *)user_input, "help") == 0) { 
					message = (uint8_t *)
							"Command Descriptions:"
							"\r\ntravel <current_floor> <destination_floor>: Moves the elevator from the current floor to the destination floor"
							"\r\nmaintenance: turns on maintenance mode"
							"\r\nresume: resumes to regular elevator operation"
							"\r\nHelp: Describes the function of each command";
			} 
			
			else if (sscanf((char *)user_input, "%s %d %d", command, &currentFloor, &destinationFloor) == 3) {
					if (strcmp(command, "travel") == 0) {
							if ((currentFloor >= 1 && currentFloor <= 9) && (destinationFloor >= 1 && destinationFloor <= 9)) {
									addTravelRequest(currentFloor, destinationFloor);
									snprintf(messageBuffer, sizeof(messageBuffer), 
									 "Travel request added (Floor %d to Floor %d)", 
									 currentFloor, destinationFloor);

									 message = (uint8_t *)messageBuffer;
							} else {
									message = (uint8_t *)"Invalid floor numbers. Floors should be between 1 and 9.";
							}
					} else {
							message = (uint8_t *)"Unknown command (use command 'help' to see available commands)";
					}
			}
			else if (strcmp((char *)user_input, "maintenance") == 0) {
					// Set elevator state to MAINTENANCE
					elevator.state = MAINTENANCE;
					message = (uint8_t *)"Elevator is now in maintenance mode.";
					xQueueSend(xCLIQueue, &elevator, 0);
			} 
			// Add a check for resuming normal operation
			else if (strcmp((char *)user_input, "resume") == 0) {
					// Resume normal operation (set to IDLE or appropriate state)
					elevator.state = DOOR_OPEN;
					message = (uint8_t *)"Elevator has resumed normal operation.";
					xQueueSend(xCLIQueue, &elevator, 0);
			}
			else {
					message = (uint8_t *)"Invalid command format (use command 'help' to see available commands)";
			}
		}
		else{
			message = (uint8_t*)"Command Can't Be Issued In Emergency Mode, Stand By Till Emergency Services Arrive";
		}
			CLI_Transmit_Status_Window(message);

			// Move to the next line for new command
			currentLine++;
			moveCursorToLine(currentLine, 0);
			CLI_Transmit((uint8_t *)"cmd> ");
			moveCursorToLine(currentLine, 6);
		
}




void CLI_Transmit_Status_Window(uint8_t *pData) {
    // Clear the status window
    for (unsigned i = 0; i <= 7; i++) {
        moveCursorToLine(i, 0);
        CLI_Transmit((uint8_t *)clearLineSeq);
    }

    // Print the status message
    moveCursorToLine(0, 0);
    CLI_Transmit(pData);

    // Restore cursor position to the command line
    moveCursorToLine(SCROLLABLE_REGION_START_LINE, receive_buffer_index);
}






void moveCursorToLine(unsigned line, unsigned column) {
    char cursorPositionDynamic[11];
    snprintf(cursorPositionDynamic, sizeof(cursorPositionDynamic), "\x1b[%d;%dH", line, column);
    CLI_Transmit((uint8_t *)cursorPositionDynamic);
}

void CLI_UpdateElevatorStatus() {
    char elevatorStatus[200];  // Increased size for additional information
    char *directionString;

    // Convert direction enum to string
    switch (elevator.direction) {
        case UP:
            directionString = "Up";
            break;
        case DOWN:
            directionString = "Down";
            break;
        default:
            directionString = "Stationary";
            break;
    }

    moveCursorToLine(ELEVATOR_STATUS_LINE, 0);
    CLI_Transmit((uint8_t *)clearLineSeq);

    snprintf(elevatorStatus, sizeof(elevatorStatus), 
             "Current Floor: %d, Target Floor: %d, State: %s, Direction: %s", 
             elevator.currentFloor, elevator.targetFloor, 
             (elevator.state == MOVING) ? "Moving" : 
             (elevator.state == DOOR_CLOSED) ? "Door Closed" : 
             (elevator.state == DOOR_OPEN) ? "Door Open" : 
             (elevator.state == EMERGENCY) ? "EMERGENCY" : 
             (elevator.state == MAINTENANCE) ? "Maintenance Mode" : "Other",
             directionString);

    CLI_Transmit((uint8_t *)elevatorStatus);

    // Restore cursor position to the command line
    moveCursorToLine(currentLine, 6 + receive_buffer_index);
}


void prepareCLI() {
    CLI_Transmit((uint8_t *)clearScreenSeq);
    CLI_Transmit((uint8_t *)scrollSeq);
    CLI_Transmit((uint8_t *)"Use command 'help' to see available commands.");

    CLI_UpdateElevatorStatus();

    moveCursorToLine(SCROLLABLE_REGION_START_LINE, 0);
    CLI_Transmit((uint8_t *)"cmd> ");

    moveCursorToLine(SCROLLABLE_REGION_START_LINE, 6); 
}

