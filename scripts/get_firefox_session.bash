#!/usr/bin/env bash
set -ueo pipefail

# extract a Firefox session cookie for adventofcode.com


function error {
  printf "$@\n" >> /dev/stderr
}

cookie_file=cookies.sqlite
aoc_url=adventofcode.com

if [ $# -ne 2 ]; then
  error "usage: $0 cookie_dir output_path"
  error "example: $0 \"\$HOME/Library/Application Support/Firefox\" ./aoc_session"
  exit 2
fi

cookie_dir="$1"
output_path="$2"

if [ -z $(type -p sqlite3) ]; then
  error "cannot find sqlite3"
  exit 1
fi

if [ ! -d "$cookie_dir" ]; then
  error "cannot search for firefox cookies in non-existing directory '${cookie_dir}'"
  exit 1
fi

firefox_cookies=$(find "$cookie_dir" -type f -name ${cookie_file} | head -n 1)
if [ ! -f "$firefox_cookies" ]; then
  error "cannot find '${cookie_file}' within '${cookie_dir}'"
  exit 1
fi

tmpdir="$(mktemp --directory)"
function rm_tmpdir {
  rm -rf "$tmpdir"
}
trap rm_tmpdir EXIT

cp "$firefox_cookies" $tmpdir

get_session_value="
  select value from moz_cookies
  where host = '.${aoc_url}' and name = 'session';
"
sqlite3 "${tmpdir}/${cookie_file}" "$get_session_value" > "$output_path"
if [ ! -s "$output_path" ]; then
  error "unable to find session cookie for host ${aoc_url}, log in with firefox to ${aoc_url}, then try again"
  exit 1
fi
