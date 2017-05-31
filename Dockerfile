FROM alpine:3.5

RUN mkdir -p /opt/tulip/lib
COPY tulip/* /opt/tulip/
COPY tulip/lib/* /opt/tulip/lib/
RUN apt-get update
RUN apt-get install build-base libgpg-error-dev
RUN make -C /opt/tulip
