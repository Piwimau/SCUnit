/**
 * @file suite.h
 *
 * @brief A suite to group logically related tests together.
 *
 * @details Suites work in pretty much the same way as you would expect if you have ever used a unit
 * testing framework before. They are identified by a name and contain zero or more logically
 * related tests to be executed. SCUnit provides two ways of working with suites and tests.
 * A brief explanation for both of them:
 *
 * - One way is to use convenient macros such as `SCUNIT_SUITE()` and `SCUNIT_TEST()` which do some
 *   clever tricks under the hood. If you use these in conjunction with the `scunit_main()` function
 *   from `<SCUnit/scunit.h>`, all suites and tests will be discovered, registered and run
 *   automatically without you having to setup anything. Note however that they rely on a compiler-
 *   specific (though widely supported) attribute called `__attribute__((constructor))` (or
 *   ``[[gnu::constructor]]` in the new syntax) to implement the automatic registration.
 *
 * - If you are not a fan of macro tricks, or your compiler does not support the attribute, you may
 *   also use the normal function API (which is used by the macros under the hood anyway, but in a
 *   convenient, automated fashion) to register and run suites and tests manually.
 *
 * The general process for setting up suites and tests using either the convenience macros or the
 * normal function API is as follows:
 *
 * 1. Initialize a new `SCUnitSuite` with `SCUNIT_SUITE()` or `scunit_suite_new()`. `SCUNIT_SUITE()`
 *    automatically registers the new suite to be run if used in conjunction with `scunit_main()`.
 *
 *    #include <SCUnit/scunit.h>
 *
 *    SCUNIT_SUITE(AutomaticSuite);
 *
 *    int main(int argc, char** argv) {
 *        // Automatically runs the registered suite "AutomaticSuite".
 *        return scunit_main(argc, argv);
 *    }
 *
 *    In the latter case, an explicit call to `scunit_registerSuite()` from `<SCUnit/scunit.h>`
 *    is required if you want the suite to be run by `scunit_main()`. This is completely optional,
 *    an `SCUnitSuite` can be run at any point manually by calling `scunit_suite_run()` (more on
 *    that later). Caution: By calling `scunit_registerSuite()`, you transfer ownership of the suite
 *    to SCUnit, which is responsible for managing and deallocating it at a later point.
 *
 *    #include <SCUnit/scunit.h>
 *
 *    int main(int argc, char** argv) {
 *        SCUnitSuite* manualSuite;
 *        SCUnitError error = scunit_suite_new("ManualSuite", &manualSuite);
 *        if (error != SCUNIT_ERROR_NONE) {
 *            // Error handling...
 *        }
 *        error = scunit_registerSuite(manualSuite);
 *        if (error != SCUNIT_ERROR_NONE) {
 *            // Error handling...
 *        }
 *        // "ManualSuite" is run after manual registration as well.
 *        return scunit_main(argc, argv);
 *    }
 *
 * 2. Suites support two types of setup and teardown functions to be registered and run
 *    automatically. A suite setup or teardown (defined using the macros `SCUNIT_SUITE_BEFORE_ALL()`
 *    and `SCUNIT_SUITE_AFTER_ALL()`) is only run once before or after executing all tests in the
 *    suite. In contrast, a test setup or test teardown (defined using `SCUNIT_SUITE_BEFORE_EACH()`
 *    and `SCUNIT_SUITE_AFTER_EACH()`) is run before or after each individual test in the suite.
 *
 *    #include <SCUnit/scunit.h>
 *
 *    SCUNIT_SUITE(ExampleSuite);
 *
 *    SCUNIT_SUITE_BEFORE_ALL(ExampleSuite) {
 *        printf("Run once before all tests...\n");
 *    }
 *
 *    SCUNIT_SUITE_AFTER_ALL(ExampleSuite) {
 *        printf("Run once after all tests...\n");
 *    }
 *
 *    SCUNIT_SUITE_BEFORE_EACH(ExampleSuite) {
 *        printf("Run before each individual test...\n");
 *    }
 *
 *    SCUNIT_SUITE_AFTER_EACH(ExampleSuite) {
 *        printf("Run after each individual test...\n");
 *    }
 *
 *    int main(int argc, char** argv) {
 *        return scunit_main(argc, argv);
 *    }
 *
 *    The same can of course be achieved by using the normal function API. Suite and test setup
 *    and teardown functions have to defined with an appropriate function signature: They take no
 *    arguments as input and return nothing (i. e. the return type must be `void`) as indicated by
 *    the corresponding typedefs `SCUnitSetup`, `SCUnitTeardown`, `SCUnitTestSetup` and
 *    `SCUnitTestTeardown`. Registration is done by calling the appropriate function, e. g.
 *    `scunit_suite_registerSetup()` for setting a suite setup. Note that suites only support at
 *    most one function of each type for the sake of simplicity. Additional calls to the
 *    registration functions will replace the previous suite/test setup or teardown.
 *
 * 3. Suites may contain an arbitrary number of tests. Once again, the `SCUNIT_TEST()` macro
 *    automatically registers the test function in the appropriate `SCUnitSuite`, which in turn
 *    causes the test to be executed when the suite itself is run. This is achieved by passing two
 *    arguments to the macro: The first is the name of the suite the test is supposed to be part of,
 *    while the the second represents the test's name itself.
 *
 *    SCUNIT_SUITE(ExampleSuite);
 *
 *    SCUNIT_TEST(ExampleSuite, ExampleTest) {
 *        printf("Running the example test...\n");
 *    }
 *
 *    int main(int argc, char** argv) {
 *        return scunit_main(argc, argv);
 *    }
 *
 *    Tests can also be defined manually with the same function signature as the
 *    `SCUnitTestFunction` typedef. To register it, call `scunit_suite_registerTest()` with an
 *    `SCUnitSuite`, the name of the test and the function to be executed.
 *
 * 4. When all setup, teardown and test functions have been registered, an `SCUnitSuite` is ready to
 *    be run. If you used the automated macros or registered the suite using
 *    `scunit_registerSuite()`, all that is left for you to do is call `scunit_main()` and the
 *    suites will be run automatically. You can also run any `SCUnitSuite` at any point in time
 *    manually using `scunit_suite_run()`.
 *
 * 5. Finally, if you used the automated macros or registered the suite using
 *    `scunit_registerSuite()`, no additional cleanup is required, as SCUnit took ownership of the
 *    suite and is responsible for deallocating it. Note however that you must call `scunit_main()`
 *    in this case (which performs the deallocation internally), as a memory is leaked otherwise.
 *
 *    Any suites that were not created using the automated macros or registered using
 *    `scunit_registerSuite()` must be deallocated explicitly using a call to `scunit_suite_free()`.
 *
 * @note Suites with a large number of tests may be distributed across different files for
 * readability. Create one externally visible `SCUnitSuite` definition with `SCUNIT_PARTIAL_SUITE()`
 * and import it in other translation units using `SCUNIT_PARTIAL_SUITE_IMPORT()`.
 *
 * // File example1.c:
 *
 * #include <SCUnit/scunit.h>
 *
 * SCUNIT_PARTIAL_SUITE(ExampleSuite);
 *
 * SCUNIT_TEST(ExampleSuite, ExampleTestOne) {
 *     ...
 * }
 *
 * // File example2.c:
 *
 * #include <SCUnit/scunit.h>
 *
 * SCUNIT_PARTIAL_SUITE_IMPORT(ExampleSuite);
 *
 * SCUNIT_TEST(ExampleSuite, ExampleTestTwo) {
 *     ...
 * }
 *
 * @warning An `SCUnitSuite` is not thread-safe. Concurrent access to a single suite instance by
 * multiple threads may result in undefined behavior. It is your responsibility to ensure that any
 * access is properly synchronized when used in a multi-threaded context.
 *
 * @author Philipp Raschka
 */

