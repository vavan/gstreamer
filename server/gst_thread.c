#include <pthread.h>


#include <gst/gst.h>
#include <string.h>
#include <stdio.h>
#include "gst_thread.h"


GST_DEBUG_CATEGORY_STATIC (tvb);
#define GST_CAT_DEFAULT tvb


char video_uri[MAX_STRING];
char image_uri[MAX_STRING];

typedef struct _CustomData {
	gboolean is_live;
	GstElement *pipeline;
	GMainLoop *loop;
} CustomData;



static void cb_message(GstBus *bus, GstMessage *msg, CustomData *data) {

	switch (GST_MESSAGE_TYPE(msg)) {
	case GST_MESSAGE_ERROR: {
		GError *err;
		gchar *debug;
		printf("GST_MESSAGE_ERROR\n");

		gst_message_parse_error(msg, &err, &debug);
		g_print("Error: %s\n", err->message);
		g_error_free(err);
		g_free(debug);

		gst_element_set_state(data->pipeline, GST_STATE_READY);
		g_main_loop_quit(data->loop);
		break;
	}
	case GST_MESSAGE_EOS:
		printf("GST_MESSAGE_EOS\n");
		/* end-of-stream */
		gst_element_set_state(data->pipeline, GST_STATE_READY);
		g_main_loop_quit(data->loop);
		break;
	case GST_MESSAGE_BUFFERING: {
		printf("GST_MESSAGE_BUFFERING\n");
		gint percent = 0;

		/* If the stream is live, we do not care about buffering. */
		if (data->is_live)
			break;

		gst_message_parse_buffering(msg, &percent);
		g_print("Buffering (%3d%%)\r", percent);
		/* Wait until buffering is complete before start/resume playing */
		if (percent < 100)
			gst_element_set_state(data->pipeline, GST_STATE_PAUSED);
		else
			gst_element_set_state(data->pipeline, GST_STATE_PLAYING);
		break;
	}
	case GST_MESSAGE_CLOCK_LOST:
		printf("GST_MESSAGE_CLOCK_LOST\n");

		/* Get a new clock */
		gst_element_set_state(data->pipeline, GST_STATE_PAUSED);
		gst_element_set_state(data->pipeline, GST_STATE_PLAYING);
		break;
	default:
		/* Unhandled message */
		break;
	}
}

GstElement *icf, *q1, *q2;


GstElement *build_pipe() {
/*
"autovideosrc ! video/x-raw, width=640, height=480, framerate=30/1"
			" ! queue ! x264enc  bitrate=2000 tune=zerolatency speed-preset=ultrafast"
		    " ! queue ! rtph264pay ! udpsink host=%s port=5000 sync=false", client_ip);
*/
    GstCaps *vcaps, *icaps;
    GstElement *pipeline, *src, *vcf, *scale, *enc, *pay, *usink, *tee, *jpeg, *fsink, *rate;
 
    icaps = gst_caps_from_string( "video/x-raw, width=1280, height=720, framerate=30/1" );
    vcaps = gst_caps_from_string( "video/x-raw, width=640, height=400, framerate=30/1" );
    
    
    
	pipeline = gst_pipeline_new("video-pipeline");
	src = gst_element_factory_make ("autovideosrc", "src");
	icf = gst_element_factory_make ("capsfilter", "icf");
	q1 = gst_element_factory_make ("queue", "vqueue");
	q2 = gst_element_factory_make ("queue", "iqueue");
	scale = gst_element_factory_make ("videoscale", "scale");
	vcf = gst_element_factory_make ("capsfilter", "vcf");
	enc = gst_element_factory_make ("x264enc", "enc");
	pay = gst_element_factory_make ("rtph264pay", "pay");
	usink = gst_element_factory_make ("udpsink", "sink");
	
	//tee = gst_element_factory_make ("tee", "tee");
	//rate = gst_element_factory_make ("videorate", "rate");
	jpeg = gst_element_factory_make ("jpegenc", "jpeg");
	fsink = gst_element_factory_make ("filesink", "fsink");
	
	
	if (!pipeline || !src || !icf || !q1 || !scale || !vcf || !enc || !pay || !usink
	    || !q2 || !jpeg || !fsink) {
       g_printerr ("One element could not be created.\n");
       return NULL;
    }
    g_object_set (G_OBJECT (icf), "caps", icaps, NULL);
    g_object_set (G_OBJECT (vcf), "caps", vcaps, NULL);
    g_object_set (G_OBJECT (enc), "bitrate", 2000, NULL);
    g_object_set (G_OBJECT (enc), "tune", 4, NULL);
    g_object_set (G_OBJECT (enc), "speed-preset", 1, NULL);
    g_object_set (G_OBJECT (usink), "host", "192.168.0.117", NULL);
    g_object_set (G_OBJECT (usink), "port", 5000, NULL);
    g_object_set (G_OBJECT (usink), "sync", 0, NULL);
    
    //g_object_set (G_OBJECT (rate), "out", 1, NULL);
    g_object_set (G_OBJECT (fsink), "location", "dodo.jpeg", NULL);


    gst_bin_add_many (GST_BIN (pipeline), src, icf, q1, scale, vcf, enc, pay, usink, q2, jpeg, fsink, NULL);
    gst_element_link_many (src, icf, q1, NULL);
    gst_element_link_many (q1, scale, vcf, enc, pay, usink, NULL);
    gst_element_link_many (q2, jpeg, fsink, NULL);

	return pipeline;
}

