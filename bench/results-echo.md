### My environment
- Intel i7-3630QM laptop (4cores, HT) with 16g ram
- Ubuntu 14.04 x64

### jvm args
-Xms128m -Xmx128m -XX:NewRatio=1

## Echo key
```
./bench-echo.sh
Running 30s test @ http://127.0.0.1:5000/todo/user/echoKey
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   408.08us  657.27us  33.40ms   98.91%
    Req/Sec    53.12k     6.11k   59.83k    94.33%
  3171210 requests in 30.00s, 290.33MB read
Requests/sec: 105702.91
Transfer/sec:      9.68MB

./bench-echo.sh
Running 30s test @ http://127.0.0.1:5000/todo/user/echoKey
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   372.82us  218.42us   9.73ms   96.79%
    Req/Sec    54.96k     3.06k  106.87k    84.86%
  3286146 requests in 30.10s, 300.86MB read
Requests/sec: 109173.99
Transfer/sec:     10.00MB
```
### nginx -> pdb
```
PORT=8080 ./bench-echo.sh
Running 30s test @ http://127.0.0.1:8080/todo/user/echoKey
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   833.61us  683.04us  35.75ms   99.37%
    Req/Sec    24.68k     2.30k   50.20k    95.17%
  1475774 requests in 30.10s, 242.00MB read
Requests/sec:  49029.18
Transfer/sec:      8.04MB

PORT=8080 ./bench-echo.sh
Running 30s test @ http://127.0.0.1:8080/todo/user/echoKey
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   829.55us  148.69us  12.22ms   93.56%
    Req/Sec    24.24k     1.97k   26.37k    91.20%
  1452441 requests in 30.10s, 238.18MB read
Requests/sec:  48253.75
Transfer/sec:      7.91MB
```
### nginx -> ssdb
```
PORT=8080 ./bench-echo.sh 
Running 30s test @ http://127.0.0.1:8080/todo/user/echoKey
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     0.97ms  638.20us  33.34ms   99.05%
    Req/Sec    21.05k     2.14k   42.39k    93.18%
  1258644 requests in 30.10s, 272.42MB read
Requests/sec:  41816.09
Transfer/sec:      9.05MB

PORT=8080 ./bench-echo.sh 
Running 30s test @ http://127.0.0.1:8080/todo/user/echoKey
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     0.94ms  164.46us  21.46ms   98.03%
    Req/Sec    21.36k   522.44    21.91k    95.00%
  1274907 requests in 30.00s, 275.94MB read
Requests/sec:  42496.68
Transfer/sec:      9.20MB
```

### nginx W2 -> pdb
```
PORT=8080 ./bench-echo.sh
Running 30s test @ http://127.0.0.1:8080/todo/user/echoKey
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   537.11us  739.18us  33.42ms   99.03%
    Req/Sec    39.70k     6.62k   62.49k    74.04%
  2373813 requests in 30.10s, 389.27MB read
Requests/sec:  78864.58
Transfer/sec:     12.93MB

PORT=8080 ./bench-echo.sh
Running 30s test @ http://127.0.0.1:8080/todo/user/echoKey
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   481.62us  546.55us  21.62ms   99.02%
    Req/Sec    44.63k     6.36k  103.10k    54.58%
  2668616 requests in 30.10s, 437.61MB read
Requests/sec:  88659.71
Transfer/sec:     14.54MB
```
### nginx W2 -> ssdb
```
PORT=8080 ./bench-echo.sh
Running 30s test @ http://127.0.0.1:8080/todo/user/echoKey
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   633.28us  841.54us  40.38ms   99.46%
    Req/Sec    33.08k     6.31k   45.79k    82.72%
  1980914 requests in 30.10s, 428.74MB read
Requests/sec:  65811.24
Transfer/sec:     14.24MB

PORT=8080 ./bench-echo.sh
Running 30s test @ http://127.0.0.1:8080/todo/user/echoKey
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   545.34us  172.22us   9.21ms   94.46%
    Req/Sec    36.89k     3.97k   45.76k    78.33%
  2202681 requests in 30.00s, 476.74MB read
Requests/sec:  73421.95
Transfer/sec:     15.89MB
```

