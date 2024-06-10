FROM debian:stretch

RUN sed -i s/deb.debian.org/archive.debian.org/g /etc/apt/sources.list
RUN sed -i 's|security.debian.org|archive.debian.org/|g' /etc/apt/sources.list
RUN sed -i '/stretch-updates/d' /etc/apt/sources.list
RUN apt-get update
RUN apt-get install build-essential libcurl3 make curl cmake wget autogen autoconf automake python bc -y

RUN cd && wget https://github.com/steward-fu/website/releases/download/miyoo-mini/gcc-arm-8.2-2018.08-x86_64-arm-linux-gnueabihf.tar.gz
RUN cd && tar xvf gcc-arm-8.2-2018.08-x86_64-arm-linux-gnueabihf.tar.gz && mv gcc-arm-8.2-2018.08-x86_64-arm-linux-gnueabihf /opt/prebuilt
RUN export PATH=/opt/prebuilt/bin/:$PATH
