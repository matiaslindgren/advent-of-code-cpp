#!/usr/bin/env bash
set -ueo pipefail

# this script downloads text inputs and problem descriptions from adventofcode.com
# the data is downloaded with curl, using an existing Firefox session cookie


function error {
  printf "$@\n" >> /dev/stderr
  exit 1
}

cookie_file=cookies.sqlite
aoc_url=adventofcode.com

if [ $# -ne 2 ]; then
  error "\
usage: $0 cookie_dir output_dir\n\
example: $0 \"\$HOME/Library/Application Support/Firefox\" ./txt"
fi

cookie_dir="$1"
output_dir="$2"

if [ -z $(type -p curl) -o -z $(type -p sqlite3) ]; then
  error "cannot find curl and sqlite3"
fi

if [ ! -d "$cookie_dir" ]; then
  error "cannot search for firefox cookies in non-existing directory '${cookie_dir}'"
fi

firefox_cookies=$(find "$cookie_dir" -type f -name ${cookie_file} | head -n 1)
if [ ! -f "$firefox_cookies" ]; then
  error "cannot find '${cookie_file}' within '${cookie_dir}'"
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
session_token=$(sqlite3 "${tmpdir}/${cookie_file}" "$get_session_value")
if [ -z "$session_token" ]; then
  error "unable to find session cookie for host ${aoc_url}, log in with firefox to ${aoc_url}, then try again"
else
  echo "using an $aoc_url login session found inside '$firefox_cookies'"
fi

skipped_problems_count=0
download_problems_count=0
skipped_inputs_count=0
download_inputs_count=0

year_now="$(TZ=America/New_York date '+%Y')"

for y in $(seq 2015 "$year_now"); do
  for d in $(seq 1 25); do
    year=$(printf "%04d" $y)
    day=$(printf "%02d" $d)

    date_now="$(TZ=America/New_York date -I)"
    date_release="${year}-12-${day}"
    if [ "$date_now" '<' "$date_release" ]; then
      break
    fi

    problem_path="${output_dir}/problem/${year}/${day}.html"
    if grep --quiet '<h2 id="part2">' "$problem_path" > /dev/null 2>&1; then
      skipped_problems_count=$(($skipped_problems_count + 1))
    else
      echo "downloading ${problem_path}"
      mkdir -p $(dirname "$problem_path")
      curl \
        --output "$problem_path" \
        --cookie "session=${session_token}" \
        https://${aoc_url}/${y}/day/${d}
      download_problems_count=$(($download_problems_count + 1))
      sleep 1
    fi

    input_path="${output_dir}/input/${year}/${day}"
    if [ -f "$input_path" ]; then
      skipped_inputs_count=$(($skipped_inputs_count + 1))
    else
      echo "downloading ${input_path}"
      mkdir -p $(dirname "$input_path")
      curl \
        --output "$input_path" \
        --cookie "session=${session_token}" \
        https://${aoc_url}/${y}/day/${d}/input
      download_inputs_count=$(($download_inputs_count + 1))
      sleep 1
    fi
  done
done

echo "downloaded $download_problems_count problems"
echo "skipped $skipped_problems_count existing problems"
echo
echo "downloaded $download_inputs_count inputs"
echo "skipped $skipped_inputs_count existing inputs"
