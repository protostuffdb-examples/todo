#!/bin/sh

[ -e scripts ] || { echo 'Execute this script from root dir.'; exit 1; }

OUT_DIR=target/bin

[ -e $OUT_DIR ] || mkdir -p $OUT_DIR

TAR_FILE=$OUT_DIR/todo-linux-x64.tar.gz
ZIP_FILE=$OUT_DIR/todo-win-x64.zip
WIN_BIN_DIR=target/win

printf todo > target/name.txt

echo "\n========== zip"
rm -f $ZIP_FILE
cp target/opt-nw.exe todo-ts/todo.exe
zip -r $ZIP_FILE target/name.txt target/protostuffdb.exe todo-ts/todo.exe -@ < scripts/files.txt
rm -f todo-ts/todo.exe

echo "========== tar.gz"
rm -f $TAR_FILE
cp target/opt-nw todo-ts/todo
tar -cvzf $TAR_FILE target/name.txt target/protostuffdb todo-ts/todo -T scripts/files.txt
rm -f todo-ts/todo

# ==================================================
# win desktop files
cp target/setup.exe .
cp $WIN_BIN_DIR/todo.exe target/todo.exe

ZIP_FILE=$OUT_DIR/todo-desktop-client-win-x64.zip
rm -f $ZIP_FILE
zip -r $ZIP_FILE setup.exe target/todo.exe target/name.txt target/ARGS.txt assets/*
rm -f target/todo.exe setup.exe

# ==================================================

# linux desktop files
cp target/setup .
cp todo-cpp/gn-out/todo target/todo

TAR_FILE=$OUT_DIR/todo-desktop-client-linux-x64.tar.gz
rm -f $TAR_FILE
tar -cvzf $TAR_FILE setup target/todo target/name.txt target/ARGS.txt assets/*
rm -f target/todo setup

