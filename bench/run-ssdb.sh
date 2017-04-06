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

BIN=/opt/protostuffdb/bin/ssdb-server
DATA_DIR=target/data/main
JAR=../todo-all/target/todo-all-jarjar.jar
ARGS=$(cat ../ARGS.txt)

mkdir -p $DATA_DIR

$BIN run.conf $ARGS $@ -Dprotostuffdb.with_crypto=true -Djava.class.path=$JAR todo.all.Main