## Echo str
```
./bench-echo.sh 1234567abcde
Running 30s test @ http://127.0.0.1:5000/todo/user/echoStr
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   388.85us  491.92us  29.12ms   98.87%
    Req/Sec    54.36k     7.04k  112.91k    87.69%
  3250709 requests in 30.10s, 297.61MB read
Requests/sec: 107997.96
Transfer/sec:      9.89MB

./bench-echo.sh 1234567abcde
Running 30s test @ http://127.0.0.1:5000/todo/user/echoStr
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   362.95us  175.90us   9.79ms   96.46%
    Req/Sec    56.01k     2.35k   62.63k    76.74%
  3355454 requests in 30.10s, 307.20MB read
Requests/sec: 111476.04
Transfer/sec:     10.21MB
```
### nginx -> pdb
```
PORT=8080 ./bench-echo.sh 1234567abcde
Running 30s test @ http://127.0.0.1:8080/todo/user/echoStr
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   847.30us  713.52us  36.46ms   99.14%
    Req/Sec    24.37k     2.30k   26.70k    92.86%
  1459585 requests in 30.10s, 239.35MB read
Requests/sec:  48491.12
Transfer/sec:      7.95MB

PORT=8080 ./bench-echo.sh 1234567abcde
Running 30s test @ http://127.0.0.1:8080/todo/user/echoStr
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   820.33us  175.93us  20.36ms   95.69%
    Req/Sec    24.54k     1.68k   48.88k    92.18%
  1467339 requests in 30.10s, 240.62MB read
  Non-2xx or 3xx responses: 4
Requests/sec:  48749.25
Transfer/sec:      7.99MB
```
### nginx -> ssdb
```
PORT=8080 ./bench-echo.sh 1234567abcde
Running 30s test @ http://127.0.0.1:8080/todo/user/echoStr
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     0.97ms  615.61us  32.71ms   98.93%
    Req/Sec    21.16k     1.83k   42.77k    94.18%
  1265340 requests in 30.10s, 273.87MB read
Requests/sec:  42038.64
Transfer/sec:      9.10MB

PORT=8080 ./bench-echo.sh 1234567abcde
Running 30s test @ http://127.0.0.1:8080/todo/user/echoStr
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     0.93ms  127.90us  15.76ms   98.09%
    Req/Sec    21.51k   415.26    22.41k    93.36%
  1288132 requests in 30.10s, 278.80MB read
Requests/sec:  42795.13
Transfer/sec:      9.26MB
```

### nginx W2 -> pdb
```
PORT=8080 ./bench-echo.sh 1234567abcde
Running 30s test @ http://127.0.0.1:8080/todo/user/echoStr
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   481.98us  600.85us  29.69ms   98.74%
    Req/Sec    44.16k     7.53k   82.59k    77.37%
  2640879 requests in 30.10s, 433.06MB read
Requests/sec:  87737.29
Transfer/sec:     14.39MB

PORT=8080 ./bench-echo.sh 1234567abcde
Running 30s test @ http://127.0.0.1:8080/todo/user/echoStr
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   479.04us  328.47us  14.19ms   96.81%
    Req/Sec    42.96k     5.42k   52.27k    64.00%
  2565169 requests in 30.00s, 420.65MB read
Requests/sec:  85499.57
Transfer/sec:     14.02MB
```
### nginx W2 -> ssdb
```
PORT=8080 ./bench-echo.sh 1234567abcde
Running 30s test @ http://127.0.0.1:8080/todo/user/echoStr
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   639.32us  418.70us  24.20ms   98.50%
    Req/Sec    31.86k     6.40k   66.74k    87.35%
  1905457 requests in 30.10s, 412.41MB read
Requests/sec:  63303.99
Transfer/sec:     13.70MB

PORT=8080 ./bench-echo.sh 1234567abcde
Running 30s test @ http://127.0.0.1:8080/todo/user/echoStr
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   630.81us  139.15us   9.35ms   86.44%
    Req/Sec    31.75k     4.95k   45.23k    73.50%
  1895464 requests in 30.00s, 410.25MB read
Requests/sec:  63181.16
Transfer/sec:     13.67MB
```

