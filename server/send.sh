gst-launch-1.0 \
    autovideosrc \
    ! video/x-raw, width=640, height=480, framerate=30/1 \
    ! queue \
    ! x264enc  bitrate=2000 tune=zerolatency speed-preset=ultrafast \
    ! queue \
    ! rtph264pay \
    ! udpsink host=192.168.0.117 port=5000 sync=false


#    ! autovideosink sync=false
    
    
#    ! x264enc \
#    ! queue \

