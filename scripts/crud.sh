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

PORT=$(cat ../PORT.txt)

list() {
    LIMIT=$1
    [ -z "$LIMIT" ] && LIMIT=50
    curl -X POST "http://127.0.0.1:$PORT/todo/user/Todo/list" \
    -H 'Content-Type: application/json' \
    -d "{\"1\":true,\"2\":$LIMIT}"
}

create() {
    TITLE=$1
    [ -z "$TITLE" ] && TITLE=hello
    curl -X POST http://127.0.0.1:5000/todo/user/Todo/create \
    -H 'Content-Type: application/json' \
    -d "{\"3\":\"$TITLE\"}"
}

CMD=$1
case "$CMD" in
    create)
    shift && create $@
    ;;
    list)
    shift && list $@
    ;;
    *)
    echo 'The available commands are:'
    echo '  - create [ title ]'
    echo '  - list [ limit ]'
    ;;
esac