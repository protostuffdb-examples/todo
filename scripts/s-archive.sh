#!/bin/sh

[ -e scripts ] || { echo 'Execute this script from root dir.'; exit 1; }

OUT_DIR=target/bin

[ -e $OUT_DIR ] || mkdir -p $OUT_DIR

TAR_FILE=$OUT_DIR/todo-server-standalone-linux-x64.tar.gz

TAR_ARGS='target/name.txt target/protostuffdb-rjre target/jre/*/'
ZIP_ARGS='target/name.txt target/protostuffdb-sp.exe target/jre/*/'
WIN_BIN_DIR=target/win

printf todo > target/name.txt

echo "========== tar.gz"
rm -f $TAR_FILE
echo '#!/bin/sh' > start.sh && tail --lines=+4 scripts/s-start.sh >> start.sh && chmod +x start.sh && \
    tar -cvzf $TAR_FILE start.sh $TAR_ARGS -T scripts/files.txt
rm -f start.sh

# ==================================================

# win desktop files
cp target/setup.exe .
cp $WIN_BIN_DIR/todo-pdb.exe target/todo.exe
# link jre
cd target && rm -f jre && ln -s jre-win jre && cd ..

ZIP_FILE=$OUT_DIR/todo-desktop-standalone-win-x64.zip
rm -f $ZIP_FILE
zip -r $ZIP_FILE setup.exe target/todo.exe assets/* $ZIP_ARGS -@ < target/files.txt
rm -f target/todo.exe setup.exe

# ==================================================

# linux desktop files
head --lines=6 scripts/files.txt > target/files.txt
cp target/setup .
cp todo-cpp/gn-out/todo-pdb target/todo
# link jre
cd target && rm -f jre && ln -s jre-linux jre && cd ..

TAR_FILE=$OUT_DIR/todo-desktop-standalone-linux-x64.tar.gz
rm -f $TAR_FILE
tar -cvzf $TAR_FILE setup target/todo assets/* $TAR_ARGS -T target/files.txt
rm -f target/todo setup



