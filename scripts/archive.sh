#!/bin/sh

[ -e scripts ] || { echo 'Execute this script from root dir.'; exit 1; }

TAR_FILE=target/todo-linux-x64.tar.gz
ZIP_FILE=target/todo-win-x64.zip

echo "========== tar.gz"
rm -f $TAR_FILE
cp target/opt-nw todo-ts/todo
tar -cvzf $TAR_FILE target/protostuffdb todo-ts/todo -T scripts/files.txt
rm -f todo-ts/todo

echo "\n========== zip"
rm -f $ZIP_FILE
cp target/opt-nw.exe todo-ts/todo.exe
zip -r $ZIP_FILE target/protostuffdb.exe todo-ts/todo.exe -@ < scripts/files.txt
rm -f todo-ts/todo.exe

