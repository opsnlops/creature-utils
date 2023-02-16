
#include <cstring>
#include <unistd.h>
#include <pthread.h>

#include "joystick/config.h"

#include "log.h"

#include "joystick/display.h"
#include "joystick/servo.h"

extern servo servos[NUM_SERVOS];
uint8_t InfoDisplay::oldServoValues[NUM_SERVOS];

InfoDisplay::InfoDisplay(Config* config)
{
    this->config = config;
    this->display_update_thread = {};

    // Initialize the array of old servo values
    for(unsigned char & oldServoValue : oldServoValues) {
        oldServoValue = 0;
    }

    log_debug("InfoDisplay created");
}

void InfoDisplay::start() {

    pthread_create(&this->display_update_thread, nullptr, update_display, (void*)config);

}


[[noreturn]] void *InfoDisplay::update_display(void *arguments) {

    auto config = (Config*)arguments;

    bool update_needed = false;

    auto buffer = (char*)malloc(sizeof(char) * MAX_INFO_LINE_SIZE);

    while(true) {

        // Walk each servo and see if updates are needed
        for(int i = 0; i < NUM_SERVOS; i++) {
            if (oldServoValues[i] != servos[i].requested_value) {
                update_needed = true;
                break;
            }
        }

        if (update_needed) {

            makeServoDisplayString(buffer, MAX_INFO_LINE_SIZE);
            log_info(buffer);

        }

        // Copy to the oldServoValues buffer
        for(int i = 0; i < NUM_SERVOS; i++)
            oldServoValues[i] = servos[i].requested_value;


        update_needed = false;
        usleep(config->getFrameTime()); // Defaults to 40Hz

    }

}


void InfoDisplay::makeServoDisplayString(char* dst, int bufferSize) {

    char miniBuffer[5] = {};

    // Wipe out the buffer
    memset(dst, '\0', bufferSize);

    int currentChar = 0;
    dst[currentChar++] = '[';

    for(int i = 0; i < NUM_SERVOS; i++) {

        // Zero out the mini buffer
        memset(miniBuffer, '\0', 5);

        dst[currentChar++] = ' ';

        // Copy this value into a mini buffer
        snprintf(miniBuffer, 4, "%-3d", servos[i].requested_value);

        // Now copy it into our buffer
        dst[currentChar++] = miniBuffer[0];
        dst[currentChar++] = miniBuffer[1];
        dst[currentChar++] = miniBuffer[2];
    }

    dst[currentChar++] = ' ';
    dst[currentChar++] = ']';

    log_trace("converted the current state to a string");

}