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

# validate node version

print_err() {
    echo 'Running the e2e tests requires node >= v17.5.0'
    exit 1
}

NODE_VERSION=`node --version | cut -c2-`
MAJOR_MINOR=${NODE_VERSION%.*}

MAJOR=${MAJOR_MINOR%.*}
[ "$MAJOR" -lt 17 ] && print_err

MINOR=${MAJOR_MINOR#*.}
[ "$MAJOR" -eq 17 ] && [ "$MINOR" -lt 5 ] && print_err

#ARGS='index'
#[ -z "$1" ] || ARGS=$@

PORT=$(cat ../PORT.txt)
UNAME=`uname`
if [ "$UNAME" != "Linux" ] && [ "$UNAME" != "Darwin" ]; then
RPC_HOST="http://127.0.0.1:$PORT" NODE_OPTIONS='--experimental-fetch' \
npm run e2e -- $@
else
RPC_HOST="http://127.0.0.1:$PORT" node --experimental-fetch \
./node_modules/.bin/uvu -r tsm -r esm test/uvu $@
fi
