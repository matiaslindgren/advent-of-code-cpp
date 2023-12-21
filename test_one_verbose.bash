#!/usr/bin/env bash
set -ue

if [ $# -ne 3 ]; then
  printf "usage: $0 solution input correct\n"
  exit 2
fi

solution="$1"
input="$2"
correct="$3"

test_dir="$(mktemp --directory)"
function rm_test_dir {
  rm -rf "$test_dir"
}
trap rm_test_dir EXIT

log="${test_dir}/${solution//\//_}.log"

time_verbose_flag=-v
if [ "$(uname)" = 'Darwin' ]; then
  time_verbose_flag=-l
fi

printf '%s < %s\n' "$solution" "$input"
result=$(/usr/bin/env time $time_verbose_flag -o "$log" "$solution" < "$input")
expect=$(cat $correct)
printf 'result: %s\n' "$result"
printf 'expect: %s\n' "$expect"
cat "$log"
printf '\n'

if [ "$result" != "$expect" ]; then
  exit 1
fi
