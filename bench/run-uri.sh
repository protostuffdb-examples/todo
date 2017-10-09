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

if [ -n "$1" ] && [ -e "../target/$1-bench" ]; then
    echo "$1"
    BIN="../target/$1-bench"
elif [ -n "$1" ] && [ -e "/opt/protostuffdb/bin/$1-bench" ]; then
    echo "$1"
    BIN="/opt/protostuffdb/bin/$1-bench"
elif [ -e target/protostuffdb-bench ]; then
    BIN=./target/protostuffdb-bench
else
    echo 'The target/protostuffdb binary must exist.  Download that file from https://1drv.ms/f/s!Ah8UGrNGpqlzeAVPYtkNffvNZBo'
    exit 1
fi

URI=$2
[ -n "$URI" ] || { echo "Execute with (hprotostuffdb|protostuffdb) uri [ payload, iterations, warmups ]"; exit 1; }

BENCH_ARGS="-Dbench.uri=$URI"

PAYLOAD=$3
[ -n "$PAYLOAD" ] && BENCH_ARGS="$BENCH_ARGS -Dbench.payload=$PAYLOAD"

ITERATIONS=$4
[ -n "$ITERATIONS" ] && BENCH_ARGS="$BENCH_ARGS -Dbench.iterations=$ITERATIONS"

WARMUPS=$5
[ -n "$WARMUPS" ] && BENCH_ARGS="$BENCH_ARGS -Dbench.warmups=$WARMUPS"

DATA_DIR=target/data/main
JAR=../todo-all/target/todo-all-jarjar.jar
ARGS=$(cat ../ARGS.txt)
PORT=$(cat ../PORT.txt)

mkdir -p $DATA_DIR

$BIN $PORT ../todo-ts/g/user/UserServices.json $ARGS $BENCH_ARGS -Djava.class.path=$JAR todo.all.Main

