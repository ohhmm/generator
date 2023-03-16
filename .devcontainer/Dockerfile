FROM --platform=$BUILDPLATFORM ohhmm/openmind AS build



RUN emerge --update \
    media-libs/libjpeg-turbo \
    media-libs/libpng \
    -j`nproc` && rm -rf /var/cache/distfiles 
