#!/usr/bin/env bash
set -ueo pipefail

tmpdir="$(mktemp --directory)"
function rm_tmpdir {
  rm -rf "$tmpdir"
}
trap rm_tmpdir EXIT

function test_with_random_data {
  local bs="$1"
  local n="$2"
  data="${tmpdir}/rand.data"
  dd if=/dev/urandom of="$data" bs=${bs} count=${n} 2> /dev/null
  wc -c "$data"
  aoc_md5=$(make LLVM_VERSION=${LLVM_VERSION:-18} FAST=${FAST:-0} run_tools/md5 < "$data")
  gnu_md5=$(md5sum < "$data" | cut -f1 -d' ')
  if [ "$aoc_md5" != "$gnu_md5" ]; then
    printf "aoc_md5 '$aoc_md5' != '$gnu_md5' gnu_md5\n"
    cp "$data" failing_md5_test_data
    return 1
  fi
  rm "$data"
  return 0
}

for n in $(seq 1 1 129); do
  if ! test_with_random_data 1 $n; then
    exit 1;
  fi
done

for n in $(seq 1000 1000 20000); do
  if ! test_with_random_data 1024 $n; then
    exit 1;
  fi
done
