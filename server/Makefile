
all:
	gcc -g -c videosend.c `pkg-config --cflags gstreamer-1.0` 
	gcc -g -c gst_thread.c `pkg-config --cflags gstreamer-1.0` 
	gcc -g videosend.o gst_thread.o -o videosend `pkg-config --libs gstreamer-1.0` -pthread

clean:
	rm -rf *.o videosend
