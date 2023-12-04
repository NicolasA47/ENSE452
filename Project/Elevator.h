// Elevator.h

#ifndef ELEVATOR_H
#define ELEVATOR_H


// Enumeration for elevator states
typedef enum {
    MOVING, DOOR_OPEN, DOOR_CLOSED, EMERGENCY, MAINTENANCE
} ElevatorState;

// Enumeration for elevator direction
typedef enum {
    UP, DOWN, NONE
} Direction;

// Structure to represent the elevator
typedef struct {
    int currentFloor;
    int targetFloor;
    ElevatorState state;
    Direction direction;
} Elevator;

// Function prototypes
void ElevatorControlTask(void *pvParameters);
void InitializeElevator(Elevator *elevator);
void SetTargetFloor(int floor);
void HandleEmergency();

#endif // ELEVATOR_H
