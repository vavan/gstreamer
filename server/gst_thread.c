#include <pthread.h>

#include <gst/gst.h>
#include <string.h>
#include <stdio.h>
#include "gst_thread.h"

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

	/* Initialize our data structure */
	memset(&data, 0, sizeof(data));

	/* Build the pipeline */

	printf("Video: %s\n", video_uri);
	printf("Image: %s\n", image_uri);

	video_pipeline = gst_parse_launch(video_uri, NULL);

	image_pipeline = gst_parse_launch(image_uri, NULL);

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

