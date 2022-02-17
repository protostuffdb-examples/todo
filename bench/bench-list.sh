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

[ "$1" != "" ] || { echo "First arg (count) is required."; exit 1; }
COUNT=$1
shift

[ ! -n "$PORT" ] && PORT=$(cat ../PORT.txt)
WRK_ARGS=$@
[ $# -gt 0 ] || WRK_ARGS='-t2 -c40 -d10s'

printf "wrk.method = 'POST'\nwrk.body = '{\"1\": true, \"2\": $COUNT}'" > target/list.lua

WRK=./target/bin/wrk
command -v wrk >/dev/null 2>&1 && WRK=wrk

$WRK -s target/list.lua $WRK_ARGS http://127.0.0.1:$PORT/todo/user/Todo/list

