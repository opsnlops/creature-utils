
#include <cstdio>
#include <unistd.h>
#include <pthread.h>
#include <termios.h>
#include <cstring>
#include <linux/joystick.h>

#include "joystick.h"
#include "servo.h"
#include "uart.h"

#define NUM_SERVOS 2

#define DEBUG 1

// How long should we sleep on each update run?
useconds_t sleep_time = 25 * 1000;  // 25ms, which is 40Hz

char *joystick_device = "/dev/input/js0";
char *uart_device = "/dev/ttyUSB0";

// Take the lazy way out and keep our file descriptors in the global scope 😅
int joystick_fd = 0;
int uart_fd = 0;

// An array of servos
servo servos[NUM_SERVOS] = {};

// HOP
#define HEADER_SIZE 3
u_int8_t header[HEADER_SIZE] = {0x48, 0x4F, 0x50};

void *joystick_reader_thread(void *ptr)
{
    js_event event = {};

    // Wait for an event and update the array of servos if it's
    // one of the axes we care about.
    while (read_joystick_event(joystick_fd, &event) == 0) {

        // If this is an axis event, process it
        if (event.type == JS_EVENT_AXIS) {

#ifdef DEBUG
            printf("*");
#endif
            // Walk the servo list and see if this is one we care about
            for(auto & servo : servos) {
                if(servo.joystick_axis == event.number)
                {
                    request_servo_value(&servo, js_get_axis_event_value(&event));
                    break;
                }
            }
        }

        // Ignore the other event types for now (buttons and init)

    }

    return nullptr;
}

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

int main() {

    // Set up the servos
    init_servo(&servos[0], 3, false);
    init_servo(&servos[1], 4, true);

    joystick_fd = open_joystick(joystick_device);
    printf("opened %s\n", joystick_device);

    uart_fd = open_uart(uart_device, B115200);
    printf("opened %s\n", uart_device);

    pthread_t reader_thread, updater_thread;

    // Go little threads go!
    pthread_create(&reader_thread, nullptr, joystick_reader_thread, nullptr);
    pthread_create(&updater_thread, nullptr, send_updates_thread, nullptr);

    // Wait for the threads to join us!
    pthread_join( reader_thread, nullptr);
    pthread_join( updater_thread, nullptr);

    close_joystick(joystick_fd);
    close_uart(uart_fd);
    return 0;

}
