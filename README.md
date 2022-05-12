# How to compile
You compile the whole project with:
```bash
./build_all.sh
```

Or manually with:
```bash
cmake -DCMAKE_BUILD_TYPE=Debug -S . -B build
cmake --build build
```
# Run all unit test
```bash
./run_all.sh
```