FROM ubuntu:18.04

USER root

ARG EPIC_USER=user
ARG EPIC_PASS=pass
ENV DEBIAN_FRONTEND=noninteractive
ENV HTTP_PROXY="http://proxy-us.intel.com:911"
ENV HTTPS_PROXY="http://proxy-us.intel.com:911"
ENV NO_PROXY="intel.com,.intel.com,10.0.0.0/8,192.168.0.0/16,localhost,.local,127.0.0.0/8,134.134.0.0/16,git-amr-1.devtools.intel.com"
ENV http_proxy=$HTTP_PROXY
ENV https_proxy=$HTTPS_PROXY
ENV no_proxy=$NO_PROXY

# apt-get proxy
ENV APT_PROXY_PATH="/etc/apt/apt.conf.d/proxy.conf"
RUN rm -rf "${APT_PROXY_PATH}"
RUN touch "${APT_PROXY_PATH}"
RUN echo 'Acquire::http::Proxy "http://proxy-us.intel.com:911";' | tee -a "${APT_PROXY_PATH}" >/dev/null
RUN echo 'Acquire::https::Proxy "http://proxy-us.intel.com:911";' | tee -a "${APT_PROXY_PATH}" >/dev/null
RUN echo "Written to ${APT_PROXY_PATH}:"
RUN cat "${APT_PROXY_PATH}"

RUN apt-get update
RUN apt-get install -y wget software-properties-common

RUN apt-key adv --keyserver keyserver.ubuntu.com --keyserver-options http-proxy=${http_proxy} --recv-keys 1E9377A2BA9EF27F
RUN apt-add-repository "deb http://ppa.launchpad.net/ubuntu-toolchain-r/test/ubuntu bionic main"

#RUN  add-apt-repository ppa:ubuntu-toolchain-r/test

RUN  wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key|apt-key add - && \
  apt-add-repository "deb http://apt.llvm.org/xenial/ llvm-toolchain-xenial-8 main" && \
  apt-get update ; \
  apt-get install -y build-essential \
    clang-8 \
    lld-8 \
    g++-7 \
    cmake \
    ninja-build \
    libvulkan1 \
    python \
    python-pip \
    python-dev \
    python3-dev \
    python3-pip \
    libpng-dev \
    libtiff5-dev \
    libjpeg-dev \
    tzdata \
    sed \
    curl \
    unzip \
    autoconf \
    libtool \
    rsync \
    libxml2-dev \
    git \
    aria2 && \
  pip3 install -Iv setuptools==47.3.1 && \
  pip3 install distro && \
  update-alternatives --install /usr/bin/clang++ clang++ /usr/lib/llvm-8/bin/clang++ 180 && \
  update-alternatives --install /usr/bin/clang clang /usr/lib/llvm-8/bin/clang 180

RUN useradd -m carla
COPY --chown=carla:carla . /home/carla
USER carla
WORKDIR /home/carla
ENV UE4_ROOT /home/carla/UE4.26

RUN git clone --depth 1 -b marcel/gbuffer_view "https://${EPIC_USER}:${EPIC_PASS}@github.com/CarlaUnreal/UnrealEngine.git" ${UE4_ROOT}

#RUN git clone --depth 1 -b carla "https://github.com/CarlaUnreal/UnrealEngine.git" ${UE4_ROOT}

RUN cd $UE4_ROOT && \
  ./Setup.sh && \
  ./GenerateProjectFiles.sh && \
  make

WORKDIR /home/carla/
