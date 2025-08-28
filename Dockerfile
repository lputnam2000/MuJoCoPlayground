# Build and run:
#   docker build -t clion/ubuntu/cpp-env:1.0 -f Dockerfile.cpp-env-ubuntu .

FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

# Install minimal prerequisites first from default mirrors
RUN apt-get update && apt-get install -y --no-install-recommends ca-certificates tzdata && rm -rf /var/lib/apt/lists/*

# Ensure default Ubuntu ports mirrors are configured (arm64)
RUN rm -f /etc/apt/sources.list.d/* \
  && codename=$(awk -F= '/^VERSION_CODENAME=/{print $2}' /etc/os-release) \
  && printf "deb http://us.ports.ubuntu.com/ubuntu-ports %s main restricted universe multiverse\n" "$codename" > /etc/apt/sources.list \
  && printf "deb http://us.ports.ubuntu.com/ubuntu-ports %s-updates main restricted universe multiverse\n" "$codename" >> /etc/apt/sources.list \
  && printf "deb http://us.ports.ubuntu.com/ubuntu-ports %s-security main restricted universe multiverse\n" "$codename" >> /etc/apt/sources.list \
  && printf "deb http://us.ports.ubuntu.com/ubuntu-ports %s-backports main restricted universe multiverse\n" "$codename" >> /etc/apt/sources.list \
  && printf "Acquire::Retries \"5\"; Acquire::http::Pipeline-Depth \"0\"; Acquire::http::No-Cache \"true\"; Acquire::https::No-Cache \"true\"; Acquire::By-Hash \"yes\";" > /etc/apt/apt.conf.d/99network

RUN apt-get update \
  && apt-get install -y build-essential \
  gcc \
  g++ \
  gdb \
  clang \
  make \
  ninja-build \
  cmake \
  autoconf \
  automake \
  libtool \
  valgrind \
  locales-all \
  dos2unix \
  rsync \
  tar \
  python3 \
  python3-dev \
  python3-pip \
  python-is-python3 \
  && apt-get clean \
  && rm -rf /var/lib/apt/lists/*