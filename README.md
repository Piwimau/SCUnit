<p align="center">
    <img src="resources/SCUnit.png" alt="SCUnit Logo" width="50%"/>
</p>

SCUnit is a small, easy-to-use unit testing framework for C. With no complex configuration or setup
required, it allows you to focus on the important things: Writing and executing tests.

## Why should you use SCUnit?

SCUnit is written using the popular [XUnit](https://en.wikipedia.org/wiki/XUnit) style. If you've
ever worked with a framework like [NUnit](https://nunit.org) or [JUnit](https://junit.org/junit5)
before, you'll feel right at home. Even if you've never done any unit testing, you won't face a
steep learning curve as SCUnit is specifically designed to be small and easy to use. All you need to
get started is a compliant C23 compiler, include SCUnit in your sources and link it as a library to
your test executable.

Before we go into more detail, here's a simple example of what testing with SCUnit looks like:

```c
#include <SCUnit/scunit.h>

SCUNIT_SUITE(ExampleSuite);

SCUNIT_TEST(ExampleSuite, ExampleTest) {
    Widget* widget = widget_new();
    SCUNIT_ASSERT_NOT_NULL(widget);
    SCUNIT_ASSERT_TRUE(widget->isAvailable);
    SCUNIT_ASSERT_GREATER_OR_EQUAL(widget->capacity, 42);
    widget_free(widget);
}

int main(int argc, char** argv) {
    scunit_parseArguments(argc, argv);
    return scunit_executeSuites();
}
```

Some of the key features for which you might consider using SCUnit include:

* Automatic discovery and registration of tests and suites.
* A minimal, yet useful set of assertions for checking various conditions in tests.
* Ability to group logically related tests into suites. Particularly large suites can even be
  distributed across multiple source files for readability.
* Support for suite or test setup and teardown functions.
* Accurate and optionally colored diagnostic output on `stdout` and `stderr` that not only informs
  you of the status of a test, but also includes execution time measurements and some helpful
  context when an assertion fails.
* Ability to configure SCUnit (if required at all) by passing command line arguments to the test
  executable.

Some other nice features that probably aren't that relevant for normal users, but might be for you
if you want to dive deeper and take full advantage of SCUnit:

* A well-documented API (down to the very last implementation detail) that allows you to understand
  why SCUnit is written the way it is and how it is intended to be used.
* Consistent and predictable error handling using an error enumeration.
* Ability to manually register and execute suites and tests using a normal function API if you need
  the full control.
* Ability to replace the functions for dynamic memory management (e. g. for debugging purposes).
* A utility module for printing formatted and optionally colored strings to streams and buffers.
* A simple timer for measuring the elapsed wall or CPU time required to execute a block of code.
* A pseudorandom number generator (PRNG) based on the [xoshiro256**](https://prng.di.unimi.it/)
  variant.

## How do you build SCUnit?

SCUnit is written in pure C23 and does not have many dependencies besides the C standard library and
a compliant C23 compiler. Specifically, it does use three features that might not be available on
all platforms:

* The automatic allocation, registration and deallocation of suites and tests is implemented using
  compiler-specific attributes called `__attribute__((constructor))` and
  `__attribute__((destructor))` (or `[[gnu::constructor]]` and `[[gnu::destructor]]` in the new C23
  syntax), which cause an annotated function to be executed before (and after) `main()` itself is.
  These attributes are supported by GCC and Clang, but not by MSVC, which shouldn't be too much of
  an issue since it doesn't have adequate C23 support at the moment anyway. If you absolutely need
  the automatic registration in conjunction with MSVC, you should be able to find various
  workarounds related to linker sections fairly easily on internet.
* The simple timer used by SCUnit to measure the elapsed wall and CPU time of suites and tests is
  implemented using the POSIX function
  [`clock_gettime()`](https://man7.org/linux/man-pages/man3/clock_gettime.3.html). This is mainly
  due to the lack of other suitable and portable options in the C standard library.
  Generally speaking, it should be available on MacOS and Linux, but not on Windows. In the latter
  case, I can highly recommend [MSYS2](https://www.msys2.org) as a solution. Another alternative
  would be to rely on a platform-specific, more feature-rich timer and change the underlying
  implementation.
* Command line arguments passed to the test executable are parsed using the function
  [`getopt_long()`](https://linux.die.net/man/3/getopt_long), which is a GNU extension of
  [`getopt()`](https://www.man7.org/linux/man-pages/man3/getopt.3.html) to support long command line
  options like `--help` or `--version` in addition to the standard short ones.

If your platform supports these features, you can go ahead and build SCUnit from source using the
provided [Makefile](Makefile). Run `make help` first to see a useful overview of all options,
which should produce something like this:

```plaintext
Usage: make [TARGET]... [VARIABLE]...

Targets:
  all     Build both a static and shared library (default).
  static  Build only a static library.
  shared  Build only a shared library.
  clean   Remove all build artifacts.
  help    Display this help.

Variables:
  BUILD_TYPE={debug|release}  Set the build type (default = release).
```

SCUnit can be built and linked as a static or shared library, whichever you prefer. Run `make all`
or simply `make` to build both (the default). If you need a variant for debugging purposes,
change the build type from `release` (the default) to `debug` by defining `BUILD_TYPE=debug`.
Debug variants are identified by an additional `d` suffix in the library name (e. g. `libscunitd.a`
for a static library built in debug mode). They are not optimized and contain various debug symbols
to provide a better debugging experience.

All binaries are generated in the [bin](bin/) directory. Here's a quick overview of the different
variants that can be built (links only work after the specific variant has been built):

| Build Type / Library Type | Static                                        | Shared                                          |
|---------------------------|-----------------------------------------------|-------------------------------------------------|
| Debug                     | [libscunitd.a](bin/debug/static/libscunitd.a) | [libscunitd.so](bin/debug/shared/libscunitd.so) |
| Release                   | [libscunit.a](bin/release/static/libscunit.a) | [libscunit.so](bin/release/shared/libscunit.so) |

## How do you use SCUnit?

This is hard to answer, since I don't know the exact structure or build systems you use in your
project. However, these are the general steps you need to follow:

1. Build your preferred variant of SCUnit and place it in an appropriate directory along with the
   [headers](include/SCUnit/).

2. Include the main header [`<SCUnit/scunit.h>`](include/SCUnit/scunit.h) in your source files and
   start writing tests. You'll probably spend most of your time working with the contents of the
   [`<SCUnit/suite.h>`](include/SCUnit/suite.h) and [`<SCUnit/assert.h>`](include/SCUnit/assert.h)
   headers, which contain the main functionality needed to define suites, tests, setup and teardown
   functions, as well as assertions to be used in tests.

3. Link SCUnit as a static or shared library when you build your test executable.

See the individual headers for detailed documentation on the interface and intended usage.

## Why was SCUnit created?

Besides the occasional learning experience, SCUnit gave me a chance to try out modern C23 in
practice. I hadn't done any unit testing in a long time and needed a way to test some of my own side
projects, including the last and current year of [Advent of Code](https://adventofcode.com),
which I plan on doing in C this time.

Of course, there are other great unit testing frameworks available for C, but they are often huge
or cumbersome to work with in my personal opinion, especially if tests and suites have to be
registered manually, or if a lot of configuration and setup code is required. Smaller frameworks,
ranging from tiny (essentially just providing a single `assert` macro) to very powerful, often
didn't provide the exact set of features I was looking for, including style, consistent error
handling, and being written and intended for use in pure C.

## Contributing

I don't really expect any contributions, as SCUnit is more or less just one of my little side
projects. However, if you find a bug or have a nice suggestion, feel free to open an issue and I'll
see what I can do.

## Acknowledgements

I originally got the idea to create SCUnit when I stumbled across
[this](https://www.youtube.com/watch?v=z-uWt5wVVkU) nice talk by Benno Rice, in which he presented
some of the issues related to unit testing C code and two example frameworks one could use.

SCUnit was inspired by many other existing frameworks, including (but not limited to)
[Google Test](https://github.com/google/googletest), [Check](https://github.com/libcheck/check),
[Unity](https://github.com/ThrowTheSwitch/Unity), [Tau](https://github.com/jasmcaus/tau),
[µnit](https://nemequ.github.io/munit), [tst](https://github.com/rdentato/tst) and
[MinUnit](https://github.com/siu/minunit).

## License

SCUnit is licensed under the MIT License. See the [LICENSE](LICENSE) file for more information.