FROM ubuntu:20.04

WORKDIR ./bloke

COPY / .

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Europe/Moscow
RUN apt-get update && apt-get install -y \
  libsdl2-dev \
  libsdl2-image-dev \
  libsdl2-ttf-dev \
  libsdl2-mixer-dev \
  libenet-dev \
  libcurl4-openssl-dev \
  cmake \
  make \
  git  \	
  build-essential

RUN if [ -d "build" ]; then rm -Rf build; fi
RUN mkdir build
RUN cd ../ && git submodule update --init
WORKDIR build
RUN cmake ..
RUN make bomberbloke_server

EXPOSE 8888
CMD ["./bomberbloke_server"]
