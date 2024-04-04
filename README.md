# mpmc-zmqpp

A multi publisher - multi subscriber system implementation using ZMQ and C++. Project is designed to have an intermediate(proxy) to which all the 
subscribers and publishers connect. The proxy manages subscriptions as well as publisher messages. 

Each publisher produces a specific stream of data prefix_timestamp_suffixMSGNo, ex. PROD1_TIMESTAMP_MSG1
Each subscriber can subscribe/unsubscribe to one or multiple topics.

To build the project you need to have conan and cmake installed.

```
$ cd |publisher|proxy|subscriber|/
$ mkdir build && cd build
$ conan install .. -of cmake-build-release --build=missing
$ cmake .. -DCMAKE_TOOLCHAIN_FILE=cmake-build-release/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
$ cmake --build .
```
To run each one of the following (publisher, proxy, subscribe)

```
$ ./proxy
$ ./publisher prefix,suffix,interval prefix,suffix,interval ... -- starts 3 publishers with respective prefix, suffix and interval
$ ./subscriber prefix1,prefix2 prefix2,prefix7 ... starts 2 subscribers with topics to subscribe to
```
