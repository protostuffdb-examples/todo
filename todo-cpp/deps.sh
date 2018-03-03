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

[ -e coreds ] || $GIT_CLONE master https://github.com/fbsgen/coreds.git
[ -e flatbuffers ] || $GIT_CLONE gn https://github.com/fbsgen/flatbuffers.git
[ -e brynet ] || $GIT_CLONE gn https://github.com/dyu/brynet.git

[ -e tiny-process-library ] || $GIT_CLONE gn https://github.com/dyu/tiny-process-library.git
[ -e nana ] || $GIT_CLONE gn https://github.com/dyu/nana.git

