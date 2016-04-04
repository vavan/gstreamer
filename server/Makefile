
CFLAGS:=`pkg-config --cflags gstreamer-1.0`
LDFLAGS:=`pkg-config --libs gstreamer-1.0` -pthread

all:
	gcc -c videosend.c $(CFLAGS)
	gcc -c gst_thread.c $(CFLAGS) 
	gcc videosend.o gst_thread.o -o videosend $(LDFLAGS)
	gcc wait_shutter.c $(CFLAGS) -o wait_shutter $(LDFLAGS)

clean:
	rm -rf *.o videosend wait_shutter
