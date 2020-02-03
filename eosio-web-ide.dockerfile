# docker build -t eosio/eosio-web-ide:v0.1.2 -f eosio-web-ide.dockerfile .

from ubuntu:18.04

### base
run yes | unminimize \
 && apt-get update \
 && apt-get install -yq \
     build-essential \
     curl \
     g++-8 \
     git \
     jq \
     less \
     locales \
     nano \
     nginx \
     ninja-build \
     python \
     sudo \
     vim \
     wget \
 && update-alternatives --remove-all cc \
 && update-alternatives --install /usr/bin/cc cc /usr/bin/gcc-8 100 \
 && update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-8 100 \
 && update-alternatives --remove-all c++ \
 && update-alternatives --install /usr/bin/c++ c++ /usr/bin/g++-8 100 \
 && update-alternatives --install /usr/bin/gcc++ gcc++ /usr/bin/g++-8 100 \
 && locale-gen en_US.UTF-8 \
 && curl -sL https://deb.nodesource.com/setup_10.x | bash - \
 && apt-get install -yq \
     nodejs \
 && apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* \
 && npm i -g yarn typescript
env LANG=en_US.UTF-8

### cmake
workdir /root
run curl -LO https://cmake.org/files/v3.13/cmake-3.13.2.tar.gz \
 && tar -xzf cmake-3.13.2.tar.gz \
 && cd cmake-3.13.2 \
 && ./bootstrap --prefix=/usr/local \
 && make -j$(nproc) \
 && make install \
 && cd /root \
 && rm -rf cmake-3.13.2.tar.gz cmake-3.13.2

### Gitpod user
# '-l': see https://docs.docker.com/develop/develop-images/dockerfile_best-practices/#user
run useradd -l -u 33333 -G sudo -md /home/gitpod -s /bin/bash -p gitpod gitpod \
    # passwordless sudo for users in the 'sudo' group
    && sed -i.bkp -e 's/%sudo\s\+ALL=(ALL\(:ALL\)\?)\s\+ALL/%sudo ALL=NOPASSWD:ALL/g' /etc/sudoers
env HOME=/home/gitpod

### eosio
user root
workdir /root
run apt-get update \
 && wget https://github.com/EOSIO/eos/releases/download/v2.0.0/eosio_2.0.0-1-ubuntu-18.04_amd64.deb \
 && apt install -y ./eosio_2.0.0-1-ubuntu-18.04_amd64.deb

### CDT
user root
workdir /root
run apt-get update \
 && wget https://github.com/EOSIO/eosio.cdt/releases/download/v1.7.0/eosio.cdt_1.7.0-1-ubuntu-18.04_amd64.deb \
 && apt-get install -y ./eosio.cdt_1.7.0-1-ubuntu-18.04_amd64.deb \
 && rm -rf *.deb /var/lib/apt/lists/* \
 && apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/*

### contracts
# add-boot-contract branch until v1.10.0
workdir /home/gitpod/
user gitpod
run git clone https://github.com/EOSIO/eosio.contracts.git \
 && cd /home/gitpod/eosio.contracts \
 && git checkout add-boot-contract \
 && git submodule update --init --recursive \
 && mkdir build \
 && cd /home/gitpod/eosio.contracts/build \
 && cmake -GNinja .. \
 && ninja \
 && mkdir /home/gitpod/contracts \
 && cp `find . -name 'eosio.*.wasm'` /home/gitpod/contracts \
 && cd /home/gitpod \
 && rm -rf /home/gitpod/eosio.contracts

### nginx, /password
user root
workdir /root
run echo >/password && chown gitpod /password && chgrp gitpod /password \
 && >/run/nginx.pid \
 && chmod 666 /run/nginx.pid \
 && chmod 666 /var/log/nginx/* \
 && chmod 777 /var/lib/nginx /var/log/nginx

### Gitpod user (2)
WORKDIR $HOME
USER gitpod
# custom Bash prompt
run { echo && echo "PS1='\[\e]0;\u \w\a\]\[\033[01;32m\]\u\[\033[00m\] \[\033[01;34m\]\w\[\033[00m\] \\\$ '" ; } >> .bashrc
# use sudo so that user does not get sudo usage info on (the first) login
run sudo echo "Running 'sudo' for Gitpod: success"

### wallet
run cleos wallet create --to-console | tail -n 1 | sed 's/"//g' >/password \
    && cleos wallet import --private-key 5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3
run echo '\n\
   unlock-timeout = 31536000 \n\
' >$HOME/eosio-wallet/config.ini

### checks
# no root-owned files in the home directory
run rm -f $HOME/.wget-hsts
WORKDIR $HOME
USER gitpod
run notOwnedFile=$(find . -not "(" -user gitpod -and -group gitpod ")" -print -quit) \
    && { [ -z "$notOwnedFile" ] \
        || { echo "Error: not all files/dirs in $HOME are owned by 'gitpod' user & group"; exit 1; } }
