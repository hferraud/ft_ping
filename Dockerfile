FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    build-essential \
    wget

RUN wget https://ftpmirror.gnu.org/inetutils/inetutils-2.0.tar.gz && \
    tar -xvf inetutils-2.0.tar.gz && \
    cd inetutils-2.0 && \
    ./configure && \
    make && \
    make install