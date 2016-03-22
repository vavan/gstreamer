
all:
	gcc -c videosend.c `pkg-config --cflags gstreamer-1.0` 
	gcc -c gst_thread.c `pkg-config --cflags gstreamer-1.0` 
	gcc videosend.o gst_thread.o -o server.bin `pkg-config --libs gstreamer-1.0` -pthread
