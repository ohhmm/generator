FROM --platform=$BUILDPLATFORM ohhmm/openmind AS build

LABEL platform agnostic container

RUN emerge --update \
    kde-frameworks/extra-cmake-modules \
    -j`nproc` && rm -rf /var/cache/distfiles 

RUN echo 'ACCEPT_LICENSE="*"' >> /etc/make.conf
RUN echo "dev-util/intel-ocl-sdk **" > /etc/portage/package.accept_keywords/intel-ocl-sdk
RUN emerge -j`nproc` dev-util/intel-ocl-sdk

RUN emerge --update \
    media-libs/libjpeg-turbo \
    media-libs/libpng \
    -j`nproc` && rm -rf /var/cache/distfiles 