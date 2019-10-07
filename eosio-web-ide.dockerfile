# docker build -t eosio/eosio-web-ide:v0.1.0 -f eosio-web-ide.dockerfile .

from ubuntu:18.04

### base
run yes | unminimize \
 && apt-get update \
 && apt-get install -yq \
     asciidoctor \
     bash-completion \
     build-essential \
     clang-tools-8 \
     curl \
     g++-8 \
     git \
     htop \
     jq \
     less \
     libcurl4-gnutls-dev \
     libgmp3-dev \
     libssl-dev \
     libusb-1.0-0-dev \
     llvm-4.0 \
     locales \
     man-db \
     multitail \
     nano \
     nginx \
     ninja-build \
     pkg-config \
     python \
     software-properties-common \
     sudo \
     supervisor \
     vim \
     wget \
     xz-utils \
     zlib1g-dev \
 && update-alternatives --remove-all cc \
 && update-alternatives --install /usr/bin/cc cc /usr/bin/gcc-8 100 \
 && update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-8 100 \
 && update-alternatives --remove-all c++ \
 && update-alternatives --install /usr/bin/c++ c++ /usr/bin/g++-8 100 \
 && update-alternatives --install /usr/bin/gcc++ gcc++ /usr/bin/g++-8 100 \
 && update-alternatives --install /usr/bin/clangd clangd /usr/bin/clangd-8 100 \
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

### boost
run curl -LO https://dl.bintray.com/boostorg/release/1.70.0/source/boost_1_70_0.tar.bz2 \
 && tar -xjf boost_1_70_0.tar.bz2 \
 && cd boost_1_70_0 \
 && ./bootstrap.sh --prefix=/usr/local \
 && ./b2 --with-iostreams --with-date_time --with-filesystem --with-system --with-program_options --with-chrono --with-test -j$(nproc) install \
 && cd /root \
 && rm -rf boost_1_70_0.tar.bz2 boost_1_70_0

### Gitpod user
# '-l': see https://docs.docker.com/develop/develop-images/dockerfile_best-practices/#user
run useradd -l -u 33333 -G sudo -md /home/gitpod -s /bin/bash -p gitpod gitpod \
    # passwordless sudo for users in the 'sudo' group
    && sed -i.bkp -e 's/%sudo\s\+ALL=(ALL\(:ALL\)\?)\s\+ALL/%sudo ALL=NOPASSWD:ALL/g' /etc/sudoers
env HOME=/home/gitpod

### eosio
workdir /home/gitpod/
user gitpod
run git clone https://github.com/EOSIO/eos.git \
 && cd /home/gitpod/eos \
 && git checkout v1.8.2 \
 && git submodule update --init --recursive \
 && mkdir build \
 && cd /home/gitpod/eos/build \
 && cmake -GNinja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr .. \
 && ninja \
 && sudo ninja install \
 && sudo ln -s /usr/lib/x86_64-linux-gnu/cmake/eosio/ /usr/lib/cmake/eosio \
 && cd /home/gitpod \
 && mv /home/gitpod/eos/build/unittests/contracts /home/gitpod/contracts \
 && rm -rf /home/gitpod/eos \
 && mkdir -p /home/gitpod/eos/build/unittests/ \
 && mv /home/gitpod/contracts /home/gitpod/eos/build/unittests/

### CDT
user root
workdir /root
run apt-get update \
 && wget https://github.com/EOSIO/eosio.cdt/releases/download/v1.6.3/eosio.cdt_1.6.3-1-ubuntu-18.04_amd64.deb \
 && apt-get install -y ./eosio.cdt_1.6.3-1-ubuntu-18.04_amd64.deb \
 && rm -rf *.deb /var/lib/apt/lists/* \
 && apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/*

### contracts
workdir /home/gitpod/
user gitpod
run git clone https://github.com/EOSIO/eosio.contracts.git \
 && cd /home/gitpod/eosio.contracts \
 && git checkout v1.7.0 \
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
