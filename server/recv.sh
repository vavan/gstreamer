gst-launch-1.0 -vvv \
    udpsrc port=5000 \
    caps="application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)H264, sprop-parameter-sets=(string)\"Z0JAHqaAoD2QAA\\=\\=\\,aM44gAA\\=\", payload=(int)96, ssrc=(uint)4292440636, clock-base=(uint)2608982052, seqnum-base=(uint)4218" \
    ! rtph264depay \
    ! queue \
    ! avdec_h264 \
    ! autovideosink sync=false

