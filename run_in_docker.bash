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
  -v $(pwd)/test_one_verbose.bash:${workdir}/test_one_verbose.bash \
  --pull never \
  --rm \
  -it \
  $image \
  bash
