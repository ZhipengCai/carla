#!/usr/bin/env bash

set -eu

script_name=miniconda.sh
wget https://repo.continuum.io/miniconda/Miniconda3-latest-Linux-x86_64.sh -O ${script_name}
chmod +x ${script_name}
bash ${script_name} -b -p $HOME/miniconda3
rm ${script_name}