#ifndef SCUNIT_SUITE_H
#define SCUNIT_SUITE_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <SCUnit/context.h>
#include <SCUnit/error.h>
#include <SCUnit/print.h>
#include <SCUnit/timer.h>

/**
 * @brief Represents a suite setup function to be run before executing all tests of an
 * `SCUnitSuite`.
 *
 * @note Register a suite setup function manually using `scunit_suite_registerSetup()`.
 * Each `SCUnitSuite` can only have one `SCUnitSetup` function.
 */
typedef void (*SCUnitSetup)();

/**
 * @brief Represents a suite teardown function to be run after executing all tests of an
 * `SCUnitSuite`.
 *
 * @note Register a suite teardown function manually using `scunit_suite_registerTeardown()`.
 * Each `SCUnitSuite` can only have one `SCUnitTeardown` function.
 */
typedef void (*SCUnitTeardown)();

/**
 * @brief Represents a test setup function to be run before each individual test of an
 * `SCUnitSuite`.
 *
 * @note Register a test setup function manually using `scunit_suite_registerTestSetup()`.
 * Each `SCUnitSuite` can only have one `SCUnitTestSetup` function.
 */
typedef void (*SCUnitTestSetup)();

/**
 * @brief Represents a test teardown function to be run after each individual test of an
 * `SCUnitSuite`.
 *
 * @note Register a test teardown function manually using `scunit_suite_setTestTeardown()`.
 * Each `SCUnitSuite` can only have one `SCUnitTestTeardown` function.
 */
