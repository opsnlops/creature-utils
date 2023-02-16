
#pragma once

#include <unistd.h>
#include <pthread.h>
#include <termios.h>

#include "common/config.h"


class DMX {

public:
    DMX(Config* config);
    ~DMX();

    pthread_t start();

    int getSocketFd();
    Config* getConfig();

    char* getHostBanner();
    uint16_t getHostBannerLength();

    [[noreturn]] static void *dmx_sender_thread(void* config);

private:

    char* hostBanner;
    uint16_t hostBannerLength;

    int socketFd;

    Config* config;

    pthread_t dmx_sender_thread_handle;

};