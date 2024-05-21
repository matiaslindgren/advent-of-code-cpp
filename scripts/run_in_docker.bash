#!/usr/bin/env bash
set -ue

workdir=/aoc
image=aoc:1

docker build --tag $image .
docker run \
  --workdir $workdir \
  -v $(pwd)/.clang-format:${workdir}/.clang-format \
  -v $(pwd)/.clang-tidy:${workdir}/.clang-tidy \
  -v $(pwd)/src:${workdir}/src \
  -v $(pwd)/include:${workdir}/include \
  -v $(pwd)/ndvec:${workdir}/ndvec \
  -v $(pwd)/txt:${workdir}/txt \
  -v $(pwd)/Makefile:${workdir}/Makefile \
  -v $(pwd)/scripts/test_one_verbose.bash:${workdir}/scripts/test_one_verbose.bash \
  --pull never \
  --rm \
  -it \
  $image \
  bash
