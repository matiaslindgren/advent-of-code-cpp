#!/usr/bin/env bash
set -ue

workdir=/aoc
image=aoc:1

docker build --tag $image .
docker run \
  --workdir $workdir \
  -v $(pwd)/src:${workdir}/src \
  -v $(pwd)/txt:${workdir}/txt \
  -v $(pwd)/Makefile:${workdir}/Makefile \
  -v $(pwd)/test_one.bash:${workdir}/test_one.bash \
  --pull never \
  --rm \
  $image \
  bash -c 'make -j && make test'
