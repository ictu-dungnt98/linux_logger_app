#include <errno.h>  /*Error number definition*/
#include <fcntl.h>  /*File Control Definition*/
#include <stdio.h>  /*Standard input and output definitions*/
#include <stdlib.h> /*Standard function library definition*/
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pthread.h>
#include <termios.h> /*PPSIX Terminal Control Definition*/
#include <unistd.h>  /*Unix Standard Function Definitions*/
#include <string.h>
#include "uart.h"

#define FALSE -1
#define TRUE 0

#define LEN_TO_READ 512

const char* log_file = "uart_log.txt";
int log_fd; 

void clean_stdin(void)
{
    int c;
	
    do {
        c = getchar();
    } while (c != '\n' && c != EOF);
}

void create_command_log_file(char *data)
{
    printf("data: %s\n", data);

    int len_write = write(log_fd, data, strlen(data));
	if (len_write < 0) {
		printf("write log error\n");
	}
}

int signal_handler_IO(int status)
{
	printf("signal_handler_IO\n");
	return 0;
}

static void *thread_tx_handle(void *arg)
{
    int cmd;
	
	printf("%s\n", __FUNCTION__);

    while (1) {
        cmd = 0;

        printf("Select command to send\n");
        printf("1. clear buffer\n");
        scanf("%d", &cmd);

        clean_stdin();

        switch (cmd) {
            case 1: {
                printf("Clear buffer\n");
				create_command_log_file("UART Logger Application - Dungnt98\r\n");
            } break;

            default:
                break;
        }
    }

	printf("%s terminated\n", __FUNCTION__);

    return 0;
}

static void *thread_rx_handle(void *arg)
{
    char buff[LEN_TO_READ];
    int rx_data_len;

    printf("%s\n", __FUNCTION__);

	log_fd = open(log_file, O_RDWR | O_CREAT | O_APPEND);
	if (log_fd < 0) {
        perror("can not create/open log file\n");;
        return 0;
    }

    while (1) {
        rx_data_len = 0;
        memset(buff, 0, sizeof(buff));
        rx_data_len = uartRxNonBlocking(LEN_TO_READ, buff);
        if (rx_data_len > 0) {
			printf("data: %s\n", buff);
			create_command_log_file(buff);    
        }

        usleep(10000);
    }

    printf("%s terminated\n", __FUNCTION__);

    return 0;
}

int main(int argc, char **argv)
{
    int err;

    pthread_t thread_tx, thread_rx;

    if (argc < 2) {
        printf("Usage: %s /dev/ttySn\n", argv[0]);
        return FALSE;
    }

    /* Open /dev/tty* file for work with */
    err = uartOpen(argv[1], 115200, 0, 0);  // Open the serial port and return the file descriptor
	if (err < 0) {
		printf("Error opening %s: %s\n", argv[1], strerror(errno));
		return FALSE;
	}

    /* implement handler */
    if (0 != pthread_create(&thread_tx, NULL, thread_tx_handle, NULL)) {
        perror("Can not create thread transmiter\n");
        exit(-1);
    }

    if (0 != pthread_create(&thread_rx, NULL, thread_rx_handle, NULL)) {
        perror("Can not create thread receiver\n");
        exit(-1);
    }

    pthread_join(thread_tx, NULL);
    pthread_join(thread_rx, NULL);

	return 0;
}

