gst-launch-1.0 \
    autovideosrc \
    ! video/x-raw, width=1280, height=720, framerate=30/1 \
    ! tee name=t \
    ! queue \
    ! videoscale ! video/x-raw, width=640, height=400, framerate=30/1 \
    ! x264enc bitrate=2000 tune=zerolatency speed-preset=ultrafast \
    ! rtph264pay \
    ! udpsink host=192.168.0.117 port=5000 sync=false t. \
    ! queue \
    ! videorate out=1 \
    ! jpegenc \
    ! filesink location=test002.jpeg
    
#    autovideosrc \
#    ! video/x-raw, width=1920, height=1080, framerate=30/1 \
#    ! tee name=t \
#    ! queue \
#    ! videoscale ! video/x-raw, width=640, height=480, framerate=30/1 \
#    ! x264enc bitrate=2000 tune=zerolatency speed-preset=ultrafast \
#    ! rtph264pay \
#    ! udpsink host=192.168.0.117 port=5000 sync=false t. \
#    ! queue \
#    ! videorate out=1 \
#    ! jpegenc \
#    ! filesink location=test002.jpeg
    


#    ! autovideosink sync=false
    
    
#    ! x264enc \
#    ! queue \