void take_shot() {
	GST_ERROR("Shutter");
	gst_element_set_state(video_pipeline, GST_STATE_PAUSED);
	gst_element_get_state(video_pipeline, NULL, NULL, GST_TRANSITION_TIMEOUT_NS);
	gst_element_set_state(video_pipeline, GST_STATE_NULL);
	gst_element_get_state(video_pipeline, NULL, NULL, GST_TRANSITION_TIMEOUT_NS);
//    gst_element_unlink (icf, q1);
  //  gst_element_link (icf, q2);
	//gst_element_set_state(video_pipeline, GST_STATE_PLAYING);
//	gst_element_get_state(video_pipeline, NULL, NULL, GST_TRANSITION_TIMEOUT_NS);

//	gst_element_set_state(video_pipeline, GST_STATE_PAUSED);
	//GST_ERROR("Saved");
//	gst_element_get_state(video_pipeline, NULL, NULL, GST_TRANSITION_TIMEOUT_NS);
//    gst_element_unlink (icf, q2);
//    gst_element_link (icf, q1);
	gst_element_set_state(video_pipeline, GST_STATE_PLAYING);
	//gst_element_get_state(video_pipeline, NULL, NULL, GST_TRANSITION_TIMEOUT_NS);
	GST_ERROR("Resume");
}

void *gst_thread(void *threadid) {
	GstBus *bus;
	GstStateChangeReturn ret;
	GMainLoop * main_loop;
	CustomData data;
	long tid;

	int argc = 0;
	char **argv;

	/* Initialize GStreamer */
	gst_init(&argc, &argv);
	
	GST_DEBUG_CATEGORY_INIT (tvb, "my category", 0, "This is my very own");

	/* Initialize our data structure */
	memset(&data, 0, sizeof(data));
	
	GST_ERROR("The fist111");
	

	/* Build the pipeline */

	printf("Video: %s\n", video_uri);
	printf("Image: %s\n", image_uri);

	video_pipeline = gst_parse_launch(video_uri, NULL);

	image_pipeline = gst_parse_launch(image_uri, NULL);
    //video_pipeline = build_pipe();

	bus = gst_element_get_bus(video_pipeline);

	/* Start playing */
	ret = gst_element_set_state(video_pipeline, GST_STATE_PLAYING);
	if (ret == GST_STATE_CHANGE_FAILURE) {
		g_printerr("Unable to set the pipeline to the playing state.\n");
		gst_object_unref(video_pipeline);
		pthread_exit(NULL);
	} else if (ret == GST_STATE_CHANGE_NO_PREROLL) {
		data.is_live = TRUE;
	}

	main_loop = g_main_loop_new(NULL, FALSE);
	data.loop = main_loop;
	data.pipeline = video_pipeline;

	gst_bus_add_signal_watch(bus);
	g_signal_connect(bus, "message", G_CALLBACK(cb_message), &data);

	g_main_loop_run(main_loop);
	printf("Done with the loop\n");

	/* Free resources */
	g_main_loop_unref(main_loop);
	gst_object_unref(bus);
	gst_element_set_state(video_pipeline, GST_STATE_NULL);
	gst_object_unref(video_pipeline);
	gst_element_set_state(image_pipeline, GST_STATE_NULL);
	gst_object_unref(image_pipeline);

	pthread_exit(NULL);
}

