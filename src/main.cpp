
#include <cstdio>
#include <unistd.h>
#include <pthread.h>
#include <termios.h>
#include <cstring>
#include <linux/joystick.h>

#include "log.h"

#include "config.h"
#include "ola.h"
#include "joystick.h"
#include "servo.h"
#include "uart.h"



// How long should we sleep on each update run?
useconds_t sleep_time = 25 * 1000;  // 25ms, which is 40Hz


// Take the lazy way out and keep our file descriptors in the global scope 😅
int joystick_fd = 0;
int uart_fd = 0;

// An array of servos
servo servos[NUM_SERVOS] = {};

// HOP
#define HEADER_SIZE 3
u_int8_t header[HEADER_SIZE] = {0x48, 0x4F, 0x50};


Config* config = nullptr;


/**
 * Sends updates down the wire if the value of one of the servos changed
 * @param ptr
 * @return
 */
[[noreturn]] void *send_updates_thread(void *ptr)
{

    // Buffer for the bytes we're going to send
    u_int8_t send_buffer[HEADER_SIZE + NUM_SERVOS + 1];

    bool update_needed = false;

    while(true) {

        // Walk each servo and see if updates are needed
        for(auto & servo : servos) {
            if(servo_needs_update(&servo)) {
                update_needed = true;
                break;
            }
        }

        if(update_needed) {

            u_int8_t bytes = 0;
            memset(&send_buffer, '\0', HEADER_SIZE + NUM_SERVOS + 1);
            memcpy(&send_buffer, &header, HEADER_SIZE);

            // Copy out the state of the servos to the buffer, skipping the size of the header
            for(auto & servo : servos) {

                send_buffer[HEADER_SIZE + bytes++] = servo.requested_value;
                servo_requested_to_current(&servo);
            }

            // Write this out to the port
            write(uart_fd, &send_buffer, HEADER_SIZE + NUM_SERVOS + 1);

#ifdef DEBUG
            for(unsigned char i : send_buffer)
            {
                printf("0x%x ", i);
            }
            // End the line and flush
            printf("\n");
            fflush(stdout);
#endif
        }

        update_needed = false;
        usleep(sleep_time); // Simulate 40Hz

    }
}

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

    if(!config->useJoystick)
    {
        log_warn("useJoystick is false");
        config->doHelp(programName);
        exit(0);
    }

    ola_test();

    // Set up the servos
    init_servo(&servos[0], 3, false);
    init_servo(&servos[1], 4, true);

    joystick_fd = open_joystick(config->getJoystick());
    printf("opened %s\n", config->getJoystick());

    //uart_fd = open_uart(uart_device, B57600);
    //printf("opened %s\n", uart_device);

    pthread_t reader_thread, updater_thread;

    // Go little threads go!
    pthread_create(&reader_thread, nullptr, joystick_reader_thread, nullptr);
    //pthread_create(&updater_thread, nullptr, send_updates_thread, nullptr);

    // Wait for the threads to join us!
    pthread_join( reader_thread, nullptr);
    //pthread_join( updater_thread, nullptr);

    close_joystick(joystick_fd);
    //close_uart(uart_fd);
    return 0;

}
