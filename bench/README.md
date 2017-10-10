## Build deps
```sh
./deps.sh
```

## Tune
```sh
sudo sysctl -w net.core.somaxconn=1024
# verify
cat /proc/sys/net/core/somaxconn
```

## Run
```sh
./run.sh
```

## Test via curl
```
curl http://127.0.0.1:5000/todo/user/echoInt -X POST -d @payload/echoInt.json
curl http://127.0.0.1:5000/todo/user/echoStr -X POST -d @payload/echoStr.json
curl http://127.0.0.1:5000/todo/user/echoKey -X POST -d @payload/echoKey.json
curl http://127.0.0.1:5000/todo/user/Todo/create -X POST -d @payload/create.json
curl http://127.0.0.1:5000/todo/user/Todo/list -X POST -d @payload/get.json
curl http://127.0.0.1:5000/todo/user/Todo/list -X POST -d @payload/list.json
```

## Bench
```
./bench-echo.sh
./bench-create.sh
./bench-list.sh 10
./bench-list.sh 100
```

## local bench (no networking involved)
```
./run-uri.sh protostuffdb /todo/user/generateInt /dev/null
./run-uri.sh protostuffdb /todo/user/parseInt payload/echoInt.json
./run-uri.sh protostuffdb /todo/user/echoInt payload/echoInt.json
./run-uri.sh protostuffdb /todo/user/echoStr payload/echoStr.json
./run-uri.sh protostuffdb /todo/user/echoKey payload/echoKey.json
./run-uri.sh protostuffdb /todo/user/Todo/create payload/create.json
./run-uri.sh protostuffdb /todo/user/Todo/list payload/get.json
./run-uri.sh protostuffdb /todo/user/Todo/list payload/list.json 1000000
```

