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

GIT_CLONE='git clone --depth 1 --single-branch -b'

[ -e gn-deps ] || $GIT_CLONE build https://github.com/dyu/gn-deps.git

./gn-deps/fetch.sh libuv libpng

mkdir -p deps && cd deps

[ -e coreds ] || $GIT_CLONE master https://github.com/fbsgen/coreds.git
[ -e flatbuffers ] || $GIT_CLONE gn https://github.com/fbsgen/flatbuffers.git
[ -e brynet ] || $GIT_CLONE gn https://github.com/dyu/brynet.git
[ -e fmt ] || $GIT_CLONE gn https://github.com/dyu/fmt.git

[ -e coreds-nana ] || $GIT_CLONE master https://github.com/fbsgen/coreds-nana.git
[ -e nana ] || $GIT_CLONE gn https://github.com/dyu/nana.git

[ -e tiny-process-library ] || $GIT_CLONE gn https://github.com/dyu/tiny-process-library.git
