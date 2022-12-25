
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

private:

    //
    // Config Defaults
    //
    const char* defaultJoystick = "/dev/input/js0";
    const char* defaultUart = "/dev/ttyUSB0";
    bool defaultUseDmx = false;
    bool defaultUseUart = false;
    bool defaultUseJoystick = false;


    bool useUART = defaultUseUart;
    bool useJoystick = defaultUseJoystick;
    bool useDmx = defaultUseDmx;

    const char* uartDevice;
    const char* joystickDevice;




};