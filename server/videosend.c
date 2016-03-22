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

#define INPUT_DEVICE "/dev/input/event3"
#define EVENT_BUFFER_SIZE 128
#define GST_TRANSITION_TIMEOUT_NS 1000000000
#define SHUTTER_BUTTON_CODE 30



typedef struct {
      struct timeval time;
      unsigned short type;
      unsigned short code;
      unsigned int value;
} input_event_t;

int open_shutter_button() {
	int fd;
	fd = open(INPUT_DEVICE, O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd == -1) {
		perror("open_port: Unable to open input device - ");
		return (-1);
	}

	int flags = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	return fd;
}

int detect_press(char *event) {
	input_event_t *p;
	p = (input_event_t*)event;
	if ((p->type == 1) && (p->code == SHUTTER_BUTTON_CODE)) {
		if (p->value == 1) {
			return 1;
		}
	}
//	printf("%d %d %d\n", p->type, p->value, p->code);
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
	printf("\n");
	return 0;
}


int cp(const char *to, const char *from)
{
    int fd_to, fd_from;
    char buf[4096];
    ssize_t nread;

    fd_from = open(from, O_RDONLY);
    fd_to = open(to, O_WRONLY | O_CREAT | O_EXCL, 0666);

    while (nread = read(fd_from, buf, sizeof buf), nread > 0)
    {
        char *out_ptr = buf;
        ssize_t nwritten;

        do {
            nwritten = write(fd_to, out_ptr, nread);

            if (nwritten >= 0)
            {
                nread -= nwritten;
                out_ptr += nwritten;
            }
            else if (errno != EINTR)
            {
            	return -1;
            }
        } while (nread > 0);
    }

    if (fd_to >= 0)
    	close(fd_to);
    if (fd_from >= 0)
    	close(fd_from);
	return 0;
}


void main_loop(int fd_shutter) {
	int counter = 0;
	char image_destination[MAX_STRING];
	char buffer[EVENT_BUFFER_SIZE];

	while (1) {
		int i;
		fd_set fds;
		FD_ZERO(&fds);
		FD_SET(fd_shutter, &fds);


		if (select(fd_shutter + 1, &fds, 0, 0, 0) == 1) {
			int received = read(fd_shutter, buffer, EVENT_BUFFER_SIZE);
			if (parse_events(buffer, received)) {
				printf("Shutter pressed\n");

				printf("Pause video\n");
				gst_element_set_state(video_pipeline, GST_STATE_PAUSED);
				gst_element_set_state(video_pipeline, GST_STATE_NULL);
				printf("Capture image\n");
				gst_element_set_state(image_pipeline, GST_STATE_PLAYING);
				gst_element_get_state(image_pipeline, NULL, NULL, GST_TRANSITION_TIMEOUT_NS);
				gst_element_set_state(image_pipeline, GST_STATE_PAUSED);
				gst_element_get_state (image_pipeline, NULL, NULL, GST_TRANSITION_TIMEOUT_NS);


				sprintf(image_destination, "img%04d.jpeg", counter++);
				cp(image_destination, "output.jpeg");


				gst_element_set_state(image_pipeline, GST_STATE_NULL);
				printf("Resume video\n");
				gst_element_set_state(video_pipeline, GST_STATE_PLAYING);
			}
		}
	}
}


int main(int argc, char *argv[]) {
	pthread_t gst_thread_ptr;
	int rc;
	long t;
	char client_ip[15];

	if (argc >= 2) {
		strcpy(client_ip, argv[1]);
	} else {
		strcpy(client_ip, "192.168.5.20");
	}

#ifdef LAPTOP
	sprintf(video_uri,
			"autovideosrc ! video/x-raw, width=640, height=480, framerate=30/1"
			" ! queue ! x264enc  bitrate=2000 tune=zerolatency speed-preset=ultrafast"
		    " ! queue ! rtph264pay ! udpsink host=127.0.0.1 port=5000 sync=false");

	sprintf(image_uri,
			"v4l2src num-buffers=1 ! image/jpeg,width=1280,height=720  ! filesink location=output.jpeg");
#else
	sprintf(video_uri,
			"imxv4l2src device=/dev/video0 ! video/x-raw, format=NV12,width=640,height=480"
            " ! vpuenc_h264 bitrate=900 ! rtph264pay ! udpsink host=%s port=5000 sync=false", client_ip);

	sprintf(image_uri,
			"imxv4l2src device=/dev/video0 num-buffers=1 ! video/x-raw, format=I420, width=1920, height=1080"
            " ! jpegenc ! filesink location=output.jpeg");
#endif

	rc = pthread_create(&gst_thread_ptr, NULL, gst_thread, (void *) t);
	if (rc) {
		printf("ERROR; return code from pthread_create() is %d\n", rc);
		exit(-1);
	}

	int fd = open_shutter_button();
	main_loop(fd);

	pthread_exit(NULL);
}


