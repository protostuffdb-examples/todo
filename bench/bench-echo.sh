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

if [ "$1" = "" ]; then
    TYPE=Key
    printf "wrk.method = 'POST'\nwrk.body = '{\"1\":\"AAAAAAAAAAAA\"}'" > target/echo$TYPE.lua
elif [ "$1" -eq "$1" ] 2>/dev/null; then
    TYPE=Int
    printf "wrk.method = 'POST'\nwrk.body = '{\"1\":$1}'" > target/echo$TYPE.lua
else
    TYPE=Str
    printf "wrk.method = 'POST'\nwrk.body = '{\"1\":\"$1\"}'" > target/echo$TYPE.lua
fi

WRK_ARGS='-t2 -c40 -d30s'

./target/bin/wrk -s target/echo$TYPE.lua $WRK_ARGS http://127.0.0.1:$PORT/todo/user/echo$TYPE

