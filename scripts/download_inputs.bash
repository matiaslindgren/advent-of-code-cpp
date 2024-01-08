#!/usr/bin/env bash
set -ueo pipefail

# this script downloads text inputs and problem descriptions from adventofcode.com
# the data is downloaded with curl, using an existing session cookie


function error {
  printf "$@\n" >> /dev/stderr
}

aoc_url=adventofcode.com

if [ $# -ne 2 -a $# -ne 3 ]; then
  error "usage: $0 output_dir session_token [year]"
  error "example: $0 ./txt ./aoc_session"
  error "example: $0 ./txt ./aoc_session 2017"
  exit 2
fi

output_dir="$1"
session_token="$2"

if [ $# -eq 3 ]; then
  year_begin=$3
  year_end=$3
else
  year_begin=2015
  year_end=$(TZ=America/New_York date '+%Y')
fi

if [ -z $(type -p curl) ]; then
  error "cannot find curl"
  exit 1
fi

if [ ! -s "$session_token" ]; then
  error "cannot read session token from non-existing or empty '${session_token}'"
  exit 1
fi

tmpdir="$(mktemp --directory)"
function rm_tmpdir {
  rm -rf "$tmpdir"
}
trap rm_tmpdir EXIT

function download {
  local url="$1"
  local output="$2"
  local token=$(cat "$session_token")
  echo "downloading $output"
  mkdir -p $(dirname "$output")
  curl \
    --output "$output" \
    --cookie "session=${token}" \
    "$url"
}

n_html=0
n_inputs=0

for y in $(seq $year_begin $year_end); do
  for d in $(seq 1 25); do
    year=$(printf "%04d" $y)
    day=$(printf "%02d" $d)

    date_now="$(TZ=America/New_York date -I)"
    date_release="${year}-12-${day}"
    if [ "$date_now" '<' "$date_release" ]; then
      break
    fi

    aoc_day_url=https://${aoc_url}/${y}/day/${d}

    html_path="${output_dir}/html/${year}/${day}.html"
    if ! grep '<h2 id="part2">' "$html_path" > /dev/null 2>&1; then
      download ${aoc_day_url} "$html_path"
      sed -n '/<main>/,/<\/main>/p' "$html_path" > ${tmpdir}/tmp.html
      mv "${tmpdir}/tmp.html" "$html_path"
      n_html=$(($n_html + 1))
      sleep 0.2
    fi

    input_path="${output_dir}/input/${year}/${day}"
    if [ ! -f "$input_path" ]; then
      download ${aoc_day_url}/input "$input_path"
      n_inputs=$(($n_inputs + 1))
      sleep 0.2
    fi
  done
done

echo "downloaded $n_html html pages"
echo "downloaded $n_inputs inputs"
