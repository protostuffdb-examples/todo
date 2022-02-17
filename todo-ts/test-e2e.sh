#!/bin/sh

set -e

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

ARGS='index'
[ -z "$1" ] || ARGS=$@

PORT=$(cat ../PORT.txt)
RPC_HOST="http://127.0.0.1:$PORT" node --experimental-fetch \
./node_modules/.bin/uvu -r tsm -r esm test/uvu $ARGS
