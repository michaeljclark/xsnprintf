# xsnprintf

> simplified high-performance snprintf replacement.

- succinct and simplified snprintf implementation.
- fast integer to string using a combination of tactics:
  divide and conquer, base 100 table plus Duff's device.

## building

```
cmake -G Ninja -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build
```

## testing

```
./build/xsnprintf_test
./build/xsnprintf_bench
```