typedef void (*SCUnitTestTeardown)();

/**
 * @brief Represents a test function to be registered and executed by an `SCUnitSuite`.
 *
 * @param[in, out] scunit_context `SCUnitContext` storing important information about the test.
 *                                See `<SCUnit/context.h>` for more information.
 */
typedef void (*SCUnitTestFunction)([[maybe_unused]] SCUnitContext* scunit_context);

/** @brief Represents a suite to group logically related tests together. */
typedef struct SCUnitSuite SCUnitSuite;

/**
 * @brief Represents a little summary produced while running an `SCUnitSuite` using
 * `scunit_suite_run()`.
 */
typedef struct SCUnitSummary {

    /** @brief Number of tests that passed while running an `SCUnitSuite`. */
    int64_t passedTests;

    /** @brief Number of tests that were skipped while running an `SCUnitSuite`. */
    int64_t skippedTests;

    /** @brief Number of tests that failed while running an `SCUnitSuite`. */
    int64_t failedTests;

} SCUnitSummary;

/**
 * @brief Initializes and automatically registers an `SCUnitSuite` with a given name.
 *
 * @note This macro is intended to be used at file scope and before any setup, teardown or test
 * functions are registered using the other macros below.
 *
 * An `SCUnitSuite` defined by this macro is private in the sense that it is only visible to this
 * translation unit (achieved by using the `static` modifier). If you want to distribute an
 * `SCUnitSuite` with a large number of tests across different files, use `SCUNIT_PARTIAL_SUITE()`
 * instead, which creates an externally visible definition.
 *
 * @attention If an unexpected error occurs while initializing or registering the suite, an error
 * message is written to `stderr` and the program exits using `EXIT_FAILURE`.
 *
 * In addition, this macro uses a compiler-specific attribute to implement the automatic
 * registration. See the introductory text in `<SCUnit/suite.h>` for more information.
 *
 * @param[in] name Name of the `SCUnitSuite` to define.
 */
