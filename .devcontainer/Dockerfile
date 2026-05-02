FROM mcr.microsoft.com/devcontainers/cpp:1-debian-11

RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends \
    libmysqlcppconn-dev \
    default-mysql-client \
    build-essential \
    cmake \
    libgtest-dev \
    lcov \
    doxygen \
    graphviz \
    clang-format \
    && cd /usr/src/gtest \
    && cmake . \
    && make \
    && cp lib/*.a /usr/lib/ \
    && apt-get clean && rm -rf /var/lib/apt/lists/*
