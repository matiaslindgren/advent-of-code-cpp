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

if [ $# -ne 2 -a $# -ne 3 ]; then
  error "\
usage: $0 cookie_dir output_dir [year]\n\
example: $0 \"\$HOME/Library/Application Support/Firefox\" ./txt"
fi

cookie_dir="$1"
output_dir="$2"

if [ $# -eq 3 ]; then
  year_begin=$3
  year_end=$3
else
  year_begin=2015
  year_end=$(TZ=America/New_York date '+%Y')
fi

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

function download {
  local url="$1"
  local output="$2"
  echo "downloading $output"
  mkdir -p $(dirname "$output")
  curl \
    --output "$output" \
    --cookie "session=${session_token}" \
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
