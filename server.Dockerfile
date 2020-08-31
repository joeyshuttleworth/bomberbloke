FROM ubuntu:20.04

WORKDIR ./bloke

COPY / ./bloke

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Europe/Moscow
RUN apt-get update && apt-get install -y \
  libsdl2-dev \
  libsdl2-image-dev \
  libenet-dev \
  libsdl2-mixer-dev \
  libcurl4-openssl-dev \
  cmake \
  make \
  build-essential

RUN if [ -d "build" ]; then rm -Rf build; fi
RUN mkdir build
WORKDIR ./bloke/build
RUN cmake ..
RUN make

EXPOSE 8888
CMD ["./bomberbloke_server"]