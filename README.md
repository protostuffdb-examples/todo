# full-stack todo app

It can be seen live [here](https://apps.dyuproject.com/todo/) with equivalent [android](https://play.google.com/store/apps/details?id=com.dyuproject.todo) (flutter) and desktop (nwjs) apps.

They all have the same semantics.
Swipe horizontally to nagivate the pages (paginate).
Long-press the item to update.

## Server runtime dependencies
- jdk7

## Desktop runtime dependencies
- jdk7
- [nwjs](https://nwjs.io/) [0.19.5](https://dl.nwjs.io/v0.19.5/) or higher

### Running the desktop app without building
- download either **todo-linux-x64.tar.gz** or **todo-win-x64.zip** from [here](https://1drv.ms/f/s!Ah8UGrNGpqlzeAVPYtkNffvNZBo)
- extract it and cd into the directory
- finally, exec: ```nw todo-ts```

## Dev requirements
- [node](https://nodejs.org/en/download/) 6.9.0 or higher
- yarn (npm install -g yarn)
- jdk7 (at /usr/lib/jvm/java-7-oracle)
- [maven](https://maven.apache.org/download.cgi)
- [protostuffdb](https://gitlab.com/dyu/protostuffdb) (downloaded below)

## Setup
```sh
mkdir -p target/data/main
echo "Your data lives in user/ dir.  Feel free to back it up." > target/data/main/README.txt

# download protostuffdb
yarn add protostuffdb@0.13.1 && mv node_modules/protostuffdb/dist/* target/ && rm -f package.json yarn.lock && rm -r node_modules

wget -O target/fbsgen-ds.jar https://repo1.maven.org/maven2/com/dyuproject/fbsgen/ds/fbsgen-ds-fatjar/1.0.9/fbsgen-ds-fatjar-1.0.9.jar
./modules/codegen.sh
mvn install

cd todo-ts
yarn install

# build css
yarn run ns.b
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
# or
node chrome-app.js
```

## Packaging for desktop (nwjs)
Exec
```sh
./scripts/archive.sh
```

That script generates:
- target/todo-linux-x64.tar.gz
- target/todo-win-x64.zip

