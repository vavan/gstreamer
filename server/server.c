#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <gst/gst.h>

   
void *gst_thread(void *threadid);

extern GstElement *video_pipeline;
extern GstElement *image_pipeline;


int main(int argc, char *argv[])
{
	pthread_t gst_thread_ptr;
	int rc;
	long t;
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

