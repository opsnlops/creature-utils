
#include <getopt.h>
#include <cstdlib>
#include <unistd.h>
#include <climits>
#include <cstring>


#include "joystick/config.h"

#include "log.h"


uint8_t header[HEADER_SIZE] = { 0x48, 0x4F, 0x50 };


Config::Config(int argc, char **argv) {

    log_debug("new Config() made");

    // Go look up our hostname
    memset(hostname, '\0', HOST_NAME_MAX);
    if(gethostname(hostname, HOST_NAME_MAX) == -1) {
        log_warn("unable to get hostname");
        memcpy(hostname, "[Unknown]", 9);
    }

    log_debug("my hostname: %s", hostname);

}

bool Config::isUartActive() {
    return useUART;
}

bool Config::isJoystickActive() {
    return useJoystick;
}

bool Config::isSecondJoystickActive() {
    return useSecondJoystick;
}

const char* Config::getUart() {
    return uartDevice;
}

const char* Config::getJoystick() {
    return joystickDevice;
}

const char* Config::getSecondJoystick() {
    return secondJoystickDevice;
}

useconds_t Config::getFrameTime() {
    return frameTime;
}

char* Config::getHostname() {
    return hostname;
}

Config::PacketType Config::getNetworkMode() {
    return networkMode;
}

const char* Config::getUnicastTargetAddress() {
    return unicastTargetAddress;
}

uint16_t Config::getUniverse() {
    return universe;
}

void Config::doHelp(const char* progname) {

    printf("\n");
    printf("usage: %s [--help] [--firehose] [--uart(=device)] [--joystick(=device)] [--dmx] [--frame-time=<frameMs>] [--verbose]\n", progname);

    printf("\n");
    printf("Options:\n");
    printf("  --help                Show this message\n");
    printf("  --uart(=device)       Use UART on [device] or default      (default: %s)\n", defaultUart);
    printf("  --joystick(=device)   Use joystick on [device] or default  (default: %s)\n", defaultJoystick);
    printf("  --second-joystick     Use a second joystick                (default: %s)\n", defaultSecondJoystick);
    printf("  --dmx                 Use DMX                              (default: %s)\n", defaultUseDmx ? "true" : "false");
    printf("  --universe            E1.31 Universe                       (default: %d)\n", defaultUniverse);
    printf("  --frame-time          Number of milliseconds in each frame (default: %d)\n", defaultFrameTime);
    printf("  --multicast           Use multicast networking             (default: %s)\n", defaultNetworkMode == multicast ? "true" : "false");
    printf("  --unicast(=targetIP)  Use unicast networking               (default: %s)\n", defaultNetworkMode == unicast ? "true" : "false");
    printf("  --verbose             Verbose logging\n");
    printf("  --firehose            I wish to drink from the log firehose\n");
    printf("\n");
    printf(" *** Network Info ***\n");
    printf("  - Only one of unicast or multicast can be used at once.\n");
    printf("  - The default unicast target address is %s.\n", defaultUnicastTargetAddress);
    printf("  - The multicast address is automatically calculated based on the universe.\n");
    printf("\n");
    printf("Note: If neither UART or DMX is in use, output will be to stdout only for\n");
    printf("      debugging purposes.\n");
    printf("\n");

};

int Config::processCommandLine(int argc, char **argv) {

    log_debug("processing the command line");

    // If there's no arguments, bail
    if(argc == 1) {
        return 0;
    }

    const char* const short_opts = "vu:j:dhft";
    const option options[] = {
            {"verbose",         no_argument,        nullptr,    'v'},
            {"uart",            optional_argument,  nullptr,    'u'},
            {"joystick",        optional_argument,  nullptr,    'j'},
            {"second-joystick", optional_argument,  nullptr,    's'},
            {"dmx",             no_argument,        nullptr,    'd'},
            {"help",            no_argument,        nullptr,    'h'},
            {"firehose",        no_argument,        nullptr,    'f'},
            {"frame-time",      required_argument,  nullptr,    't'},
            {"unicast",         optional_argument,  nullptr,    'c'},
            {"multicast",       no_argument,        nullptr,    'm'},
            {"universe",        required_argument,  nullptr,    'n'},
            {nullptr,           no_argument,        nullptr,     0 }
    };


    int c;

    while ((c = getopt_long(argc, argv, short_opts, options, nullptr)) != -1) {

        switch (c)
        {
            case 'v':
                log_set_level(LOG_DEBUG);
                break;
            case 'f':
                log_set_level(LOG_TRACE);
                break;
            case 'u':
                useUART = true;
                uartDevice = (optarg == nullptr) ? defaultUart : optarg;
                log_debug("uart device is now %s", uartDevice);
                break;
            case 'j':
                useJoystick = true;
                joystickDevice = (optarg == nullptr) ? defaultJoystick : optarg;
                log_debug("joystick device is now %s", joystickDevice);
                break;
            case 's':
                useSecondJoystick = true;
                secondJoystickDevice = (optarg == nullptr) ? defaultSecondJoystick : optarg;
                log_debug("second joystick device is now %s", secondJoystickDevice);
                break;
            case 'd':
                useDmx = true;
                log_debug("useDmx is now %s", useDmx ? "true" : "false");
                break;
            case 'c':
                networkMode = unicast;
                unicastTargetAddress = (optarg == nullptr) ? defaultUnicastTargetAddress : optarg;
                log_debug("network mode is now unicast");
                log_debug("set the unicast target address to %s", unicastTargetAddress);
                break;
            case 'm':
                networkMode = multicast;
                log_debug("network mode is now multicast");
                break;
            case 't':
                frameTime = atoi(optarg) * 1000;
                log_debug("frame time is now %dus", frameTime);
                break;
            case 'n':
                universe = atoi(optarg);
                log_debug("now using universe %d", universe);
                break;
            default:
                return 0;
        }
    }

    return 1;

}