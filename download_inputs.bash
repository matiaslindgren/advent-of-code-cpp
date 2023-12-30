#!/usr/bin/env bash
set -ueo pipefail

if [ $# -ne 1 ]; then
  printf "usage: $0 output_dir\n"
  exit 2
fi

output_dir="$1"

function err {
  echo "$@" >> /dev/stderr
  exit 1
}

firefox_dir="$HOME/Library/Application Support/Firefox"
if [ ! -d "$firefox_dir" ]; then
  err "cannot search for firefox cookies in non-existing directory '${firefox_dir}'"
fi

cookie_file=cookies.sqlite

firefox_cookies=$(find "$firefox_dir" -type f -name ${cookie_file} | head -n 1)
if [ ! -f "$firefox_cookies" ]; then
  err "unable to find '${cookie_file}' in '${firefox_dir}'"
fi

tmpdir="$(mktemp --directory)"
function rm_tmpdir {
  rm -rf "$tmpdir"
}
trap rm_tmpdir EXIT

cp "$firefox_cookies" $tmpdir

get_session_value="
  select value from moz_cookies
  where host = '.adventofcode.com' and name = 'session';
"
session_token=$(sqlite3 "${tmpdir}/${cookie_file}" "$get_session_value")
if [ -z "$session_token" ]; then
  err 'unable to find session cookie for host adventofcode.com, log in with firefox to adventofcode.com, then try again'
else
  printf "found adventofcode.com login session token inside '%s'\n" "$firefox_cookies"
fi

exist_count=0
download_count=0

for year in $(seq 2015 $(TZ=America/New_York date '+%Y')); do
  for day in $(seq 1 25); do
    release_date=$(printf "%04d-12-%02d" $year $day)
    if [ $(TZ=America/New_York date -I) '<' "$release_date" ]; then
      echo skip future "$release_date"
      continue
    fi
    output_path=$(printf "%s/%04d/%02d" "$output_dir" $year $day)
    if [ ! -f "$output_path" ]; then
      printf "downloading ${output_path}\n"
      mkdir -p $(dirname "$output_path")
      curl \
        --output "$output_path" \
        --cookie "session=${session_token}" \
        https://adventofcode.com/${year}/day/${day}/input
      sleep 1
      download_count=$(($download_count + 1))
    else
      exist_count=$(($exist_count + 1))
    fi
  done
done

printf "downloaded %d inputs\n" $download_count
printf "skipped %d existing inputs\n" $exist_count
