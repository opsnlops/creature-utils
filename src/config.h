
#pragma once


#define NUM_SERVOS 2

#define DEBUG 1

/**
 * Application Configuration
 *
 * Devices the devices and what to use
 *
 */
class Config {

public:

    Config();

    bool useUART;
    char* uartDevice;

    bool useJoystick;
    char* joystickDevice;

    bool useDmx;

};