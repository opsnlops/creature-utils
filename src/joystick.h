
#pragma once

#include <fcntl.h>
#include <cstdio>
#include <unistd.h>
#include <linux/joystick.h>


#define JOYSTICK_DEVICE  "/dev/input/js0"


int open_joystick(char *device);

void close_joystick(int fd);

int read_joystick_event(int fd, struct js_event *event);

ssize_t js_get_axis_event_value(struct js_event *event);