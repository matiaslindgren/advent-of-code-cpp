FROM ubuntu:22.04

ARG llvm=19

RUN apt update --yes \
    && apt install --yes jq wget \
    && wget -qO- https://apt.llvm.org/llvm-snapshot.gpg.key > /etc/apt/trusted.gpg.d/apt.llvm.org.asc \
    && echo "deb http://apt.llvm.org/jammy/ llvm-toolchain-jammy-${llvm} main" >> /etc/apt/sources.list \
    && echo "deb-src http://apt.llvm.org/jammy/ llvm-toolchain-jammy-${llvm} main >> /etc/apt/sources.list"

RUN apt update --yes \
    && apt install --yes \
          time \
          make \
          clang-${llvm} \
          clang-tidy-${llvm} \
          libc++-${llvm}-dev \
          libc++abi-${llvm}-dev \
          lld-${llvm}
