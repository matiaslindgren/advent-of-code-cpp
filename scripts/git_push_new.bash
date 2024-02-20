#!/usr/bin/env bash
set -xue

if [ $# -ne 2 ]; then
  printf "usage: $0 year day\n"
  exit 2
fi

year="$1"
day="$2"
sol=${year}/${day}

pushd txt
git add correct/${sol}
git commit -m "solve ${sol}"
git push
popd

git add src/${sol}.cpp
git commit -m "solve ${sol}"
