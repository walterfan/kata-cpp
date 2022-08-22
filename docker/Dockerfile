FROM ubuntu:20.04
LABEL Author="Walter Fan <walter.fan@gmail.com>"
LABEL Description="Docker image for C++ program"
ENV TZ=Asia/Shanghai \
    DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
  apt-get install -y \
  tzdata \
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
  libc6-dev \
  libgd-dev \
  libgtest-dev \
  libboost-all-dev

# Create working directory
RUN mkdir -p /opt
WORKDIR /opt

# Cleanup
RUN apt-get clean && \
  rm -rf /var/lib/apt

CMD [ "/bin/bash" ]

