# Changelog

All notable changes to SCUnit will be documented in this file.

## 0.3.0 (2025-01-14)

See the [full changelog](https://github.com/Piwimau/SCUnit/compare/0.2.1...0.3.0).

### Features

* Added the ability to randomize the order in which suites and tests are executed
  ([3cedb38](https://github.com/Piwimau/SCUnit/commit/3cedb38ebaee440bd22b6c0ea52b5286c7d7c72a)).
  The default is sequential (`--order=sequential`), which executes suites and tests in the order
  they were originally defined. By specifying the option `--order=random` for the test executable,
  the order is randomized, which might help with finding unintentional dependencies between
  different suites and tests. The seed used for the run is printed out at the end and can be input
  again using the option `--seed=<seed>` to reproduce the results.

  ```plaintext
  --- Summary ---

  Suites: 1 Passed (100.00%), 0 Failed (0.00%), 1 Total
  Tests: 67 Passed (100.00%), 0 Skipped (0.00%), 0 Failed (0.00%), 67 Total
  Wall: 6.123 ms, CPU: 0.000 ns

  Note: Suites and tests were executed in a random order.
  Specify '--seed=42' to reproduce this run.
  ```
* Added a simple pseudorandom number generator (PRNG) based on the
  [xoshiro256**](https://prng.di.unimi.it/) variant
  ([3cedb38](https://github.com/Piwimau/SCUnit/commit/3cedb38ebaee440bd22b6c0ea52b5286c7d7c72a)).

### Changes

* Removed the extensive file comments to prevent them from getting out of sync
  ([60aa47c](https://github.com/Piwimau/SCUnit/commit/60aa47c5edb09c9022fe70ba42e66da43616ab21)).
  They provided little value, as they mostly repeated what was already specified in the
  documentation. More documentation and examples may be added in the future.
* Revised and improved the error handling all around the library
  ([0616d0f](https://github.com/Piwimau/SCUnit/commit/0616d0f2fc4448bbd4c52a1b51b2af6a346d9640),
  [d06feb1](https://github.com/Piwimau/SCUnit/commit/d06feb13f5feb8f5208e910e6a0baf53c7ba2ce9)).
  This results in some breaking changes to the function signatures, but makes the library a bit more
  uniform and ergonomic to use. The following now applies:

  * Pointers are generally assumed to be valid (i. e. not equal to `nullptr` and pointing to valid,
    allocated memory) unless specified otherwise.

  * If an error code provides little extra value to indicate a failure, it is omitted and the return
    value of a function is used instead (if possible).

    ```c
    SCUnitTimer* scunit_timer_new();
    ```

  * If a function has a procedural character (i. e. called primarily for its side-effect),
    the return value of the function is used to return an error code.

    ```c
    SCUnitError scunit_timer_start(SCUnitTimer* timer);
    ```

  * If the primary goal of a function is to compute or return a value, the error code is placed as
    the last out parameter.

    ```c
    SCUnitMeasurement scunit_timer_getWallTime(const SCUnitTimer* timer, SCUnitError* error);
    ```
* Improved the parsing of the command line arguments to support widely used conventions
  ([fe920fb](https://github.com/Piwimau/SCUnit/commit/fe920fb382cc93e5d7a8603fda7f18b0f3bb5a5b)).
  This adds a new dependency, the GNU extension
  [`getopt_long()`](https://linux.die.net/man/3/getopt_long).
* Renamed the option `--colored-output` to `--color` and the possible arguments `disabled` to
  `never` and `enabled` to `always` to follow widely used conventions
  ([1ee265a](https://github.com/Piwimau/SCUnit/commit/1ee265a80df79fb7ff39852dfee930468c08da17)).
  The corresponding functions for getting and setting the colored output in code were moved from the
  [print module](https://github.com/Piwimau/SCUnit/blob/main/include/SCUnit/print.h) to the main
  [scunit module](https://github.com/Piwimau/SCUnit/blob/main/include/SCUnit/scunit.h).
* Splitted `scunit_main()` from
  [`<SCUnit/scunit.h>`](https://github.com/Piwimau/SCUnit/blob/main/include/SCUnit/scunit.h) into
  two separate functions, `scunit_parseArguments()` and `scunit_executeSuites()`, which can now be
  called any number of times
  ([3cedb38](https://github.com/Piwimau/SCUnit/commit/3cedb38ebaee440bd22b6c0ea52b5286c7d7c72a)).

### Fixes

* Fixed various small issues and inconsistencies in the documentation and source code
  ([9bcf8b6](https://github.com/Piwimau/SCUnit/commit/9bcf8b6c66a1c09a81dc5a32cc4a52bb17b8a8e6),
  [1e1bcf0](https://github.com/Piwimau/SCUnit/commit/1e1bcf0a5cffd2238892e8f3d3b668b6e9051491),
  [586267b](https://github.com/Piwimau/SCUnit/commit/586267b7dc95161e09f04807850504ed8ae5555d),
  [4a041a3](https://github.com/Piwimau/SCUnit/commit/4a041a3de7538120899e15d6fe41340e40fc581f)).
* Ensured the proper initialization and deinitialization of SCUnit even if `scunit_executeSuites()`
  is not explicitly called
  ([3cedb38](https://github.com/Piwimau/SCUnit/commit/3cedb38ebaee440bd22b6c0ea52b5286c7d7c72a)).

## 0.2.1 (2024-12-31)

See the [full changelog](https://github.com/Piwimau/SCUnit/compare/0.2.0...0.2.1).

### Changes

* Minor updates to the documentation
  ([3939b8c](https://github.com/Piwimau/SCUnit/commit/3939b8c96524527b4a979cce6934b543172a351b)).
* Removed unnecessary writes in deallocation functions
  ([8b3f16b](https://github.com/Piwimau/SCUnit/commit/8b3f16bd1391adf38703231141dae53563238477)).

### Fixes

* Replaced some calls to `SCUNIT_CALLOC()` with `SCUNIT_MALLOC()` and empty-initialization to avoid
  trap representations
  ([abeb1ea](https://github.com/Piwimau/SCUnit/commit/abeb1eaece7d6a93d7c51de295e449249b8f9e18)).

## 0.2.0 (2024-12-24)

See the [full changelog](https://github.com/Piwimau/SCUnit/compare/0.1.1...0.2.0).

### Features

* Added the ability to query the version information of SCUnit programatically (see
  `scunit_getVersion()` in
  [`<SCUnit/scunit.h>`](https://github.com/Piwimau/SCUnit/blob/main/include/SCUnit/scunit.h)) or by
  passing an option (`-v` or `--version`) to the test executable
  ([4f608e4](https://github.com/Piwimau/SCUnit/commit/4f608e43d623e4707fc9b973c6cd4cfd27711ca5)).

### Changes

* Minor improvements to the SCUnit logo
  ([dbee3a9](https://github.com/Piwimau/SCUnit/commit/dbee3a9e12acb9f840f2db3f251c5a6b5ae43ceb),
  [221098d](https://github.com/Piwimau/SCUnit/commit/221098dcc94dd6a2add8a66fda47ddcd47498de6),
  [8a49d90](https://github.com/Piwimau/SCUnit/commit/8a49d90a0252616960a7c33b5c3587e380cbf846),
  [f711782](https://github.com/Piwimau/SCUnit/commit/f711782d2c3cfba3bb0140c1ee8055ab21c3c6dc)).
* Replaced some macros by proper constants
  ([89f1a8f](https://github.com/Piwimau/SCUnit/commit/89f1a8fffa000bac5745fe586adf434fe06cfebe)).
* Revised README.md
  ([f6d259c](https://github.com/Piwimau/SCUnit/commit/f6d259ccc41cb1a220cb8fe91b188f7bec8b82ac)).
* Simplified the output format by removing the special handling of edge cases
  ([c1a1845](https://github.com/Piwimau/SCUnit/commit/c1a18453baafc5ff627b832ec4d20e75a41b0d68)).
* Renamed various macros, functions and variables for reasons of consistency
  ([5f5f97f](https://github.com/Piwimau/SCUnit/commit/5f5f97f1fbb502b8d3a568181369ef5bafea8ed1)).

### Fixes

* Fixed a missing deallocation in the example shown in the README.md
  ([a549af7](https://github.com/Piwimau/SCUnit/commit/a549af72603d3e05b9dc66cf6561d3f4ef14981d)).
* Fixed a possible divide by zero error and incorrect handling of format specifiers in the summary
  output
  ([fe6cf38](https://github.com/Piwimau/SCUnit/commit/fe6cf38453eb332d11bbbb146669ac385e29c9e9)).

## 0.1.1 (2024-12-20)

See the [full changelog](https://github.com/Piwimau/SCUnit/compare/0.1.0...0.1.1).

### Changes

* Reformatted and renamed a few things for reasons of consistency
  ([1211921](https://github.com/Piwimau/SCUnit/commit/121192140976c0de5a2e9c9af53c12e9f0eef354)).
* Improved and clarified the documentation
  ([5cdb0ef](https://github.com/Piwimau/SCUnit/commit/5cdb0ef3e1d4580f7f0c5ae4257cd33be086e56d)).
* Simplified the display of context lines
  ([3ab8474](https://github.com/Piwimau/SCUnit/commit/3ab8474e526ff3207aa275f977f87d9f049aa228)).

## 0.1.0 (2024-12-19)

Initial release of SCUnit.