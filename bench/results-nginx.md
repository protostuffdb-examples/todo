### My environment
- Intel i7-3630QM laptop (4cores, HT) with 16g ram
- Ubuntu 14.04 x64

### jvm args
-Xms128m -Xmx128m -XX:NewRatio=1

At the time of this writing, the versions used are:
- leveldb 1.19
- snappy 1.1.4

## Create
```
./bench-create.sh
Running 10s test @ http://127.0.0.1:5000/todo/user/Todo/create
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     1.05ms    1.64ms  39.68ms   97.62%
    Req/Sec    22.15k     5.29k   48.80k    89.55%
  442896 requests in 10.10s, 64.20MB read
Requests/sec:  43850.49
Transfer/sec:      6.36MB

./bench-create.sh
Running 10s test @ http://127.0.0.1:5000/todo/user/Todo/create
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     0.88ms  233.73us   6.86ms   88.06%
    Req/Sec    23.05k     2.20k   26.07k    76.73%
  463123 requests in 10.10s, 67.13MB read
Requests/sec:  45855.29
Transfer/sec:      6.65MB
```

### Inserted entries in 20 seconds
- 906095 on primary index
- 906095 on secondary index (```bool completed```)

## Get
Calling ```list``` with ```{ limit: 0, start_key: $KEY }``` is internally a get.
```
./bench-get.sh CgFa9U8PUCCZ
Running 10s test @ http://127.0.0.1:5000/todo/user/Todo/list
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   831.75us    1.36ms  42.27ms   97.63%
    Req/Sec    27.85k     6.87k   60.29k    89.55%
  556974 requests in 10.10s, 80.74MB read
Requests/sec:  55146.75
Transfer/sec:      7.99MB

./bench-get.sh CgFa9U8PUCCZ
Running 10s test @ http://127.0.0.1:5000/todo/user/Todo/list
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   685.53us  304.92us  10.04ms   96.78%
    Req/Sec    29.82k     0.97k   32.13k    77.50%
  593548 requests in 10.00s, 86.04MB read
Requests/sec:  59346.99
Transfer/sec:      8.60MB
```
### nginx -> ssdb
```
PORT=8080 ./bench-get.sh CgFa9U8PUCCZ
Running 10s test @ http://127.0.0.1:8080/todo/user/Todo/list
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     1.11ms    1.53ms  43.38ms   98.27%
    Req/Sec    19.86k     4.12k   22.56k    83.66%
  399199 requests in 10.10s, 107.72MB read
Requests/sec:  39524.73
Transfer/sec:     10.67MB

PORT=8080 ./bench-get.sh CgFa9U8PUCCZ
Running 10s test @ http://127.0.0.1:8080/todo/user/Todo/list
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     0.94ms  210.46us  14.78ms   90.54%
    Req/Sec    21.44k     2.19k   43.44k    90.05%
  428636 requests in 10.10s, 115.66MB read
Requests/sec:  42441.13
Transfer/sec:     11.45MB
```
### nginx W2 -> ssdb
```
PORT=8080 ./bench-get.sh CgFa9U8PUCCZ
Running 10s test @ http://127.0.0.1:8080/todo/user/Todo/list
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     0.85ms    1.07ms  33.09ms   96.68%
    Req/Sec    25.90k     7.62k   40.61k    70.65%
  517939 requests in 10.10s, 139.76MB read
Requests/sec:  51283.48
Transfer/sec:     13.84MB

PORT=8080 ./bench-get.sh CgFa9U8PUCCZ
Running 10s test @ http://127.0.0.1:8080/todo/user/Todo/list
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   660.11us  152.86us   4.91ms   87.91%
    Req/Sec    30.39k     4.19k   36.98k    55.00%
  604595 requests in 10.00s, 163.15MB read
Requests/sec:  60456.88
Transfer/sec:     16.31MB
```
### nginx W2 -> ssdb R2
```
PORT=8080 ./bench-get.sh CgFa9U8PUCCZ
Running 10s test @ http://127.0.0.1:8080/todo/user/Todo/list
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   735.74us    1.43ms  40.58ms   99.32%
    Req/Sec    30.64k     6.00k   38.30k    78.71%
  615781 requests in 10.10s, 166.16MB read
Requests/sec:  60968.88
Transfer/sec:     16.45MB

PORT=8080 ./bench-get.sh CgFa9U8PUCCZ
Running 10s test @ http://127.0.0.1:8080/todo/user/Todo/list
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   588.32us  185.86us   6.36ms   95.52%
    Req/Sec    34.22k     3.98k   64.47k    78.61%
  684196 requests in 10.10s, 184.63MB read
Requests/sec:  67744.17
Transfer/sec:     18.28MB
```
### nginx W2 -> ssdb R3
```
PORT=8080 ./bench-get.sh CgFa9U8PUCCZ
Running 10s test @ http://127.0.0.1:8080/todo/user/Todo/list
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   816.45us    2.29ms  55.43ms   99.19%
    Req/Sec    30.20k     4.57k   38.34k    85.00%
  601047 requests in 10.00s, 162.19MB read
Requests/sec:  60101.36
Transfer/sec:     16.22MB

PORT=8080 ./bench-get.sh CgFa9U8PUCCZ
Running 10s test @ http://127.0.0.1:8080/todo/user/Todo/list
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   621.34us  148.28us   6.47ms   92.27%
    Req/Sec    32.20k     2.73k   38.71k    67.50%
  640818 requests in 10.00s, 172.92MB read
Requests/sec:  64053.68
Transfer/sec:     17.28MB
```

