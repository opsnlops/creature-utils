
#include <cstdio>
#include <unistd.h>
#include <pthread.h>
#include <linux/joystick.h>

#include "joystick.h"
#include "servo.h"

#define NUM_SERVOS 2

// How long should we sleep on each update run?
useconds_t sleep_time = 25 * 1000;  // 25ms, which is 40Hz

int fd = 0;

// An array of servos
servo servos[NUM_SERVOS] = {};

void *joystick_reader_thread(void *ptr)
{
    js_event event = {};

    // Wait for an event and update the array of servos if it's
    // one of the axes we care about.
    while (read_joystick_event(fd, &event) == 0) {

        // If this is an axis event, process it
        if (event.type == JS_EVENT_AXIS) {

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

            for(auto & servo : servos) {
                printf("%d ", servo.requested_value);
                servo_requested_to_current(&servo);
            }

            // End the line and flush
            printf("\n");
            fflush(stdout);
        }

        update_needed = false;
        usleep(sleep_time); // Simulate 40Hz

    }
}

int main() {

    // Set up the servos
    init_servo(&servos[0], 3, false);
    init_servo(&servos[1], 4, true);

    fd = open_joystick("/dev/input/js0");

    pthread_t reader_thread, updater_thread;

    // Go little threads go!
    pthread_create(&reader_thread, nullptr, joystick_reader_thread, nullptr);
    pthread_create(&updater_thread, nullptr, send_updates_thread, nullptr);

    // Wait for the threads to join us!
    pthread_join( reader_thread, nullptr);
    pthread_join( updater_thread, nullptr);

    close_joystick(fd);
    return 0;

}
