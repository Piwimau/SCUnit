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
 * @note Register a suite setup function manually using `scunit_suite_registerSuiteSetup()`.
 * Each `SCUnitSuite` can only have one `SCUnitSuiteSetup` function.
 */
typedef void (*SCUnitSuiteSetup)();

/**
 * @brief Represents a suite teardown function to be run after executing all tests of an
 * `SCUnitSuite`.
 *
 * @note Register a suite teardown function manually using `scunit_suite_registerSuiteTeardown()`.
 * Each `SCUnitSuite` can only have one `SCUnitSuiteTeardown` function.
 */
typedef void (*SCUnitSuiteTeardown)();

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
 * @note Register a test teardown function manually using `scunit_suite_registerTestTeardown()`.
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
#define SCUNIT_SUITE_BEFORE_ALL(name)                                                             \
    static void scunit_suite##name##SuiteSetup();                                                 \
    [[gnu::constructor(102)]]                                                                     \
    static void scunit_registerSuite##name##SuiteSetup() {                                        \
        SCUnitError error = scunit_suite_registerSuiteSetup(                                      \
            scunit_suite##name,                                                                   \
            scunit_suite##name##SuiteSetup                                                        \
        );                                                                                        \
        if (error != SCUNIT_ERROR_NONE) {                                                         \
            scunit_fprintfc(                                                                      \
                stderr,                                                                           \
                SCUNIT_COLOR_DARK_RED,                                                            \
                SCUNIT_COLOR_DARK_DEFAULT,                                                        \
                "An unexpected error occurred while registering the suite %s setup (code %d).\n", \
                #name,                                                                            \
                error                                                                             \
            );                                                                                    \
            exit(EXIT_FAILURE);                                                                   \
        }                                                                                         \
    }                                                                                             \
    static void scunit_suite##name##SuiteSetup()

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
#define SCUNIT_SUITE_AFTER_ALL(name)                                                    \
    static void scunit_suite##name##SuiteTeardown();                                    \
    [[gnu::constructor(102)]]                                                           \
    static void scunit_registerSuite##name##SuiteTeardown() {                           \
        SCUnitError error = scunit_suite_registerSuiteTeardown(                         \
            scunit_suite##name,                                                         \
            scunit_suite##name##SuiteTeardown                                           \
        );                                                                              \
        if (error != SCUNIT_ERROR_NONE) {                                               \
            scunit_fprintfc(                                                            \
                stderr,                                                                 \
                SCUNIT_COLOR_DARK_RED,                                                  \
                SCUNIT_COLOR_DARK_DEFAULT,                                              \
                "An unexpected error occurred while registering the suite %s teardown " \
                    "(code %d).\n",                                                     \
                #name,                                                                  \
                error                                                                   \
            );                                                                          \
            exit(EXIT_FAILURE);                                                         \
        }                                                                               \
    }                                                                                   \
    static void scunit_suite##name##SuiteTeardown()

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
#define SCUNIT_SUITE_BEFORE_EACH(name)                                                    \
    static void scunit_suite##name##TestSetup();                                          \
    [[gnu::constructor(102)]]                                                             \
    static void scunit_registerSuite##name##TestSetup() {                                 \
        SCUnitError error = scunit_suite_registerTestSetup(                               \
            scunit_suite##name,                                                           \
            scunit_suite##name##TestSetup                                                 \
        );                                                                                \
        if (error != SCUNIT_ERROR_NONE) {                                                 \
            scunit_fprintfc(                                                              \
                stderr,                                                                   \
                SCUNIT_COLOR_DARK_RED,                                                    \
                SCUNIT_COLOR_DARK_DEFAULT,                                                \
                "An unexpected error occurred while registering the suite %s test setup " \
                    "(code %d).\n",                                                       \
                #name,                                                                    \
                error                                                                     \
            );                                                                            \
            exit(EXIT_FAILURE);                                                           \
        }                                                                                 \
    }                                                                                     \
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
#define SCUNIT_SUITE_AFTER_EACH(name)                                                        \
    static void scunit_suite##name##TestTeardown();                                          \
    [[gnu::constructor(102)]]                                                                \
    static void scunit_registerSuite##name##TestTeardown() {                                 \
        SCUnitError error = scunit_suite_registerTestTeardown(                                    \
            scunit_suite##name,                                                              \
            scunit_suite##name##TestTeardown                                                 \
        );                                                                                   \
        if (error != SCUNIT_ERROR_NONE) {                                                    \
            scunit_fprintfc(                                                                 \
                stderr,                                                                      \
                SCUNIT_COLOR_DARK_RED,                                                       \
                SCUNIT_COLOR_DARK_DEFAULT,                                                   \
                "An unexpected error occurred while registering the suite %s test teardown " \
                    "(code %d).\n",                                                          \
                #name,                                                                       \
                error                                                                        \
            );                                                                               \
            exit(EXIT_FAILURE);                                                              \
        }                                                                                    \
    }                                                                                        \
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
SCUnitError scunit_suite_getName(const SCUnitSuite* suite, const char** name);

/**
 * @brief Registers a suite setup function function for an `SCUnitSuite`.
 *
 * @attention Each `SCUnitSuite` can only have one `SCUnitSuiteSetup` function. Multiple calls to
 * this function will cause the current one to be replaced.
 *
 * @param[in, out] suite `SCUnitSuite` to register the `SCUnitSuiteSetup` for.
 * @param[in]      setup `SCUnitSuiteSetup` function to register. If equal to `nullptr`, causes the
 *                       current one to be deregistered.
 * @return `SCUNIT_ERROR_ARGUMENT_NULL` if `suite` is `nullptr` and `SCUNIT_ERROR_NONE` otherwise.
 */
SCUnitError scunit_suite_registerSuiteSetup(SCUnitSuite* suite, SCUnitSuiteSetup suiteSetup);

/**
 * @brief Registers a suite teardown function function for an `SCUnitSuite`.
 *
 * @attention Each `SCUnitSuite` can only have one `SCUnitSuiteTeardown` function. Multiple calls to
 * this function will cause the current one to be replaced.
 *
 * @param[in, out] suite    `SCUnitSuite` to register the `SCUnitSuiteTeardown` for.
 * @param[in]      teardown `SCUnitSuiteTeardown` function to register. If equal to `nullptr`,
 *                          causes the current one to be deregistered.
 * @return `SCUNIT_ERROR_ARGUMENT_NULL` if `suite` is `nullptr` and `SCUNIT_ERROR_NONE` otherwise.
 */
SCUnitError scunit_suite_registerSuiteTeardown(
    SCUnitSuite* suite,
    SCUnitSuiteTeardown suiteTeardown
);

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
SCUnitError scunit_suite_registerTestTeardown(SCUnitSuite* suite, SCUnitTestTeardown testTeardown);

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