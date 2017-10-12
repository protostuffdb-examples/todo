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

jarjar() {
  cd ../todo-all
  rm -f target/*.jar
  mvn -o -Pjwd -Djwd=compile -Dmaven.javadoc.skip=true compile
  cd - > /dev/null
}

ARGS_TXT=$(cat ../ARGS.txt)

MASTER_PORT=$1
if [ "$MASTER_PORT" = "" ]; then
    MASTER_PORT=$(cat ../PORT.txt)
else
    ARGS_TXT="$ARGS_TXT -Dprotostuffdb.rep_idle_refresh_interval=1"
fi

BIN=/opt/protostuffdb/bin/protostuffdb-rslave
ARGS="$ARGS_TXT -Dprotostuffdb.with_backup=true -Dprotostuffdb.master=ws://127.0.0.1:$MASTER_PORT"

DATA_DIR=target/data/main
JAR=../todo-all/target/todo-all-jarjar.jar
PORT=$(cat PORT.txt)

[ -e $JAR ] || jarjar

mkdir -p $DATA_DIR

$BIN $PORT ../todo-ts/g/user/UserServices.json $ARGS -Djava.class.path=$JAR todo.all.Main

