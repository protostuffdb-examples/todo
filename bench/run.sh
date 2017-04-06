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

if [ -n "$1" ] && [ -e "/opt/protostuffdb/bin/$1" ]; then
    echo "$1"
    BIN="/opt/protostuffdb/bin/$1"
elif [ -e /opt/protostuffdb/bin/protostuffdb ]; then
    BIN=/opt/protostuffdb/bin/protostuffdb
elif [ -e target/protostuffdb ]; then
    BIN=./target/protostuffdb
else
    echo 'The target/protostuffdb binary must exist.  Download that file from https://1drv.ms/f/s!Ah8UGrNGpqlzeAVPYtkNffvNZBo'
    exit 1
fi

DATA_DIR=target/data/main
JAR=../todo-all/target/todo-all-jarjar.jar
ARGS=$(cat ../ARGS.txt)
PORT=$(cat ../PORT.txt)

mkdir -p $DATA_DIR

$BIN $PORT ../todo-ts/g/user/UserServices.json $ARGS -Djava.class.path=$JAR todo.all.Main

