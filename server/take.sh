#gst-launch-1.0 -vvvvv v4l2src num-buffers=1 ! video/x-raw,format=NV12,width=800,height=600 ! jpegenc ! filesink location=test.jpg

gst-launch-1.0  v4l2src num-buffers=1 ! image/jpeg,width=1280,height=720  ! filesink location=test.jpg

