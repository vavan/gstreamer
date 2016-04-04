#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <gst/gst.h>
#include <string.h>
#include "gst_thread.h"

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define LAPTOP

#ifdef LAPTOP
#define INPUT_DEVICE1 "/dev/input/event3"
#define INPUT_DEVICE0 "/dev/input/event3"
#else
#define INPUT_DEVICE1 "/dev/input/event1"
#define INPUT_DEVICE0 "/dev/input/event0"
#endif

#define EVENT_BUFFER_SIZE 128



typedef struct {
      struct timeval time;
      unsigned short type;
      unsigned short code;
      unsigned int value;
} input_event_t;

int open_shutter_button(char* device) {
	int fd;
	//printf("Open: %s", device);
	fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd == -1) {
		return (-1);
	}

	int flags = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	return fd;
}

int detect_press(char *event) {
	input_event_t *p;
	p = (input_event_t*)event;
	if ((p->type == 1) && (p->code > 0)) {
		if (p->value == 1) {
			return 1;
		}
	}
	//printf("%d %d %d\n", p->type, p->value, p->code);
	return 0;
}

int parse_events(char* buffer, int size) {
	int start = 0;
	while (start + sizeof(input_event_t) <= size) {
		if (detect_press(buffer + start)) {
			return 1;
		}
		start += sizeof(input_event_t);
	}
	return 0;
}



void main_loop(int fd_shutter) {
	char buffer[EVENT_BUFFER_SIZE];

	while (1) {
		int i;
		fd_set fds;
		FD_ZERO(&fds);
		FD_SET(fd_shutter, &fds);


		if (select(fd_shutter + 1, &fds, 0, 0, 0) == 1) {
			int received = read(fd_shutter, buffer, EVENT_BUFFER_SIZE);
			//printf("got %d\n", received);
			
			if (parse_events(buffer, received)) {
				printf("Shutter pressed\n");
				break;
			}
		}
	}
}


int main(int argc, char *argv[])
{
	int fd;
	fd = open_shutter_button(INPUT_DEVICE1);
	if (fd == -1) {
		fd = open_shutter_button(INPUT_DEVICE0);
	}
	if (fd == -1) {
		perror("open_port: Unable to open input device - ");
	} else {
		//printf("device opened\n");
		main_loop(fd);
	}
}



