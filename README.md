# full-stack todo app

A full-stack todo app with:
- c++ backend (leveldb + uWebSockets) with java (jni) stored procs
- dart (flutter) mobile frontend (todo-dart) available [here](https://play.google.com/store/apps/details?id=com.dyuproject.todo)
- typescript (vuets) web frontend (todo-ts) available [here](https://apps.dyuproject.com/todo/)
- c++ (nana) desktop frontend (todo-cpp)
  - [linux64 ~980kb](https://unpkg.com/pdb-todo@0.5.1/bin/todo-desktop-client-linux-x64.tar.gz)
  - [win64 ~1.59mb](https://unpkg.com/pdb-todo@0.5.1/bin/todo-desktop-client-win-x64.zip)

The frontends above connect to the same [rpc endpoint](https://todo.dyuproject.com)

The web and mobile apps have the same swipe/mouse semantics.
Swipe horizontally to nagivate the pages (paginate).
Long-press the item to update.

The web and desktop apps have the same keyboard shortcuts.

![screenshot](https://github.com/protostuffdb-examples/todo/raw/master/screencast.gif)
![screenshot](https://github.com/protostuffdb-examples/todo/raw/master/screenie-win10.png)
![screenshot](https://github.com/protostuffdb-examples/todo/raw/master/screenie-lubuntu.png)
![screenshot](https://github.com/protostuffdb-examples/todo/raw/master/screenie-ubuntu.png)
![screenshot](https://github.com/protostuffdb-examples/todo/raw/master/screenie-win8.png)

Desktop-standalone (client+server+jre) downloads:
 - [linux64 ~13.22mb](https://unpkg.com/pdb-todo@0.5.0/bin/todo-desktop-standalone-linux-x64.tar.gz)
 - [win64 ~17.12mb](https://unpkg.com/pdb-todo@0.5.0/bin/todo-desktop-standalone-win-x64.zip)

## Server runtime dependencies
- jdk7

## Desktop dev requirements (c++)
- [ninja](https://ninja-build.org/)
- [gn](https://refi64.com/gn-builds/)

## Web dev requirements
- [node](https://nodejs.org/en/download/) 6.9.0 or higher
- [yarn](https://yarnpkg.com/en/)

## Common dev requirements
- jdk7 (at /usr/lib/jvm/java-7-oracle)
- [maven](https://maven.apache.org/download.cgi)
- [protostuffdb](https://gitlab.com/dyu/protostuffdb) (downloaded below)

## Setup
```sh
mkdir -p target/data/main
echo "Your data lives in user/ dir.  Feel free to back it up." > target/data/main/README.txt

# download protostuffdb
yarn add protostuffdb@0.19.0 && mv node_modules/protostuffdb/dist/* target/ && rm -f package.json yarn.lock && rm -r node_modules

wget -O target/fbsgen-ds.jar https://repo1.maven.org/maven2/com/dyuproject/fbsgen/ds/fbsgen-ds-fatjar/1.0.17/fbsgen-ds-fatjar-1.0.17.jar
./modules/codegen.sh
mvn install
```

### To build the c++ desktop frontend, visit [todo-cpp/README.md](todo-cpp/README.md)

### To build the dart mobile frontend, visit [todo-dart/README.md](todo-dart/README.md)

## Web dev setup
```sh
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

## Memory usage on ubuntu 14.04 x64

### nana (8mb)
```
ps_mem.py -p 14073
 Private  +   Shared  =  RAM used	Program

  7.5 MiB + 544.0 KiB =   8.0 MiB	todo
---------------------------------
                          8.0 MiB
=================================
```

### nana + jvm (38mb)
```
ps_mem.py -p 13930
 Private  +   Shared  =  RAM used	Program

  7.1 MiB + 362.5 KiB =   7.4 MiB	todo-pdb
---------------------------------
                          7.4 MiB
=================================

ps_mem.py -p 13934
 Private  +   Shared  =  RAM used	Program

 30.2 MiB + 387.0 KiB =  30.6 MiB	protostuffdb-rjre
---------------------------------
                         30.6 MiB
=================================
```

### nwjs + jvm (107.5mb)
```
ps_mem.py -p 13497
 Private  +   Shared  =  RAM used	Program

 37.0 MiB +   8.3 MiB =  45.3 MiB	nw
---------------------------------
                         45.3 MiB
=================================

ps_mem.py -p 13499
 Private  +   Shared  =  RAM used	Program

392.0 KiB +   7.6 MiB =   8.0 MiB	nw
---------------------------------
                          8.0 MiB
=================================

ps_mem.py -p 13532
 Private  +   Shared  =  RAM used	Program

 48.1 MiB +   6.1 MiB =  54.2 MiB	nw
---------------------------------
                         54.2 MiB
=================================
```

### flutter-desktop (106.7mb)
- engine version: ed303c628fe4b322529f8cf01ecb38135a2bab73 (2018-04-10)
- build type: host_release (optimized)

```
ps_mem.py -p 12136
 Private  +   Shared  =  RAM used	Program

105.3 MiB +   1.4 MiB = 106.7 MiB	run
---------------------------------
                        106.7 MiB
=================================
```

The [most basic example app](https://github.com/dyu/flutter-desktop-embedding/tree/gn/example_flutter) from flutter consumes [100mb](https://github.com/dyu/flutter-desktop-embedding/blob/gn/README-gn.md#memory-usage-of-the-app-example_flutter) 
