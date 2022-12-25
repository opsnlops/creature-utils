
#include <getopt.h>
#include <cstdlib>
#include "config.h"

#include "log.h"


uint8_t header[HEADER_SIZE] = { 0x48, 0x4F, 0x50 };


Config::Config(int argc, char **argv) {

    log_debug("new Config() made");

}

bool Config::isUartActive() {
    return useUART;
}
bool Config::isJoystickActive() {
    return useJoystick;
}

const char* Config::getUart() {
    return uartDevice;
}

const char* Config::getJoystick() {
    return joystickDevice;
}

useconds_t Config::getFrameTime() {
    return frameTime;
}

void Config::doHelp(const char* progname) {

    printf("\n");
    printf("usage: %s [--help] [--firehose] [--uart(=device)] [--joystick(=device)] [--dmx] [--frame-time=<frameMs>] [--verbose]\n", progname);

    printf("\n");
    printf("Options:\n");
    printf("  --help                Show this message\n");
    printf("  --uart(=device)       Use UART on [device] or default      (default: %s)\n", defaultUart);
    printf("  --joystick(=device)   Use joystick on [device] or default  (default: %s)\n", defaultJoystick);
    printf("  --dmx                 Use DMX                              (default: %s)\n", defaultUseDmx ? "true" : "false");
    printf("  --frame-time          Number of milliseconds in each frame (default: %d)\n", defaultFrameTime);
    printf("  --verbose             Verbose logging\n");
    printf("  --firehose            I wish to drink from the log firehose\n");
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
            {"verbose",     no_argument,        nullptr,    'v'},
            {"uart",        optional_argument,  nullptr,    'u'},
            {"joystick",    optional_argument,  nullptr,    'j'},
            {"dmx",         no_argument,        nullptr,    'd'},
            {"help",        no_argument,        nullptr,    'h'},
            {"firehose",    no_argument,        nullptr,    'f'},
            {"frame-time",  required_argument,  nullptr,    't'},
            {nullptr,       no_argument,        nullptr,     0 }
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
            case 'd':
                useDmx = true;
                log_debug("useDmx is now %s", useDmx ? "true" : "false");
                break;
            case 't':
                frameTime = atoi(optarg) * 1000;
                log_debug("frame time is now %dus", frameTime);
                break;
            default:
                return 0;
        }
    }

    return 1;

}