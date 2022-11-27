
#include <fcntl.h>
#include <cstdio>
#include <unistd.h>
#include <linux/joystick.h>

#include "joystick.h"

int open_joystick(char *device) {

    int fd = open(device, O_RDONLY);

    if (fd == -1)
        perror("Could not open joystick");

    return fd;
}


void close_joystick(int fd) {
    close(fd);
}


int read_event(int fd, struct js_event *event) {
    ssize_t bytes;

    bytes = read(fd, event, sizeof(*event));

    if (bytes == sizeof(*event))
        return 0;

    /* Error, could not read full event. */
    return -1;
}
