FROM ubuntu:16.04
ENV DEBIAN_FRONTEND noninteractive

RUN mkdir -p /opt/tulip/lib
COPY * /opt/tulip/
COPY lib/* /opt/tulip/lib/
RUN apt-get update
RUN apt-get install build-base libgpg-error-dev
RUN make -C /opt/tulip
