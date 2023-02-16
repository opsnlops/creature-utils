
#pragma once

#include <unistd.h>
#include <pthread.h>
#include <termios.h>

#include "common/config.h"

#include "servo.h"


#define MAX_INFO_LINE_SIZE   50

class InfoDisplay {


public:

    explicit InfoDisplay(Config* config);

    Config* config;

    void start();

    [[noreturn]] static void *update_display(void* arguments);


private:

    pthread_t display_update_thread;

    static void makeServoDisplayString(char* dst, int bufferSize);
    static uint8_t oldServoValues[NUM_SERVOS];

};