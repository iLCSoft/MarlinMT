## MarlinMT testing

To run unit tests, make sure the CMake switch is on:

```shell
cmake -DBUILD_TESTING=ON [OTHER OPTIONS] ..
```

You can run test after compilation using:

```shell
# install step is optional before running tests
make # install
# use --output-on-failure in case a test
# is failing, then you get the standard output
ctest --output-on-failure
```
