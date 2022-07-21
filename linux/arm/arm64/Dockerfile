FROM ubuntu:focal

LABEL maintainer="d.vedenko@audacityteam.org"
LABEL description="A crosscompilation environment for Linux aarch64"

ENV TZ=Europe/London
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# Install native packages first
RUN apt-get update && apt-get install -y build-essential python3-minimal python3-pip g++-9 libstdc++-9-dev gosu qemu-user-static
# install native dependecies
RUN apt-get install -y libgtk2.0-dev libasound2-dev libavformat-dev libjack-jackd2-dev

RUN pip3 install cmake && pip3 install conan

# Install cross-environment packages
RUN apt-get install -y crossbuild-essential-arm64

RUN dpkg --add-architecture arm64

RUN sed -i "s/deb h/deb [arch=amd64] h/g" /etc/apt/sources.list && \
    echo "# arm64 repositories" >> /etc/apt/sources.list && \
    echo "deb [arch=arm64] http://ports.ubuntu.com/ubuntu-ports focal main restricted" >> /etc/apt/sources.list && \
    echo "deb [arch=arm64] http://ports.ubuntu.com/ubuntu-ports focal-updates main restricted" >> /etc/apt/sources.list && \
    echo "deb [arch=arm64] http://ports.ubuntu.com/ubuntu-ports focal universe" >> /etc/apt/sources.list && \
    echo "deb [arch=arm64] http://ports.ubuntu.com/ubuntu-ports focal-updates universe" >> /etc/apt/sources.list && \
    echo "deb [arch=arm64] http://ports.ubuntu.com/ubuntu-ports focal multiverse" >> /etc/apt/sources.list && \
    echo "deb [arch=arm64] http://ports.ubuntu.com/ubuntu-ports focal-updates multiverse" >> /etc/apt/sources.list && \
    echo "deb [arch=arm64] http://ports.ubuntu.com/ubuntu-ports focal-backports main restricted universe multiverse" >> /etc/apt/sources.list && \
    echo "deb [arch=arm64] http://ports.ubuntu.com/ubuntu-ports focal-security main restricted" >> /etc/apt/sources.list && \
    echo "deb [arch=arm64] http://ports.ubuntu.com/ubuntu-ports focal-security universe" >> /etc/apt/sources.list && \
    echo "deb [arch=arm64] http://ports.ubuntu.com/ubuntu-ports focal-security multiverse" >> /etc/apt/sources.list

RUN cat /etc/apt/sources.list

RUN apt-get update && apt-get install -y libgtk2.0-dev:arm64 libasound2-dev:arm64 libjack-jackd2-dev:arm64 libx11-dev:arm64 libxrandr-dev:arm64

RUN apt-get install -y libdmx-dev:arm64 \
 libfontenc-dev:arm64 \
 libfs-dev:arm64 \
 libice-dev:arm64 \
 libsm-dev:arm64 \
 libx11-dev:arm64 \
 libxau-dev:arm64 \
 libxaw7-dev:arm64 \
 libxcomposite-dev:arm64 \
 libxcursor-dev:arm64 \
 libxdamage-dev:arm64 \
 libxdmcp-dev:arm64 \
 libxext-dev:arm64 \
 libxfixes-dev:arm64 \
 libxfont-dev:arm64 \
 libxft-dev:arm64 \
 libxi-dev:arm64 \
 libxinerama-dev:arm64 \
 libxkbfile-dev:arm64 \
 libxmu-dev:arm64 \
 libxmuu-dev:arm64 \
 libxpm-dev:arm64 \
 libxrandr-dev:arm64 \
 libxrender-dev:arm64 \
 libxres-dev:arm64 \
 libxss-dev:arm64 \
 libxt-dev:arm64 \
 libxtst-dev:arm64 \
 libxv-dev:arm64 \
 libxvmc-dev:arm64 \
 libxxf86dga-dev:arm64 \
 libxxf86vm-dev:arm64 \
 x11proto-dev:arm64 \
 xserver-xorg-dev:arm64 \
 xtrans-dev:arm64

RUN apt-get install -y wget

# linuxdeploy
RUN apt-get install -y libboost-filesystem-dev:arm64 libboost-regex-dev:arm64
RUN apt-get install -y cimg-dev
RUN apt-get install -y libjpeg-dev:arm64

ENV CONAN_USER_HOME=/build/conan

COPY ["entrypoint.sh", "/entrypoint.sh"]
RUN chmod +x /entrypoint.sh
ENTRYPOINT [ "/entrypoint.sh" ]