## Echo int
```
./bench-echo.sh 1234567
Running 30s test @ http://127.0.0.1:5000/todo/user/echoInt
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   383.44us  638.63us  33.76ms   99.01%
    Req/Sec    56.00k     7.21k  115.40k    95.34%
  3348501 requests in 30.10s, 284.21MB read
Requests/sec: 111245.37
Transfer/sec:      9.44MB

./bench-echo.sh 1234567
Running 30s test @ http://127.0.0.1:5000/todo/user/echoInt
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   346.48us  129.73us   9.74ms   96.26%
    Req/Sec    58.21k     3.68k   63.66k    67.00%
  3475545 requests in 30.00s, 294.99MB read
Requests/sec: 115849.46
Transfer/sec:      9.83MB
```
### nginx -> pdb
```
PORT=8080 ./bench-echo.sh 1234567
Running 30s test @ http://127.0.0.1:8080/todo/user/echoInt
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   828.10us  366.32us  22.47ms   98.45%
    Req/Sec    24.53k     1.73k   26.16k    94.83%
  1464129 requests in 30.00s, 230.32MB read
Requests/sec:  48801.99
Transfer/sec:      7.68MB

PORT=8080 ./bench-echo.sh 1234567
Running 30s test @ http://127.0.0.1:8080/todo/user/echoInt
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   809.36us  143.49us  16.92ms   97.86%
    Req/Sec    24.84k     1.29k   50.48k    97.17%
  1485975 requests in 30.10s, 233.76MB read
  Non-2xx or 3xx responses: 4
Requests/sec:  49368.69
Transfer/sec:      7.77MB
```
### nginx -> ssdb
```
PORT=8080 ./bench-echo.sh 1234567
Running 30s test @ http://127.0.0.1:8080/todo/user/echoInt
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     0.97ms  490.01us  27.97ms   98.86%
    Req/Sec    20.97k     2.04k   22.24k    89.70%
  1255982 requests in 30.10s, 263.46MB read
Requests/sec:  41726.96
Transfer/sec:      8.75MB

PORT=8080 ./bench-echo.sh 1234567
Running 30s test @ http://127.0.0.1:8080/todo/user/echoInt
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     0.93ms   89.07us   8.06ms   98.08%
    Req/Sec    21.53k   349.58    22.07k    91.20%
  1289809 requests in 30.10s, 270.55MB read
Requests/sec:  42850.79
Transfer/sec:      8.99MB
```

### nginx W2 -> pdb
```
PORT=8080 ./bench-echo.sh 1234567
Running 30s test @ http://127.0.0.1:8080/todo/user/echoInt
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   550.37us  549.23us  30.68ms   98.92%
    Req/Sec    37.61k     8.72k   70.69k    77.54%
  2249010 requests in 30.10s, 353.79MB read
Requests/sec:  74718.44
Transfer/sec:     11.75MB

PORT=8080 ./bench-echo.sh 1234567
Running 30s test @ http://127.0.0.1:8080/todo/user/echoInt
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   484.35us  148.35us   6.74ms   81.26%
    Req/Sec    41.34k     5.38k   52.18k    69.33%
  2468262 requests in 30.00s, 388.28MB read
Requests/sec:  82270.39
Transfer/sec:     12.94MB
```
### nginx W2 -> ssdb
```
PORT=8080 ./bench-echo.sh 1234567
Running 30s test @ http://127.0.0.1:8080/todo/user/echoInt
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   577.77us  811.48us  39.43ms   99.36%
    Req/Sec    36.37k     5.31k   46.10k    76.50%
  2171564 requests in 30.00s, 455.51MB read
Requests/sec:  72377.39
Transfer/sec:     15.18MB

PORT=8080 ./bench-echo.sh 1234567
Running 30s test @ http://127.0.0.1:8080/todo/user/echoInt
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   541.81us  120.77us   6.10ms   80.57%
    Req/Sec    36.88k     5.55k   67.80k    63.89%
  2205168 requests in 30.10s, 462.56MB read
Requests/sec:  73261.26
Transfer/sec:     15.37MB
```
### nginx W2 -> ssdb R2
```
PORT=8080 ./bench-echo.sh 1234567
Running 30s test @ http://127.0.0.1:8080/todo/user/echoInt
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   614.55us  454.60us  25.55ms   99.30%
    Req/Sec    33.27k     5.17k   52.78k    61.40%
  1989940 requests in 30.10s, 417.41MB read
Requests/sec:  66112.07
Transfer/sec:     13.87MB

PORT=8080 ./bench-echo.sh 1234567
Running 30s test @ http://127.0.0.1:8080/todo/user/echoInt
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   584.72us  220.90us  16.16ms   96.09%
    Req/Sec    34.38k     4.49k   70.53k    79.37%
  2056261 requests in 30.10s, 431.32MB read
Requests/sec:  68313.04
Transfer/sec:     14.33MB
```
### nginx W2 -> ssdb R3
```
PORT=8080 ./bench-echo.sh 1234567
Running 30s test @ http://127.0.0.1:8080/todo/user/echoInt
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   646.13us  625.25us  32.34ms   99.50%
    Req/Sec    31.99k     4.21k   40.48k    46.00%
  1909671 requests in 30.00s, 400.57MB read
Requests/sec:  63651.90
Transfer/sec:     13.35MB

PORT=8080 ./bench-echo.sh 1234567
Running 30s test @ http://127.0.0.1:8080/todo/user/echoInt
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   621.73us  239.39us  16.07ms   97.66%
    Req/Sec    32.45k     3.33k   60.00k    68.72%
  1940073 requests in 30.10s, 406.95MB read
Requests/sec:  64455.06
Transfer/sec:     13.52MB
```

