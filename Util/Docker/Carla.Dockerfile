FROM carla-prerequisites:latest

ARG GIT_BRANCH

ENV HTTP_PROXY="http://proxy-us.intel.com:911"
ENV HTTPS_PROXY="http://proxy-us.intel.com:911"
ENV NO_PROXY="intel.com,.intel.com,10.0.0.0/8,192.168.0.0/16,localhost,.local,127.0.0.0/8,134.134.0.0/16,git-amr-1.devtools.intel.com"
ENV http_proxy=$HTTP_PROXY
ENV https_proxy=$HTTPS_PROXY
ENV no_proxy=$NO_PROXY

# apt-get proxy
#ENV APT_PROXY_PATH="/etc/apt/apt.conf.d/proxy.conf"
#RUN rm -rf "${APT_PROXY_PATH}"
#RUN touch "${APT_PROXY_PATH}"
#RUN echo 'Acquire::http::Proxy "http://proxy-us.intel.com:911";' | tee -a "${APT_PROXY_PATH}" >/dev/null
#RUN echo 'Acquire::https::Proxy "http://proxy-us.intel.com:911";' | tee -a "${APT_PROXY_PATH}" >/dev/null
#RUN echo "Written to ${APT_PROXY_PATH}:"
#RUN cat "${APT_PROXY_PATH}"

USER carla
WORKDIR /home/carla

RUN cd /home/carla/

RUN echo $GIT_BRANCH

#RUN if [ -z ${GIT_BRANCH+x} ]; then git clone --depth 1 https://github.com/carla-simulator/carla.git; \

RUN git clone --depth 1 --branch $GIT_BRANCH https://github.com/carla-simulator/carla.git

RUN cd /home/carla/carla \
&& echo pwd \
&& ./Update.sh 

WORKDIR /home/carla/carla

RUN pwd
RUN ls -alh

RUN make CarlaUE4Editor
RUN make PythonAPI
RUN make build.utils
RUN make package
RUN rm -r /home/carla/carla/Dist

WORKDIR /home/carla/carla
