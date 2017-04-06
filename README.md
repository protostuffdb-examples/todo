# full-stack todo app

The app is live at [https://dyuproject.com/todo/](https://dyuproject.com/todo/) and [google play](https://play.google.com/store/apps/details?id=com.dyuproject.todo) (flutter)

## Requirements
- [protostuffdb](https://gitlab.com/dyu/protostuffdb) (download the [binary](https://1drv.ms/f/s!Ah8UGrNGpqlzeAVPYtkNffvNZBo) and move it to /opt/protostuffdb/bin/protostuffdb)
  - When packaging for windows and linux, download **protostuffdb** and **protostuffdb.exe** from the link above and put it in ```target/``` dir
- [node](https://nodejs.org/en/download/) 6.9.0 or higher
- yarn (npm install -g yarn)
- jdk7 (at /usr/lib/jvm/java-7-oracle)
- [maven](https://maven.apache.org/download.cgi)

## Server runtime dependencies
- jdk7

## Desktop runtime dependencies
- jdk7
- [nwjs](https://nwjs.io/) [0.19.5](https://dl.nwjs.io/v0.19.5/) or higher

## Setup
```sh
mkdir -p target/data/main
wget -O target/fbsgen-ds.jar https://repo1.maven.org/maven2/com/dyuproject/fbsgen/ds/fbsgen-ds-fatjar/1.0.4/fbsgen-ds-fatjar-1.0.4.jar
./modules/codegen.sh
mvn install

cd todo-ts
yarn install
```

## Dev mode
```sh
# produces a single jar the first time (todo-all/target/todo-all-jarjar.jar)
./run.sh

# on another terminal
cd todo-ts
# serves the ui via http://localhost:8080/
yarn run dev
```

## Production mode
If ```run.sh``` is still running, stop that process (ctrl+c)
```sh
cd todo-ts
# produces a single js and other assets in todo-ts/dist/
yarn run build
# finally, run your production app
nw .
```

## Packaging for desktop (nwjs)
Make sure these files are in the ```target/``` dir:
- protostuffdb (linux binary)
- protostuffdb.exe (windows binary)

```sh
./scripts/archive.sh
```

That script generates:
- target/todo-linux-x64.tar.gz
- target/todo-win-x64.zip

