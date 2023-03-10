FROM --platform=$BUILDPLATFORM ohhmm/openmind AS build

LABEL platform agnostic container


RUN echo 'ACCEPT_LICENSE="*"' >> /etc/make.conf
RUN echo "dev-util/intel-ocl-sdk **" > /etc/portage/package.accept_keywords/intel-ocl-sdk
RUN emerge --update \
    dev-util/intel-ocl-sdk \
    -j`nproc` && rm -rf /var/cache/distfiles 

RUN emerge --update \
    media-libs/libjpeg-turbo \
    media-libs/libpng \
    -j`nproc` && rm -rf /var/cache/distfiles 
