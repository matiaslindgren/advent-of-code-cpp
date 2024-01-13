#!/usr/bin/env bash
set -ueo pipefail

tmpdir="$(mktemp --directory)"
function rm_tmpdir {
  rm -rf "$tmpdir"
}
trap rm_tmpdir EXIT

for n in $(seq 1000 1000 20000); do
  data="${tmpdir}/rand.data"
  dd if=/dev/urandom of="$data" bs=1024 count=${n} 2> /dev/null
  wc -c "$data"
  aoc_md5=$(make FAST=${FAST} run_tools/md5 < "$data")
  gnu_md5=$(md5sum < "$data" | cut -f1 -d' ')
  [ "$aoc_md5" == "$gnu_md5" ] || exit 1
done
