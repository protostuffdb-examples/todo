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

## Bench
```
./bench-echo.sh
./bench-create.sh
./bench-list.sh 10
./bench-list.sh 100
```

## local bench (no networking involved)
```
./run-uri.sh protostuffdb /todo/user/echoInt payload/echoInt.json
./run-uri.sh protostuffdb /todo/user/echoStr payload/echoStr.json
./run-uri.sh protostuffdb /todo/user/echoKey payload/echoKey.json
./run-uri.sh protostuffdb /todo/user/Todo/create payload/create.json
./run-uri.sh protostuffdb /todo/user/Todo/list payload/get.json
./run-uri.sh protostuffdb /todo/user/Todo/list payload/list.json
```
