
#pragma once

#include <climits>
#include <cstdint>
#include <cctype>
#include <unistd.h>


// HOP
#define HEADER_SIZE 3

#define NUM_SERVOS 6

#define DEBUG 1

/**
 * Application Configuration
 *
 * Devices the devices and what to use
 *
 */
class Config {

public:

    /**
     * Build a `Config` object from the command line args
     *
     * @param argc
     * @param argv
     */
    Config(int argc, char **argv);
    int processCommandLine(int argc, char **argv);
    void doHelp(const char* programName);

    bool isUartActive();
    bool isJoystickActive();

    const char* getUart();
    const char* getJoystick();
    useconds_t getFrameTime();

    char* getHostname();

private:

    //
    // Config Defaults
    //
    const char* defaultJoystick = "/dev/input/js0";
    const char* defaultUart = "/dev/ttyUSB0";
    bool defaultUseDmx = false;
    bool defaultUseUart = false;
    bool defaultUseJoystick = false;
    uint32_t defaultFrameTime = 25;


    bool useUART = defaultUseUart;
    bool useJoystick = defaultUseJoystick;
    bool useDmx = defaultUseDmx;
    useconds_t frameTime = defaultFrameTime * 1000;

    const char* uartDevice;
    const char* joystickDevice;

    char hostname[HOST_NAME_MAX];




};