//============================================================================
// Name        : pru-rc-arm.cpp
// Author      : Andrea Lambruschini
// Version     :
// Copyright   : hi!
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/poll.h>
#include <stdint.h>
#include <pru_rc_lib.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE         512
unsigned char readBuf[MAX_BUFFER_SIZE];

#define NUM_MESSAGES            10000
#define DEVICE_NAME             "/dev/rpmsg_pru31"



using namespace std;

int main() {
	cout << "pru-i2c-arm started" << endl; // prints pru-i2c-arm started
    struct pollfd pollfds[1];
    int result = 0;

    /* Open the rpmsg_pru character device file */
    pollfds[0].fd = open(DEVICE_NAME, O_RDWR);

    /*
     * If the RPMsg channel doesn't exist yet the character device
     * won't either.
     * Make sure the PRU firmware is loaded and that the rpmsg_pru
     * module is inserted.
     */
    if (pollfds[0].fd < 0) {
            printf("Failed to open %s\n", DEVICE_NAME);
            return -1;
    }

    /* The RPMsg channel exists and the character device is opened */
    printf("Opened %s, sending %d messages\n\n", DEVICE_NAME, NUM_MESSAGES);

    /* Send 'hello world!' to the PRU through the RPMsg channel */
    readBuf[0] = PRU_RC_LIB_CMD_ID;
    readBuf[1] = PRU_RC_LIB_CMD_START;
    result = write(pollfds[0].fd, readBuf, 2);
    if (result > 0) {
        printf("Message START sent to PRU\n");
    }

    result = read(pollfds[0].fd, readBuf, 2);
    if (result > 0) {
        printf("Message received from PRU:%d:%d\n", (uint8_t)readBuf[0],(uint8_t)readBuf[0]);
    }

    struct EcapData
    {
        uint8_t cmd[2];
        uint32_t chanValue[8];
    };
    uint32_t* data = (uint32_t*)(readBuf+2);
    printf("Sizeof struct: %d ", sizeof(struct EcapData));

    while(1) {
        readBuf[0] = PRU_RC_LIB_CMD_ID;
        readBuf[1] = PRU_RC_LIB_CMD_GET_DATA;
        result = write(pollfds[0].fd, readBuf, 2);
        if (result > 0) {
            printf("Message GET_DATA sent to PRU\n");
        }
        result = read(pollfds[0].fd, readBuf, sizeof(struct EcapData));
        if (result > 0) {
            printf("Message received from PRU:%d:%d:%d 1\n",readBuf[0], readBuf[1], result);
            if((readBuf[0] == PRU_RC_LIB_CMD_ID)
                    && (readBuf[1] == PRU_RC_LIB_CMD_GET_DATA_RSP)) {
                for(int j = 0; j < 8; j++) {
                    printf("%d , ", data[j]/200);
                }
                printf("\n");
            }
        }
        usleep(100000);
    }

    /* Close the rpmsg_pru character device file */
//    close(pollfds[0].fd);
//
//	return 0;
}
