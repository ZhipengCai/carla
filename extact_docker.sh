#!/bin/bash

docker_tag="carla:latest"

out_dir=$(pwd)/docker_out
rm -rf $out_dir
mkdir -p $out_dir
chmod 777 -R $out_dir

docker run -v "$(pwd):/opt/mount" --rm "${docker_tag}" bash -c \
    "cp -r /home/carla/carla /opt/mount/docker_out && chown $(id -u):$(id -g) -R /opt/mount/docker_out/carla"
