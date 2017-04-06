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

[ ! -n "$PORT" ] && PORT=$(cat ../PORT.txt)
WRK_ARGS=$@
[ $# -gt 0 ] || WRK_ARGS='-t2 -c40 -d10s'

./target/bin/wrk -s wrk/create.lua $WRK_ARGS http://127.0.0.1:$PORT/todo/user/Todo/create

