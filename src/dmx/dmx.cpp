
#include <climits>
#include <unistd.h>
#include <cstring>


#include "dmx/dmx.h"

#include "e131.h"

#include "common/config.h"
#include "log.h"
#include "joystick/servo.h"


extern servo servos[NUM_SERVOS];


DMX::DMX(Config* config) {

    log_trace("starting up the DMX sender");

    this->config = config;

    char tempBuffer[HOST_NAME_MAX + 25];
    memset(&tempBuffer, '\0', HOST_NAME_MAX + 25);
    snprintf(tempBuffer, HOST_NAME_MAX + 24, "E1.31 client on %s", config->getHostname());

    hostBannerLength = strlen(tempBuffer);
    hostBanner = (char*)malloc(sizeof(char) + hostBannerLength + 1);
    strncpy(hostBanner, tempBuffer, hostBannerLength);

    log_debug("set the host banner to: %s, len: %d", hostBanner, hostBannerLength);

}


pthread_t DMX::start() {

    log_debug("starting up the DMX client");

    // create a socket for E1.31
    if ((socketFd = e131_socket()) < 0) {
        log_fatal("Unable to open socket for the E1.31 client");
        exit(0);
    }

    log_debug("socket created");

    pthread_create(&this->dmx_sender_thread_handle, nullptr, dmx_sender_thread, (void*)this);
    return this->dmx_sender_thread_handle;
}

int DMX::getSocketFd() {
    return socketFd;
}

Config* DMX::getConfig() {
    return config;
}

char* DMX::getHostBanner() {
    return hostBanner;
}

uint16_t DMX::getHostBannerLength() {
    return hostBannerLength;
}


[[noreturn]] void *DMX::dmx_sender_thread(void *appConfig) {

    auto* dmx = (DMX*)appConfig;

    e131_packet_t packet;
    e131_addr_t dest;

    e131_pkt_init(&packet, dmx->config->getUniverse(), NUM_SERVOS);
    memcpy(&packet.frame.source_name, dmx->getHostBanner(), dmx->getHostBannerLength());
    if (e131_set_option(&packet, E131_OPT_PREVIEW, false) < 0)
        log_fatal("unable to set e131 packet option");

    // Set the target
    if(dmx->config->getNetworkMode() == Config::unicast ) {
        if (e131_unicast_dest(&dest, dmx->getConfig()->getUnicastTargetAddress(), E131_DEFAULT_PORT) < 0)
            log_fatal("unable to set e131 destination");

        log_debug("using unicast");
    }
    else {
        if (e131_multicast_dest(&dest, 1, E131_DEFAULT_PORT) < 0)
            log_fatal("unable to set e131 destination");

        log_debug("using mutlicast");
    }
    log_debug("destination set");

    // This is helpful for debugging
    //e131_pkt_dump(stdout, &packet);

    log_trace("starting to send updates");

    while(true)
    {
        for (size_t pos=0; pos<NUM_SERVOS; pos++)
            packet.dmp.prop_val[pos + 1] = servos[pos].requested_value;
        if (e131_send(dmx->getSocketFd(), &packet, &dest) < 0) {
            log_error("unable to send e131 packet");
            e131_pkt_dump(stdout, &packet);
        }
        packet.frame.seq_number++;

        //update_needed = false;
        usleep(dmx->getConfig()->getFrameTime()); // Defaults to 40Hz

    }

}

DMX::~DMX() {

}