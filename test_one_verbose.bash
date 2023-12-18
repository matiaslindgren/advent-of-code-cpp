#!/usr/bin/env bash
set -ue

if [ $# -ne 1 ]; then
  printf "usage: $0 target\n"
  exit 2
fi

target="$1"

test_dir="$(mktemp --directory)"
function rm_test_dir {
  rm -rf "$test_dir"
}
trap rm_test_dir EXIT

time_log="${test_dir}/time.log"
make_log="${test_dir}/make.log"

time_verbose_flag=-v
if [ "$(uname)" = 'Darwin' ]; then
  time_verbose_flag=-l
fi

/usr/bin/env time \
  $time_verbose_flag \
  -o "$time_log" \
  make "run_${target}" > "$make_log" 2>&1

result=$(tail -n 1 "$make_log")
expect=$(cat txt/correct/${target})

printf '%s\n' "$target"
printf 'result: %s\n' "$result"
printf 'expect: %s\n' "$expect"

printf 'make:\n'
head -n 1 "$make_log"

printf 'time:\n'
cat "$time_log"
printf '\n'

if [ "$result" != "$expect" ]; then
  exit 1
fi
