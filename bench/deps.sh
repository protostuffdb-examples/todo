#!/bin/sh

# locate
if [ ! -n "$BASH_SOURCE" ]; then
    SCRIPT_DIR=`dirname "$(readlink -f "$0")"`
else
    F=$BASH_SOURCE
    while [ -h "$F" ]; do
        F="$(readlink "$F")"
    done
    SCRIPT_DIR=`dirname "$F"`
fi

cd $SCRIPT_DIR

GIT_CLONE="git clone --depth 1 --single-branch -b"

mkdir -p target/builds target/bin
cd target/builds

# wrk
if [ ! -e wrk ]; then
  $GIT_CLONE master https://github.com/wg/wrk.git && cd wrk
  make && cd ../../bin && ln -s ../builds/wrk/wrk
fi

