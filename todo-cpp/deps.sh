#!/bin/sh

CURRENT_DIR=$PWD
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

mkdir -p deps && cd deps

GIT_CLONE='git clone --depth 1 --single-branch -b'

[ -e flatbuffers ] || $GIT_CLONE gn https://github.com/fbsgen/flatbuffers.git
[ -e nana ] || $GIT_CLONE gn https://github.com/dyu/nana.git
[ -e brynet ] || $GIT_CLONE gn https://github.com/dyu/brynet.git
# doesn't compile on msvc 2015
#[ -e asap ] || $GIT_CLONE gn https://github.com/dyu/asap.git
