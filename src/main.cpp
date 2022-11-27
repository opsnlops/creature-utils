
#include <cstdio>
#include <linux/joystick.h>

#include "joystick.h"

int main() {
    int fd = open_joystick(JOYSTICK_DEVICE);

    js_event event = {};

    /* This loop will exit if the controller is unplugged. */
    while (read_event(fd, &event) == 0) {
        switch (event.type) {
            case JS_EVENT_BUTTON:
                printf("Button %u %s\n", event.number, event.value ? "pressed" : "released");
                break;
            case JS_EVENT_AXIS:
                printf("Axis %u %6d\n", event.number, event.value);
                break;
            default:
                /* Ignore init events. */
                break;
        }

        fflush(stdout);
    }

    close_joystick(fd);
    return 0;

}