#define SCUNIT_SUITE(name)                                                                  \
    static SCUnitSuite* scunit_suite##name;                                                 \
    [[gnu::constructor(101)]]                                                               \
    static void scunit_registerSuite##name() {                                              \
        SCUnitError error = scunit_suite_new(#name, &scunit_suite##name);                   \
        if (error != SCUNIT_ERROR_NONE) {                                                   \
            scunit_fprintfc(                                                                \
                stderr,                                                                     \
                SCUNIT_COLOR_DARK_RED,                                                      \
                SCUNIT_COLOR_DARK_DEFAULT,                                                  \
                "An unexpected error occurred while allocating the suite %s (code %d).\n",  \
                #name,                                                                      \
                error                                                                       \
            );                                                                              \
            exit(EXIT_FAILURE);                                                             \
        }                                                                                   \
        error = scunit_registerSuite(scunit_suite##name);                                   \
        if (error != SCUNIT_ERROR_NONE) {                                                   \
            scunit_fprintfc(                                                                \
                stderr,                                                                     \
                SCUNIT_COLOR_DARK_RED,                                                      \
                SCUNIT_COLOR_DARK_DEFAULT,                                                  \
                "An unexpected error occurred while registering the suite %s (code %d).\n", \
                #name,                                                                      \
                error                                                                       \
            );                                                                              \
            exit(EXIT_FAILURE);                                                             \
        }                                                                                   \
    }                                                                                       \
    [[maybe_unused]]                                                                        \
    static constexpr bool scunit_suite##name##Registered = true

/**
 * @brief Initializes and automatically registers an `SCUnitSuite` with a given name.
 *
 * @note This macro is intended to be used at file scope and before any setup, teardown or test
 * functions are registered using the other macros below.
 *
 * An `SCUnitSuite` defined by this macro is public in the sense that it is visible to other
 * translation units (achieved by not using the `static` modifier). You may use it to distribute an
 * `SCUnitSuite` with a large number of tests across two or more files. Only define it once using
 * this macro and import the `SCUnitSuite` in other files using `SCUNIT_PARTIAL_SUITE_IMPORT()`.
 *
 * @attention If an unexpected error occurs while initializing or registering the suite, an error
 * message is written to `stderr` and the program exits using `EXIT_FAILURE`.
 *
 * In addition, this macro uses a compiler-specific attribute to implement the automatic
 * registration. See the introductory text in `<SCUnit/suite.h>` for more information.
 *
 * @param[in] name Name of the `SCUnitSuite` to define.
 */
#define SCUNIT_PARTIAL_SUITE(name)                                                          \
    SCUnitSuite* scunit_suite##name;                                                        \
    [[gnu::constructor(101)]]                                                               \
    static void scunit_registerSuite##name() {                                              \
        SCUnitError error = scunit_suite_new(#name, &scunit_suite##name);                   \
        if (error != SCUNIT_ERROR_NONE) {                                                   \
            scunit_fprintfc(                                                                \
                stderr,                                                                     \
                SCUNIT_COLOR_DARK_RED,                                                      \
                SCUNIT_COLOR_DARK_DEFAULT,                                                  \
                "An unexpected error occurred while allocating the suite %s (code %d).\n",  \
                #name,                                                                      \
                error                                                                       \
            );                                                                              \
            exit(EXIT_FAILURE);                                                             \
        }                                                                                   \
        error = scunit_registerSuite(scunit_suite##name);                                   \
        if (error != SCUNIT_ERROR_NONE) {                                                   \
            scunit_fprintfc(                                                                \
                stderr,                                                                     \
                SCUNIT_COLOR_DARK_RED,                                                      \
                SCUNIT_COLOR_DARK_DEFAULT,                                                  \
                "An unexpected error occurred while registering the suite %s (code %d).\n", \
                #name,                                                                      \
                error                                                                       \
            );                                                                              \
            exit(EXIT_FAILURE);                                                             \
        }                                                                                   \
    }                                                                                       \
    [[maybe_unused]]                                                                        \
    static constexpr bool scunit_suite##name##Registered = true

/**
 * @brief Imports an externally visible `SCUnitSuite` defined by the `SCUNIT_PARTIAL_SUITE()` macro
 * in a different file.
 *
 * @note This macro is intended to be used at file scope and before any setup, teardown or test
 * functions are registered using the other macros below.
 *
 * Only use it to import an `SCUnitSuite` that is distributed across two or more files.
 *
 * @param[in] name Name of the `SCUnitSuite` to import.
 */
#define SCUNIT_PARTIAL_SUITE_IMPORT(name) extern SCUnitSuite* scunit_suite##name

/**
* @brief Defines and registers a suite setup function for an `SCUnitSuite` with a given name.
*
* @note This macro is intended to be used at file scope and after defining an `SCUnitSuite` with
* `SCUNIT_SUITE()` or `SCUNIT_PARTIAL_SUITE()`.
*
* Each `SCUnitSuite` can only have at most one suite setup function.
*
* @attention If an unexpected error occurs while defining or registering the suite setup function,
* an error message is written to `stderr` and the program exits using `EXIT_FAILURE`.
*
* @param[in] name Name of the `SCUnitSuite` to define and register the suite setup function for.
*/
#define SCUNIT_SUITE_BEFORE_ALL(name)                                                         \
    static void scunit_suite##name##Setup();                                                  \
    [[gnu::constructor(102)]]                                                                 \
    static void scunit_registerSuite##name##Setup() {                                         \
        SCUnitError error = scunit_suite_registerSetup(                                       \
            scunit_suite##name,                                                               \
            scunit_suite##name##Setup                                                         \
        );                                                                                    \
        if (error != SCUNIT_ERROR_NONE) {                                                     \
            scunit_fprintfc(                                                                  \
                stderr,                                                                       \
                SCUNIT_COLOR_DARK_RED,                                                        \
                SCUNIT_COLOR_DARK_DEFAULT,                                                    \
                "An unexpected error occurred while setting the suite %s setup (code %d).\n", \
                #name,                                                                        \
                error                                                                         \
            );                                                                                \
            exit(EXIT_FAILURE);                                                               \
        }                                                                                     \
    }                                                                                         \
    static void scunit_suite##name##Setup()

/**
 * @brief Defines and registers a suite teardown function for an `SCUnitSuite` with a given name.
 *
 * @note This macro is intended to be used at file scope and after defining an `SCUnitSuite` with
 * `SCUNIT_SUITE()` or `SCUNIT_PARTIAL_SUITE()`.
 *
 * Each `SCUnitSuite` can only have at most one suite teardown function.
 *
 * @attention If an unexpected error occurs while defining or registering the suite teardown
 * function, an error message is written to `stderr` and the program exits using `EXIT_FAILURE`.
 *
 * @param[in] name Name of the `SCUnitSuite` to define and register the suite teardown function for.
 */
#define SCUNIT_SUITE_AFTER_ALL(name)                                                             \
    static void scunit_suite##name##Teardown();                                                  \
    [[gnu::constructor(102)]]                                                                    \
    static void scunit_registerSuite##name##Teardown() {                                         \
        SCUnitError error = scunit_suite_registerTeardown(                                       \
            scunit_suite##name,                                                                  \
            scunit_suite##name##Teardown                                                         \
        );                                                                                       \
        if (error != SCUNIT_ERROR_NONE) {                                                        \
            scunit_fprintfc(                                                                     \
                stderr,                                                                          \
                SCUNIT_COLOR_DARK_RED,                                                           \
                SCUNIT_COLOR_DARK_DEFAULT,                                                       \
                "An unexpected error occurred while setting the suite %s teardown (code %d).\n", \
                #name,                                                                           \
                error                                                                            \
            );                                                                                   \
            exit(EXIT_FAILURE);                                                                  \
        }                                                                                        \
    }                                                                                            \
    static void scunit_suite##name##Teardown()

