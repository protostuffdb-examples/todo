#!/bin/sh

[ -e scripts ] || { echo 'Execute this script from root dir.'; exit 1; }

OUT_DIR=target/bin

[ -e $OUT_DIR ] || mkdir -p $OUT_DIR

TAR_FILE=$OUT_DIR/todo-server-standalone-linux-x64.tar.gz

TAR_ARGS='target/name.txt target/protostuffdb-rjre target/jre/*/'
[ "$1" != "" ] && TAR_ARGS=$1

printf todo > target/name.txt

echo "========== tar.gz"
rm -f $TAR_FILE
echo '#!/bin/sh' > start.sh && tail --lines=+4 scripts/s-start.sh >> start.sh && chmod +x start.sh && \
    tar -cvzf $TAR_FILE start.sh $TAR_ARGS -T scripts/files.txt
rm start.sh

# desktop files
head --lines=6 scripts/files.txt > target/files.txt
cp todo-cpp/gn-out/todo-pdb target/todo

TAR_FILE=$OUT_DIR/todo-desktop-standalone-linux-x64.tar.gz
rm -f $TAR_FILE
cp target/setup .
tar -cvzf $TAR_FILE setup target/todo assets/* $TAR_ARGS -T target/files.txt
rm setup
