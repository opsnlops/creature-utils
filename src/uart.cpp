
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>


#include "uart.h"

int open_uart(char *device, int baud) {

    // Open in read/write mode
    int fd = open(&device[0], O_RDWR | O_NOCTTY);
    if (0 > fd) {
        perror(&device[0]);
        exit(-1);
    }

    set_uart_attrs(fd, baud, 0, 1000);

    return fd;
}

void close_uart(int fd)
{
    close(fd);
}

int set_uart_attrs(int fd, int baud, int parity, int wait_time_ms) {
    termios tty = {};

    memset(&tty, 0, sizeof tty);
    if (tcgetattr(fd, &tty) != 0) /* save current serial port settings */
    {
        printf("__LINE__ = %d, error %s\n", __LINE__, strerror(errno));
        return -1;
    }

    cfsetospeed(&tty, baud);
    cfsetispeed(&tty, baud);

    // See http://unixwiz.net/techtips/termios-vmin-vtime.html

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bits

    tty.c_iflag &= ~IGNBRK;         // disable break processing
    tty.c_lflag = 0;                // no signaling chars, no echo,
    tty.c_oflag = 0;                // no remapping, no delays
    tty.c_cc[VMIN] = 0;            // read doesn't block
    tty.c_cc[VTIME] = wait_time_ms / 100;   // This is in tenths of a second for some reason

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off software flow control

    tty.c_cflag |= (CLOCAL | CREAD);        // ignore modem controls,

    // enable reading
    tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
    tty.c_cflag |= parity;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("unable to set serial port attributes");
        return -1;
    }
    return 0;
}