/**
 * @brief Defines and registers a test setup function for an `SCUnitSuite` with a given name.
 *
 * @note This macro is intended to be used at file scope and after defining an `SCUnitSuite` with
 * `SCUNIT_SUITE()` or `SCUNIT_PARTIAL_SUITE()`.
 *
 * Each `SCUnitSuite` can only have at most one test setup function.
 *
 * @attention If an unexpected error occurs while defining or registering the test setup function,
 * an error message is written to `stderr` and the program exits using `EXIT_FAILURE`.
 *
 * @param[in] name Name of the `SCUnitSuite` to define and register the test setup function for.
 */
#define SCUNIT_SUITE_BEFORE_EACH(name)                                                             \
    static void scunit_suite##name##TestSetup();                                                   \
    [[gnu::constructor(102)]]                                                                      \
    static void scunit_registerSuite##name##TestSetup() {                                          \
        SCUnitError error = scunit_suite_registerTestSetup(                                        \
            scunit_suite##name,                                                                    \
            scunit_suite##name##TestSetup                                                          \
        );                                                                                         \
        if (error != SCUNIT_ERROR_NONE) {                                                          \
            scunit_fprintfc(                                                                       \
                stderr,                                                                            \
                SCUNIT_COLOR_DARK_RED,                                                             \
                SCUNIT_COLOR_DARK_DEFAULT,                                                         \
                "An unexpected error occurred while setting the suite %s test setup (code %d).\n", \
                #name,                                                                             \
                error                                                                              \
            );                                                                                     \
            exit(EXIT_FAILURE);                                                                    \
        }                                                                                          \
    }                                                                                              \
    static void scunit_suite##name##TestSetup()

