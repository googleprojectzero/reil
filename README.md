# REIL

A c++ translation/emulation library for the aarch64 instruction set to REIL.

This is not an officially supported Google product.

## Building

Cmake has been used in an attempt to make downloading and building dependencies painless, but this has had the unfortunate side-effect of making the whole process more complicated than it really needs to be. Sorry.

If you have the necessary dependencies, it should be straightforward to just build the source yourself if you don't like cmake. Apart from Unicorn, the other dependencies are all header-only libraries, and Unicorn is only required to build the tests.

For a normal (cmake) build

``` shell
git clone https://github.com/google/reil
mkdir reil-build
cd reil-build
cmake -G Ninja ../reil
ninja
```

## Running the tests

1. This translation library has a reasonable selection of unit tests to verify the translation against unicorn/qemu. To run the tests for the aarch64 translator, you can use the following commands:

``` shell
./reil-prefix/src/reil-build/aarch64_test
```

## Usage

See the code in reil/test for examples of how to use the translator, and the implementation in reil/interpreter.cpp for the intended semantics of the translated IL instructions.