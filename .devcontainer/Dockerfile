FROM mcr.microsoft.com/devcontainers/cpp:1-debian-12

RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends \
    libmysqlcppconn-dev \
    default-mysql-client \
    build-essential \
    cmake \
    libgtest-dev \
    libgmock-dev \
    lcov \
    doxygen \
    graphviz \
    clang-format \
    cppcheck \
    bc \
    && cmake -S /usr/src/googletest -B /tmp/gtest-build \
    && cmake --build /tmp/gtest-build \
    && cmake --install /tmp/gtest-build \
    && rm -rf /tmp/gtest-build \
    && apt-get clean && rm -rf /var/lib/apt/lists/*
