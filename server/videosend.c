#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <gst/gst.h>
#include <string.h>
#include "gst_thread.h"


int main(int argc, char *argv[])
{
        pthread_t gst_thread_ptr;
        int rc;
        long t;
	char client_ip[15];

	if (argc >= 2) {
		strcpy(client_ip, argv[1]);
	} else {
		strcpy(client_ip, "192.168.5.20");
	}

	sprintf(video_uri, "imxv4l2src device=/dev/video0 \
   ! video/x-raw, format=NV12,width=640,height=480\
   ! vpuenc_h264 bitrate=900 \
   ! rtph264pay \
   ! udpsink host=%s port=5000 sync=false", client_ip);

        sprintf(image_uri, "imxv4l2src device=/dev/video0 num-buffers=1 \
   ! video/x-raw, format=I420, width=1920, height=1080 \
   ! jpegenc \
   ! filesink location=test002.jpeg");


        rc = pthread_create(&gst_thread_ptr, NULL, gst_thread, (void *)t);
        if (rc) {
                printf("ERROR; return code from pthread_create() is %d\n", rc);
                exit(-1);
        }

        while(1) {
                char ch = getchar();
                if (ch == EOF) {
                        break;
                } else if (ch == 't') {
                        printf("PAUSE\n");
                        gst_element_set_state (video_pipeline, GST_STATE_PAUSED);
                        gst_element_set_state (video_pipeline, GST_STATE_NULL);
                        printf("CAPTURE\n");
                        gst_element_set_state (image_pipeline, GST_STATE_PLAYING);

                        gst_element_get_state (image_pipeline, NULL, NULL, 1000000000);
                        gst_element_set_state (image_pipeline, GST_STATE_PAUSED);
                        //gst_element_get_state (image_pipeline, NULL, NULL, 1000000000);

                        gst_element_set_state (image_pipeline, GST_STATE_NULL);
                        printf("RESUME\n");
                        gst_element_set_state (video_pipeline, GST_STATE_PLAYING);
                }
        }

        pthread_exit(NULL);
}


