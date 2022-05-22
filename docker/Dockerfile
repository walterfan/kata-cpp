FROM ubuntu:20.04
LABEL Author="Walter Fan <walter.fan@gmail.com>"
LABEL Description="Docker image for C++ program"

RUN apt-get update

# General dependencies
RUN apt-get install -y \
  git \
  mercurial \
  wget \
  vim \
  autoconf \
  bzr \
  cvs \
  unrar \
  build-essential \
  clang \
  valgrind \
  gsl-bin \
  libgsl23 \
  libgsl-dev \
  flex \
  bison \
  libfl-dev \
  tcpdump \
  sqlite3 \
  libsqlite3-dev \
  libxml2 \
  libxml2-dev \
  vtun \
  lxc \
  cmake \
  libc6-dev

# Create working directory
RUN mkdir -p /opt
WORKDIR /opt

# Fetch libevent

RUN wget https://github.com/libevent/libevent/releases/download/release-2.1.12-stable/libevent-2.1.12-stable.tar.gz
RUN tar xvfz libevent-2.1.12-stable.tar.gz
RUN cd libevent-2.1.12-stable && ./configure && make && make install

# Cleanup
RUN apt-get clean && \
  rm -rf /var/lib/apt && \
  rm /opt/libevent-2.1.12-stable.tar.gz

ENTRYPOINT ["bash"]

