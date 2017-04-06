### My environment
- Intel i7-3630QM laptop (4cores, HT) with 16g ram
- Ubuntu 14.04 x64

### jvm args
-Xms128m -Xmx128m -XX:NewRatio=1

At the time of this writing, the version used are:
- leveldb 1.19
- snappy 1.1.4

Based on the results: 
- **hyperleveldb** is 15.85% faster on writes
- both are more-or-less the same speed on gets
- both are more-or-less the same speed on range scan descending (10)
- both are more-or-less the same speed on range scan descending (100)

## Create (stock)
```
./bench-create.sh 
Running 10s test @ http://127.0.0.1:5000/todo/user/Todo/create
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     0.99ms    1.56ms  45.47ms   97.68%
    Req/Sec    23.23k     5.66k   45.44k    90.55%
  464582 requests in 10.10s, 67.35MB read
Requests/sec:  45998.87
Transfer/sec:      6.67MB

./bench-create.sh 
Running 10s test @ http://127.0.0.1:5000/todo/user/Todo/create
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     0.87ms  333.87us  11.02ms   95.52%
    Req/Sec    23.51k     2.56k   26.72k    75.00%
  467965 requests in 10.00s, 67.84MB read
Requests/sec:  46794.13
Transfer/sec:      6.78MB
```
### Inserted entries in 20 seconds
- 932623 on primary index
- 932623 on secondary index (```bool completed```)

## Create (hyper)
```
./bench-create.sh 
Running 10s test @ http://127.0.0.1:5000/todo/user/Todo/create
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     1.57ms    7.86ms 126.19ms   98.65%
    Req/Sec    26.86k     6.52k   32.06k    91.00%
  534400 requests in 10.00s, 77.47MB read
Requests/sec:  53431.78
Transfer/sec:      7.75MB

./bench-create.sh 
Running 10s test @ http://127.0.0.1:5000/todo/user/Todo/create
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     2.21ms   11.52ms 155.19ms   97.96%
    Req/Sec    27.35k     4.31k   31.93k    88.38%
  540708 requests in 10.00s, 78.38MB read
Requests/sec:  54067.16
Transfer/sec:      7.84MB
```
### Inserted entries in 20 seconds
- 1075184 on primary index
- 1075184 on secondary index (```bool completed```)

## Get (stock)
```
./bench-get.sh CgFa/o/POiCZ
Running 10s test @ http://127.0.0.1:5000/todo/user/Todo/list
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   807.60us    1.66ms  45.54ms   98.41%
    Req/Sec    29.36k     6.35k   33.24k    92.00%
  584515 requests in 10.00s, 84.73MB read
Requests/sec:  58446.00
Transfer/sec:      8.47MB

./bench-get.sh CgFa/o/POiCZ
Running 10s test @ http://127.0.0.1:5000/todo/user/Todo/list
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   644.55us  213.11us  10.21ms   95.51%
    Req/Sec    31.40k     2.36k   62.44k    96.02%
  627802 requests in 10.10s, 91.01MB read
Requests/sec:  62157.78
Transfer/sec:      9.01MB
```

## Get (hyper)
Calling ```list``` with ```{ limit: 0, start_key: $KEY }``` is internally a get.
```
./bench-get.sh CgFa/o/POiCZ
Running 10s test @ http://127.0.0.1:5000/todo/user/Todo/list
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   795.15us    1.50ms  42.99ms   98.40%
    Req/Sec    29.31k     6.23k   33.67k    92.00%
  583289 requests in 10.00s, 84.55MB read
Requests/sec:  58320.17
Transfer/sec:      8.45MB

./bench-get.sh CgFa/o/POiCZ
Running 10s test @ http://127.0.0.1:5000/todo/user/Todo/list
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   654.10us  168.68us   8.42ms   95.90%
    Req/Sec    30.82k     0.96k   32.93k    71.00%
  613316 requests in 10.00s, 88.91MB read
Requests/sec:  61326.49
Transfer/sec:      8.89MB
```

## List 10 desc (stock)
```
./bench-list.sh 10
Running 10s test @ http://127.0.0.1:5000/todo/user/Todo/list
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     1.29ms    1.79ms  59.67ms   98.12%
    Req/Sec    17.48k     3.47k   19.20k    91.09%
  351205 requests in 10.10s, 259.24MB read
Requests/sec:  34774.17
Transfer/sec:     25.67MB

./bench-list.sh 10
Running 10s test @ http://127.0.0.1:5000/todo/user/Todo/list
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     1.08ms  268.78us   9.98ms   95.83%
    Req/Sec    18.73k     1.42k   37.36k    97.51%
  374616 requests in 10.10s, 276.52MB read
Requests/sec:  37091.57
Transfer/sec:     27.38MB
```

## List 10 desc (hyper)
```
./bench-list.sh 10
Running 10s test @ http://127.0.0.1:5000/todo/user/Todo/list
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     1.25ms    1.44ms  51.20ms   97.78%
    Req/Sec    17.61k     3.72k   37.42k    90.55%
  352145 requests in 10.10s, 259.93MB read
Requests/sec:  34867.98
Transfer/sec:     25.74MB

./bench-list.sh 10
Running 10s test @ http://127.0.0.1:5000/todo/user/Todo/list
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     1.10ms  197.87us   6.23ms   94.77%
    Req/Sec    18.30k     0.90k   19.39k    91.09%
  367861 requests in 10.10s, 271.53MB read
Requests/sec:  36423.11
Transfer/sec:     26.89MB
```

## List 100 desc (stock)
```
./bench-list.sh 100
Running 10s test @ http://127.0.0.1:5000/todo/user/Todo/list
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     4.34ms    5.32ms 144.81ms   98.35%
    Req/Sec     5.17k     0.94k   10.75k    84.58%
  103560 requests in 10.10s, 689.86MB read
Requests/sec:  10253.72
Transfer/sec:     68.30MB

./bench-list.sh 100
Running 10s test @ http://127.0.0.1:5000/todo/user/Todo/list
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     3.68ms  631.81us  16.75ms   96.67%
    Req/Sec     5.46k   178.56     5.70k    72.00%
  108607 requests in 10.00s, 723.48MB read
Requests/sec:  10857.68
Transfer/sec:     72.33MB
```

## List 100 desc (hyper)
```
./bench-list.sh 100
Running 10s test @ http://127.0.0.1:5000/todo/user/Todo/list
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     4.38ms    5.48ms 126.14ms   98.46%
    Req/Sec     5.16k   787.16     5.70k    85.43%
  102272 requests in 10.00s, 681.28MB read
Requests/sec:  10223.07
Transfer/sec:     68.10MB

./bench-list.sh 100
Running 10s test @ http://127.0.0.1:5000/todo/user/Todo/list
  2 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     3.72ms  616.67us  14.84ms   96.60%
    Req/Sec     5.41k   187.13     5.77k    68.00%
  107649 requests in 10.00s, 717.09MB read
Requests/sec:  10761.75
Transfer/sec:     71.69MB
```

