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
./bench-create.sh
./bench-list.sh 10
./bench-list.sh 100
```


