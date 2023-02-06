
#pragma once

#include <climits>
#include <cstdint>
#include <cctype>
#include <unistd.h>


// HOP
#define HEADER_SIZE 3

#define NUM_SERVOS 12

#define DEBUG 1

/**
 * Application Configuration
 *
 * Devices the devices and what to use
 *
 */
class Config {

public:

    enum PacketType { unicast, multicast };

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
    bool isSecondJoystickActive();

    const char* getUart();
    const char* getJoystick();
    const char* getSecondJoystick();
    useconds_t getFrameTime();

    char* getHostname();

    PacketType getNetworkMode();
    const char* getUnicastTargetAddress();
    uint16_t getUniverse();


private:

    //
    // Config Defaults
    //
    const char* defaultJoystick = "/dev/input/js0";
    const char* defaultSecondJoystick = "/dev/input/js1";
    const char* defaultUart = "/dev/ttyUSB0";
    bool defaultUseDmx = false;
    bool defaultUseUart = false;
    bool defaultUseJoystick = false;
    bool defaultUseSecondJoystick = false;
    uint32_t defaultFrameTime = 25;
    PacketType defaultNetworkMode = multicast;
    uint16_t defaultUniverse = 1;
    const char* defaultUnicastTargetAddress = "10.3.2.11";  // fuzzball


    bool useUART = defaultUseUart;
    bool useJoystick = defaultUseJoystick;
    bool useSecondJoystick = defaultUseSecondJoystick;
    bool useDmx = defaultUseDmx;
    useconds_t frameTime = defaultFrameTime * 1000;

    uint16_t universe = defaultUniverse;

    PacketType networkMode = defaultNetworkMode;
    const char* unicastTargetAddress = defaultUnicastTargetAddress;

    const char* uartDevice;
    const char* joystickDevice;
    const char* secondJoystickDevice;

    char hostname[HOST_NAME_MAX];




};