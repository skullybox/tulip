FROM ubuntu:16.04
ENV DEBIAN_FRONTEND noninteractive

RUN mkdir -p /opt/tulip/lib
COPY * /opt/tulip/
COPY lib/* /opt/tulip/lib/
RUN apk update
RUN apk add --no-cache build-base libgpg-error-dev
RUN make -C /opt/tulip