## List
### 10
```
./bench-list.sh 10
Running 10s test @ http://127.0.0.1:5000/todo/user/Todo/list
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     1.58ms    1.84ms  56.34ms   97.89%
    Req/Sec    13.85k     3.16k   31.06k    89.55%
  277048 requests in 10.10s, 204.50MB read
Requests/sec:  27431.13
Transfer/sec:     20.25MB

./bench-list.sh 10
Running 10s test @ http://127.0.0.1:5000/todo/user/Todo/list
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     1.19ms  250.95us   9.61ms   93.70%
    Req/Sec    16.99k     1.60k   19.39k    71.00%
  338168 requests in 10.00s, 249.62MB read
Requests/sec:  33814.03
Transfer/sec:     24.96MB
```
### 10, nginx -> ssdb
```
PORT=8080 ./bench-list.sh 10
Running 10s test @ http://127.0.0.1:8080/todo/user/Todo/list
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     1.24ms    1.36ms  40.32ms   97.75%
    Req/Sec    17.63k     4.20k   38.86k    88.56%
  352407 requests in 10.10s, 304.14MB read
Requests/sec:  34892.59
Transfer/sec:     30.11MB

PORT=8080 ./bench-list.sh 10
Running 10s test @ http://127.0.0.1:8080/todo/user/Todo/list
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     1.09ms  222.59us  15.62ms   97.24%
    Req/Sec    18.53k     1.88k   20.21k    69.50%
  368796 requests in 10.00s, 318.28MB read
Requests/sec:  36877.72
Transfer/sec:     31.83MB
```
### 10, nginx W2 -> ssdb
```
PORT=8080 ./bench-list.sh 10
Running 10s test @ http://127.0.0.1:8080/todo/user/Todo/list
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     1.27ms    1.76ms  49.02ms   97.67%
    Req/Sec    17.98k     4.42k   36.71k    89.55%
  359750 requests in 10.10s, 310.48MB read
Requests/sec:  35620.25
Transfer/sec:     30.74MB

PORT=8080 ./bench-list.sh 10
Running 10s test @ http://127.0.0.1:8080/todo/user/Todo/list
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     1.08ms  224.37us   7.14ms   86.46%
    Req/Sec    18.63k     2.78k   38.73k    84.58%
  372678 requests in 10.10s, 321.63MB read
Requests/sec:  36899.32
Transfer/sec:     31.85MB
```

### 100
```
./bench-list.sh 100
Running 10s test @ http://127.0.0.1:5000/todo/user/Todo/list
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     4.29ms    4.60ms 132.61ms   98.33%
    Req/Sec     5.14k   828.24     5.79k    85.00%
  102331 requests in 10.01s, 681.67MB read
Requests/sec:  10226.84
Transfer/sec:     68.13MB

./bench-list.sh 100
Running 10s test @ http://127.0.0.1:5000/todo/user/Todo/list
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     3.75ms  626.04us  15.53ms   96.63%
    Req/Sec     5.36k   223.77     5.75k    63.00%
  106589 requests in 10.00s, 710.03MB read
Requests/sec:  10656.08
Transfer/sec:     70.98MB
```
### 100, nginx -> ssdb
```
PORT=8080 ./bench-list.sh 100
Running 10s test @ http://127.0.0.1:8080/todo/user/Todo/list
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     4.29ms    4.55ms 102.32ms   98.39%
    Req/Sec     5.17k     0.87k    5.65k    83.50%
  102857 requests in 10.00s, 698.71MB read
Requests/sec:  10285.06
Transfer/sec:     69.87MB

PORT=8080 ./bench-list.sh 100
Running 10s test @ http://127.0.0.1:8080/todo/user/Todo/list
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     3.67ms  287.62us   9.41ms   91.61%
    Req/Sec     5.47k   293.42     5.66k    91.00%
  108939 requests in 10.00s, 740.03MB read
Requests/sec:  10890.08
Transfer/sec:     73.98MB
```
### 100, nginx W2 -> ssdb
```
PORT=8080 ./bench-list.sh 100
Running 10s test @ http://127.0.0.1:8080/todo/user/Todo/list
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     4.15ms    4.56ms 101.90ms   98.49%
    Req/Sec     5.36k     0.88k    5.82k    86.00%
  106765 requests in 10.00s, 725.26MB read
Requests/sec:  10671.94
Transfer/sec:     72.50MB

PORT=8080 ./bench-list.sh 100
Running 10s test @ http://127.0.0.1:8080/todo/user/Todo/list
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     3.53ms  161.89us   8.92ms   98.84%
    Req/Sec     5.69k    94.84     5.81k    85.50%
  113202 requests in 10.00s, 768.98MB read
Requests/sec:  11317.67
Transfer/sec:     76.88MB
```

