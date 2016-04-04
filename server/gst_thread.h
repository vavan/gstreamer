#define MAX_STRING 1024
#define GST_TRANSITION_TIMEOUT_NS 1000000000

extern char video_uri[MAX_STRING];
extern char image_uri[MAX_STRING];


GstElement *video_pipeline;
GstElement *image_pipeline;
void *gst_thread(void *threadid);

void take_shot();


