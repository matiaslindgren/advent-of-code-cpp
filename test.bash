#!/usr/bin/env bash
set -ue

if [ $# -ne 1 ]; then
  printf 'usage: test.sh executable\n'
  exit 2
fi

target=$1

test_dir="$(mktemp --directory)"
function rm_test_dir {
  rm -rf "$test_dir"
}
trap rm_test_dir EXIT

time_log="${test_dir}/time.log"
time_verbose_flag=-v
if [ "$(uname)" = 'Darwin' ]; then
  time_verbose_flag=-l
fi

printf '%s\n' $target
result=$(\
  /usr/bin/env time \
  $time_verbose_flag \
  -o "$time_log" \
  make --silent run_${target}\
)
expect=$(cat txt/correct/${target})
printf 'result: %s\n' "$result"
printf 'expect: %s\n' "$expect"
printf 'time:\n'
cat "$time_log"
printf '\n'
if [ "$result" != "$expect" ]; then
  exit 1
fi
