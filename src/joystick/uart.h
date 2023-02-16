
#pragma once

#include <cstdio>

#include "common/config.h"

int open_uart(const char* device, int baud);
void close_uart(int fd);
int set_uart_attrs(int fd, int baud, int parity, int wait_time_ms);
[[noreturn]] void *send_updates_thread(void *ptr);