/**
 * @brief Defines and registers a test teardown function for an `SCUnitSuite` with a given name.
 *
 * @note This macro is intended to be used at file scope and after defining an `SCUnitSuite` with
 * `SCUNIT_SUITE()` or `SCUNIT_PARTIAL_SUITE()`.
 *
 * Each `SCUnitSuite` can only have at most one test teardown function.
 *
 * @attention If an unexpected error occurs while defining or registering the test teardown
 * function, an error message is written to `stderr` and the program exits using `EXIT_FAILURE`.
 *
 * @param[in] name Name of the `SCUnitSuite` to define and register the test teardown function for.
 */
#define SCUNIT_SUITE_AFTER_EACH(name)                                                    \
    static void scunit_suite##name##TestTeardown();                                      \
    [[gnu::constructor(102)]]                                                            \
    static void scunit_registerSuite##name##TestTeardown() {                             \
        SCUnitError error = scunit_suite_setTestTeardown(                                \
            scunit_suite##name,                                                          \
            scunit_suite##name##TestTeardown                                             \
        );                                                                               \
        if (error != SCUNIT_ERROR_NONE) {                                                \
            scunit_fprintfc(                                                             \
                stderr,                                                                  \
                SCUNIT_COLOR_DARK_RED,                                                   \
                SCUNIT_COLOR_DARK_DEFAULT,                                               \
                "An unexpected error occurred while setting the suite %s test teardown " \
                    "(code %d).\n",                                                      \
                #name,                                                                   \
                error                                                                    \
            );                                                                           \
            exit(EXIT_FAILURE);                                                          \
        }                                                                                \
    }                                                                                    \
    static void scunit_suite##name##TestTeardown()

/**
* @brief Defines and registers a test to be executed as part of an `SCUnitSuite` with a given name.
*
* @note This macro is intended to be used at file scope and after defining an `SCUnitSuite` with
* `SCUNIT_SUITE()` or `SCUNIT_PARTIAL_SUITE()`.
*
* Each `SCUnitSuite` supports an arbitrary number of tests.
*
* @attention If an unexpected error occurs while defining or registering the test, an error message
* is written to `stderr` and the program exits using `EXIT_FAILURE`.
*
* @param[in] suite Name of the `SCUnitSuite` to define and register the test for.
* @param[in] name  Name of the test itself.
*/
#define SCUNIT_TEST(suite, name)                                                                 \
    static void scunit_suite##suite##Test##name([[maybe_unused]] SCUnitContext* scunit_context); \
    [[gnu::constructor(103)]]                                                                    \
    static void scunit_registerSuite##suite##Test##name() {                                      \
        SCUnitError error = scunit_suite_registerTest(                                           \
            scunit_suite##suite,                                                                 \
            #name,                                                                               \
            scunit_suite##suite##Test##name                                                      \
        );                                                                                       \
        if (error != SCUNIT_ERROR_NONE) {                                                        \
            scunit_fprintfc(                                                                     \
                stderr,                                                                          \
                SCUNIT_COLOR_DARK_RED,                                                           \
                SCUNIT_COLOR_DARK_DEFAULT,                                                       \
                "An unexpected error occurred while registering the test %s (code %d).\n",       \
                #name,                                                                           \
                error                                                                            \
            );                                                                                   \
            exit(EXIT_FAILURE);                                                                  \
        }                                                                                        \
    }                                                                                            \
    static void scunit_suite##suite##Test##name([[maybe_unused]] SCUnitContext* scunit_context)

