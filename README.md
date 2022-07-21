# How to compile
You compile the whole project with:
```bash
python3 builder.py --build
```

Or manually with:
```bash
cmake -DCMAKE_BUILD_TYPE=Debug -S . -B build
cmake --build build -j $(nproc)
```
# Run all unit test
Use the provided python script to choose the tests you would like to run

```bash
# Run all tests
python3 builder.py -r

# Or manually with
ctest -j $(nproc) --test-dir build
```

To run a specific test, first list the tests and use the name to run 
the test you are looking for
```bash
$ python3 builder.py -l 

circular_list_dlist
hashtable_dlist
dlist_adt
avl_bst_adt
heap_adt
queue_dlist
stack_adt
treemap_avl_bst
```

The use the name to run the test
```bash
$ python3 builder.py -t stack_adt 

[==========] Running 5 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 5 tests from StackTestFixture
[ RUN      ] StackTestFixture.TestPopValue
[       OK ] StackTestFixture.TestPopValue (0 ms)
[ RUN      ] StackTestFixture.TestPeekValue
[       OK ] StackTestFixture.TestPeekValue (0 ms)
[ RUN      ] StackTestFixture.TestPopAll
[       OK ] StackTestFixture.TestPopAll (0 ms)
[ RUN      ] StackTestFixture.TestFetchNthItem
[       OK ] StackTestFixture.TestFetchNthItem (0 ms)
[ RUN      ] StackTestFixture.DumpStack
[       OK ] StackTestFixture.DumpStack (0 ms)
[----------] 5 tests from StackTestFixture (0 ms total)

[----------] Global test environment tear-down
[==========] 5 tests from 1 test suite ran. (0 ms total)
[  PASSED  ] 5 tests.
```
