FROM ubuntu:22.04
ARG CLANG_VERSION=17
RUN apt update --yes \
    && apt install --yes wget software-properties-common \
    && wget -qO- https://apt.llvm.org/llvm-snapshot.gpg.key > /etc/apt/trusted.gpg.d/apt.llvm.org.asc \
    && add-apt-repository --yes "deb http://apt.llvm.org/jammy/ llvm-toolchain-jammy-${CLANG_VERSION} main" \
    && add-apt-repository --yes "deb-src http://apt.llvm.org/jammy/ llvm-toolchain-jammy-${CLANG_VERSION} main" \
    && apt update --yes \
    && apt install --yes \
          time \
          make \
          clang-${CLANG_VERSION} \
          libc++-${CLANG_VERSION}-dev \
          libc++abi-${CLANG_VERSION}-dev \
          lld-${CLANG_VERSION}