/**
 * @brief Initializes a new `SCUnitSuite`.
 *
 * @warning The `name` is copied internally for reasons of safety. If you pass a dynamically
 * allocated string, you are responsible for deallocating it yourself.
 *
 * @param[in]  name  A null-terminated string for the name of the `SCUnitSuite` to initialize.
 * @param[out] suite A new initialized `SCUnitSuite`.
 * @return `SCUNIT_ERROR_ARGUMENT_NULL` if `name` or `suite` is `nullptr`,
 * `SCUNIT_ERROR_OUT_OF_MEMORY` if an out-of-memory condition occurred and `SCUNIT_ERROR_NONE`
 * otherwise.
 */
SCUnitError scunit_suite_new(const char* name, SCUnitSuite** suite);

/**
 * @brief Gets the name of a given `SCUnitSuite`.
 *
 * @warning The name returned via the `name` parameter is a direct reference to the internal name of
 * the `SCUnitSuite`. It must not be modified nor deallocated manually.
 *
 * @param[in]  suite `SCUnitSuite` to get the name of.
 * @param[out] name  Name of the `SCUnitSuite`.
 * @return `SCUNIT_ERROR_ARGUMENT_NULL` if `suite` or `name` is `nullptr` and `SCUNIT_ERROR_NONE`
 * otherwise.
 */
SCUnitError scunit_suite_name(const SCUnitSuite* suite, const char** name);

/**
 * @brief Registers a suite setup function function for an `SCUnitSuite`.
 *
 * @attention Each `SCUnitSuite` can only have one `SCUnitSetup` function. Multiple calls to this
 * function will cause the current one to be replaced.
 *
 * @param[in, out] suite `SCUnitSuite` to register the `SCUnitSetup` for.
 * @param[in]      setup `SCUnitSetup` function to register. If equal to `nullptr`, causes the
 *                       current one to be deregistered.
 * @return `SCUNIT_ERROR_ARGUMENT_NULL` if `suite` is `nullptr` and `SCUNIT_ERROR_NONE` otherwise.
 */
SCUnitError scunit_suite_registerSetup(SCUnitSuite* suite, SCUnitSetup setup);

/**
 * @brief Registers a suite teardown function function for an `SCUnitSuite`.
 *
 * @attention Each `SCUnitSuite` can only have one `SCUnitTeardown` function. Multiple calls to this
 * function will cause the current one to be replaced.
 *
 * @param[in, out] suite    `SCUnitSuite` to register the `SCUnitTeardown` for.
 * @param[in]      teardown `SCUnitTeardown` function to register. If equal to `nullptr`, causes the
 *                          current one to be deregistered.
 * @return `SCUNIT_ERROR_ARGUMENT_NULL` if `suite` is `nullptr` and `SCUNIT_ERROR_NONE` otherwise.
 */
SCUnitError scunit_suite_registerTeardown(SCUnitSuite* suite, SCUnitTeardown teardown);

/**
 * @brief Registers a test setup function function for an `SCUnitSuite`.
 *
 * @attention Each `SCUnitSuite` can only have one `SCUnitTestSetup` function. Multiple calls to
 * this function will cause the current one to be replaced.
 *
 * @param[in, out] suite     `SCUnitSuite` to register the `SCUnitTestSetup` for.
 * @param[in]      testSetup `SCUnitTestSetup` function to register. If equal to `nullptr`,
 *                           causes the current one to be deregistered.
 * @return `SCUNIT_ERROR_ARGUMENT_NULL` if `suite` is `nullptr` and `SCUNIT_ERROR_NONE` otherwise.
 */
SCUnitError scunit_suite_registerTestSetup(SCUnitSuite* suite, SCUnitTestSetup testSetup);

/**
 * @brief Registers a test teardown function for an `SCUnitSuite`.
 *
 * @attention Each `SCUnitSuite` can only have one `SCUnitTestTeardown` function. Multiple calls to
 * this function will cause the current one to be replaced.
 *
 * @param[in, out] suite     `SCUnitSuite` to register the `SCUnitTestTeardown` for.
 * @param[in]      testSetup `SCUnitTestTeardown` function to register. If equal to `nullptr`,
 *                           causes the current one to be deregistered.
 * @return `SCUNIT_ERROR_ARGUMENT_NULL` if `suite` is `nullptr` and `SCUNIT_ERROR_NONE` otherwise.
 */
