

#include <pthread.h>
#include <termios.h>
#include "log.h"

#include "joystick/config.h"
#include "joystick/joystick.h"
#include "joystick/servo.h"
#include "joystick/uart.h"
#include "joystick/display.h"

#include "dmx/dmx.h"

// Take the lazy way out and keep our file descriptors in the global scope 😅
int joystick_fd = 0;
int second_joystick_fd = 0;
int uart_fd = 0;

// An array of servos
servo servos[NUM_SERVOS] = {};

// Our configuration
Config* config = nullptr;


int main(int argc, char **argv) {

    const char* programName = argv[0];

    // Default to INFO logging
    log_set_level(LOG_INFO);

    // Create the config
    config = new Config(argc, argv);
    if(config->processCommandLine(argc, argv) != 1) {
        log_warn("processCommandLine() did not return 1");
        config->doHelp(programName);
        exit(0);
    }

    if(!config->isJoystickActive())
    {
        log_warn("joystick is not in use");
        config->doHelp(programName);
        exit(0);
    }

    // Set up the servos
    init_servo(&servos[0], 0, false);
    init_servo(&servos[1], 1, false);
    init_servo(&servos[2], 2, false);
    init_servo(&servos[3], 3, false);
    init_servo(&servos[4], 4, false);
    init_servo(&servos[5], 5, false);

    if(config->isSecondJoystickActive()) {
        init_servo(&servos[6], 6, false);
        init_servo(&servos[7], 7, false);
        init_servo(&servos[8], 8, false);
        init_servo(&servos[9], 9, false);
        init_servo(&servos[10], 10, false);
        init_servo(&servos[11], 11, false);
    }

    if(config->isJoystickActive()) {
        log_debug("attempting to open %s", config->getJoystick());
        joystick_fd = open_joystick(config->getJoystick());
        log_info("opened joystick: %s", config->getJoystick());
    }

    if(config->isSecondJoystickActive()) {
        log_debug("attempting to open %s", config->getSecondJoystick());
        second_joystick_fd = open_joystick(config->getSecondJoystick());
        log_info("opened joystick: %s", config->getSecondJoystick());
    }

    if(config->isUartActive()) {
        log_debug("attempting to open %s", config->getUart());
        uart_fd = open_uart(config->getUart(), B57600);
        log_info("opened uart: %s", config->getUart());
    }

    auto display = new InfoDisplay(config);
    display->start();
    auto dmx = new DMX(config);
    dmx->start();

    pthread_t reader_thread, second_reader_thread, updater_thread;

    // Go little threads go!
    if(config->isJoystickActive()) pthread_create(&reader_thread, nullptr, joystick_reader_thread, nullptr);
    if(config->isSecondJoystickActive()) pthread_create(&second_reader_thread, nullptr, second_joystick_reader_thread, nullptr);
    if(config->isUartActive()) pthread_create(&updater_thread, nullptr, send_updates_thread, nullptr);

    // Wait for the threads to join us!
    if(config->isJoystickActive()) pthread_join(reader_thread, nullptr);
    if(config->isSecondJoystickActive()) pthread_join(second_reader_thread, nullptr);
    if(config->isUartActive()) pthread_join( updater_thread, nullptr);

    if(config->isJoystickActive()) close_joystick(second_joystick_fd);
    if(config->isSecondJoystickActive()) close_joystick(second_joystick_fd);
    if(config->isUartActive()) close_uart(uart_fd);
    return 0;

}
