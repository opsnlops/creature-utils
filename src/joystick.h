
#pragma once

#include <fcntl.h>
#include <cstdio>
#include <unistd.h>
#include <linux/joystick.h>



int open_joystick(const char *device);

void close_joystick(int fd);

int read_joystick_event(int fd, struct js_event *event);

ssize_t js_get_axis_event_value(struct js_event *event);


void *joystick_reader_thread(void *ptr);