SCUnitError scunit_suite_setTestTeardown(SCUnitSuite* suite, SCUnitTestTeardown testTeardown);

/**
 * @brief Registers a test function to be executed as part of an `SCUnitSuite`.
 *
 * @note Each `SCUnitSuite` supports an arbitrary number of tests.
 *
 * @warning The `name` is copied internally for reasons of safety. If you pass a dynamically
 * allocated string, you are responsible for deallocating it yourself.
 *
 * @param[in, out] suite        `SCUnitSuite` to register the `SCUnitTestFunction` for.
 * @param[in]      name         A null-terminated string for the name of the test to register.
 * @param[in]      testFunction `SCUnitTestFunction` to register.
 * @return `SCUNIT_ERROR_ARGUMENT_NULL` if `suite`, `name` or `testFunction` is `nullptr`,
 * `SCUNIT_ERROR_OUT_OF_MEMORY` if an out-of-memory condition occurred and `SCUNIT_ERROR_NONE`
 * otherwise.
 */
SCUnitError scunit_suite_registerTest(
    SCUnitSuite* suite,
    const char* name,
    SCUnitTestFunction testFunction
);

/**
 * @brief Runs a given `SCUnitSuite`.
 *
 * @note This function produces a lot of useful diagnostic output on `stdout` and `stderr`, such as
 * names of suites and tests, results, time measurements, detailed error messages whenever an
 * assertion fails and a little summary at the end.
 *
 * It respects the current `SCUnitColoredOutput` state set by calling `scunit_setColoredOutput()`.
 * If currently set to `SCUNIT_COLORED_OUTPUT_DISABLED`, the default terminal color is used for the
 * diagnostic output.
 *
 * @param[in]  suite   `SCUnitSuite` to run.
 * @param[out] summary A little `SCUnitSummary` produced as the result of the run.
 * @return `SCUNIT_ARGUMENT_NULL` if `suite` or `summary` is `nullptr`,
 * `SCUNIT_ERROR_OUT_OF_MEMORY` if an out-of-memory condition occurred,
 * `SCUNIT_ERROR_OPENING_STREAM_FAILED`, `SCUNIT_ERROR_READING_STREAM_FAILED`,
 * `SCUNIT_ERROR_WRITING_STREAM_FAILED` or `SCUNIT_ERROR_CLOSING_STREAM_FAILED` if opening, reading,
 * writing or closing a stream failed, `SCUNIT_ERROR_WRITING_BUFFER_FAILED` if writing to a buffer
 * failed, `SCUNIT_ERROR_TIMER_FAILED` if a `SCUnitTimer` failed, `SCUNIT_ERROR_UNKNOWN_RESULT` if
 * an unknown test result is encountered (a sign of a serious programming error) and
 * `SCUNIT_ERROR_NONE` otherwise.
 */
SCUnitError scunit_suite_run(const SCUnitSuite* suite, SCUnitSummary* summary);

/**
 * @brief Deallocates any remaining resources of a given `SCUnitSuite`.
 *
 * @note It is allowed to deallocate a non-existing `SCUnitSuite`, i. e. `*suite` may be `nullptr`,
 * but `suite` itself is not allowed to be `nullptr`.
 *
 * @warning Suites created using the `SCUNIT_SUITE()` or `SCUNIT_PARTIAL_SUITE()` macros or
 * registered using `scunit_registerSuite()` from `<SCUnit/scunit.h>` are owned by SCUnit itself.
 * You must not deallocate these suites manually.
 *
 * @param[in, out] suite `SCUnitSuite` to deallocate the resources of.
 * @return `SCUNIT_ERROR_ARGUMENT_NULL` if `suite` is `nullptr` and `SCUNIT_ERROR_NONE` otherwise.
 */
SCUnitError scunit_suite_free(SCUnitSuite** suite);

#endif