### nginx W3 -> pdb
```
PORT=8080 ./bench-echo.sh 1234567
Running 30s test @ http://127.0.0.1:8080/todo/user/echoInt
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   438.36us  634.39us  30.84ms   98.63%
    Req/Sec    49.44k    10.37k   60.22k    77.00%
  2951488 requests in 30.00s, 464.29MB read
Requests/sec:  98378.42
Transfer/sec:     15.48MB

PORT=8080 ./bench-echo.sh 1234567
Running 30s test @ http://127.0.0.1:8080/todo/user/echoInt
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   383.19us  227.45us  11.01ms   96.99%
    Req/Sec    53.34k     7.95k   60.42k    78.17%
  3183960 requests in 30.00s, 500.86MB read
Requests/sec: 106130.85
Transfer/sec:     16.70MB
```
### nginx W3 -> ssdb
```
PORT=8080 ./bench-echo.sh 1234567
Running 30s test @ http://127.0.0.1:8080/todo/user/echoInt
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   525.57us  516.99us  27.17ms   98.61%
    Req/Sec    39.64k     4.17k   57.76k    93.51%
  2370695 requests in 30.10s, 497.28MB read
Requests/sec:  78760.46
Transfer/sec:     16.52MB

PORT=8080 ./bench-echo.sh 1234567
Running 30s test @ http://127.0.0.1:8080/todo/user/echoInt
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   532.44us  133.34us   6.23ms   97.03%
    Req/Sec    37.53k     2.88k   64.18k    67.89%
  2243874 requests in 30.10s, 470.68MB read
Requests/sec:  74547.96
Transfer/sec:     15.64MB
```
### nginx W3 -> ssdb R2
```
PORT=8080 ./bench-echo.sh 1234567
Running 30s test @ http://127.0.0.1:8080/todo/user/echoInt
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   488.86us  458.86us  23.85ms   98.98%
    Req/Sec    42.40k     3.09k   45.68k    96.33%
  2531936 requests in 30.00s, 531.10MB read
Requests/sec:  84385.55
Transfer/sec:     17.70MB

PORT=8080 ./bench-echo.sh 1234567
Running 30s test @ http://127.0.0.1:8080/todo/user/echoInt
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   502.62us  187.15us  11.81ms   94.37%
    Req/Sec    39.86k     1.71k   44.58k    70.50%
  2380021 requests in 30.00s, 499.24MB read
Requests/sec:  79324.88
Transfer/sec:     16.64MB
```
### nginx W3 -> ssdb R3
```
PORT=8080 ./bench-echo.sh 1234567
Running 30s test @ http://127.0.0.1:8080/todo/user/echoInt
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   507.70us  741.83us  33.89ms   98.76%
    Req/Sec    42.62k     2.98k   48.46k    93.17%
  2544372 requests in 30.01s, 533.71MB read
Requests/sec:  84787.50
Transfer/sec:     17.79MB

PORT=8080 ./bench-echo.sh 1234567
Running 30s test @ http://127.0.0.1:8080/todo/user/echoInt
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   506.63us  276.74us  12.67ms   95.26%
    Req/Sec    39.98k     1.74k   45.17k    71.59%
  2394880 requests in 30.10s, 502.35MB read
Requests/sec:  79561.38
Transfer/sec:     16.69MB
```

