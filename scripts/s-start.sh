#!/bin/sh
[ -e scripts ] || { echo 'Execute this script from root dir.'; exit 1; }
[ -e target/jre ] || { echo 'The target/jre dir is missing.'; exit 1; }

BIN=target/protostuffdb-rjre
ARGS=$(cat ARGS.txt)
PORT=$(cat PORT.txt)
DATA_DIR=target/data/main
JAR=todo-all/target/todo-all-jarjar.jar
ASSETS='-Dprotostuffdb.assets_dir=todo-ts/'

echo "The app is available at http://127.0.0.1:$PORT"
$BIN $PORT todo-ts/g/user/UserServices.json $ARGS $ASSETS -Djava.class.path=$JAR todo.all.Main

