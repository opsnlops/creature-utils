
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include "common/config.h"

#include "joystick/servo.h"
#include "joystick/uart.h"

extern int uart_fd;
extern servo servos[NUM_SERVOS];
extern uint8_t header[HEADER_SIZE];

extern Config* config;

int open_uart(const char *device, int baud) {

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


/**
 * Sends updates down the wire if the value of one of the servos changed
 * @param ptr
 * @return
 */
[[noreturn]] void *send_updates_thread(void *ptr)
{

    // Buffer for the bytes we're going to send
    u_int8_t send_buffer[HEADER_SIZE + NUM_SERVOS + 1];

    bool update_needed = false;

    while(true) {

        // Walk each servo and see if updates are needed
        for(auto & servo : servos) {
            if(servo_needs_update(&servo)) {
                update_needed = true;
                break;
            }
        }

        if(update_needed) {

            u_int8_t bytes = 0;
            memset(&send_buffer, '\0', HEADER_SIZE + NUM_SERVOS + 1);
            memcpy(&send_buffer, &header, HEADER_SIZE);

            // Copy out the state of the servos to the buffer, skipping the size of the header
            for(auto & servo : servos) {

                send_buffer[HEADER_SIZE + bytes++] = servo.requested_value;
                servo_requested_to_current(&servo);
            }

            // Write this out to the port
            write(uart_fd, &send_buffer, HEADER_SIZE + NUM_SERVOS + 1);

#ifdef DEBUG
            for(unsigned char i : send_buffer)
            {
                printf("0x%x ", i);
            }
            // End the line and flush
            printf("\n");
            fflush(stdout);
#endif
        }

        update_needed = false;
        usleep(config->getFrameTime()); // Defaults to 40Hz

    }
}