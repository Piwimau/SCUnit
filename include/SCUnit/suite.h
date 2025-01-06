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
 * @brief Represents a suite setup function that is called before all tests of an `SCUnitSuite`
 * are executed.
 */
typedef void (*SCUnitSuiteSetup)();

/**
 * @brief Represents a suite teardown function that is called after all tests of an `SCUnitSuite`
 * were executed.
 */
typedef void (*SCUnitSuiteTeardown)();

/**
 * @brief Represents a test setup function that is called before each test of an `SCUnitSuite`
 * is executed.
 */
typedef void (*SCUnitTestSetup)();

/**
 * @brief Represents a test teardown function that is called after each test of an `SCUnitSuite`
 * was executed.
 */
typedef void (*SCUnitTestTeardown)();

/**
 * @brief Represents a test function to be registered and executed by an `SCUnitSuite`.
 *
 * @param[in, out] scunit_context `SCUnitContext` storing important information about the test.
 *                                See `<SCUnit/context.h>` for more information.
 */
typedef void (*SCUnitTestFunction)([[maybe_unused]] SCUnitContext* scunit_context);

/** @brief Represents a suite for grouping logically related tests together. */
typedef struct SCUnitSuite SCUnitSuite;

/** @brief Represents a summary produced while running an `SCUnitSuite`. */
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
 * @param[in] name Name of the `SCUnitSuite` to define.
 */
#define SCUNIT_SUITE(name)                                                                  \
    static SCUnitSuite* scunit_suite##name;                                                 \
    [[gnu::constructor(101)]]                                                               \
    static void scunit_registerSuite##name() {                                              \
        SCUnitError error;                                                                  \
        scunit_suite##name = scunit_suite_new(#name, &error);                               \
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
        scunit_registerSuite(scunit_suite##name, &error);                                   \
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
 * @param[in] name Name of the `SCUnitSuite` to define.
 */
#define SCUNIT_PARTIAL_SUITE(name)                                                          \
    SCUnitSuite* scunit_suite##name;                                                        \
    [[gnu::constructor(101)]]                                                               \
    static void scunit_registerSuite##name() {                                              \
        SCUnitError error;                                                                  \
        scunit_suite##name = scunit_suite_new(#name, &error);                               \
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
        scunit_registerSuite(scunit_suite##name, &error);                                   \
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
* @brief Defines and sets a suite setup function for an `SCUnitSuite` with a given name.
*
* @note This macro is intended to be used at file scope and after defining an `SCUnitSuite` with
* `SCUNIT_SUITE()` or `SCUNIT_PARTIAL_SUITE()`.
*
* Each `SCUnitSuite` can only have at most one suite setup function.
*
* @param[in] name Name of the `SCUnitSuite` to define and set the suite setup function for.
*/
#define SCUNIT_SUITE_BEFORE_ALL(name)                                                   \
    static void scunit_suite##name##SuiteSetup();                                       \
    [[gnu::constructor(102)]]                                                           \
    static void scunit_setSuite##name##SuiteSetup() {                                   \
        scunit_suite_setSuiteSetup(scunit_suite##name, scunit_suite##name##SuiteSetup); \
    }                                                                                   \
    static void scunit_suite##name##SuiteSetup()

/**
 * @brief Defines and sets a suite teardown function for an `SCUnitSuite` with a given name.
 *
 * @note This macro is intended to be used at file scope and after defining an `SCUnitSuite` with
 * `SCUNIT_SUITE()` or `SCUNIT_PARTIAL_SUITE()`.
 *
 * Each `SCUnitSuite` can only have at most one suite teardown function.
 *
 * @param[in] name Name of the `SCUnitSuite` to define and set the suite teardown function for.
 */
#define SCUNIT_SUITE_AFTER_ALL(name)                                                          \
    static void scunit_suite##name##SuiteTeardown();                                          \
    [[gnu::constructor(102)]]                                                                 \
    static void scunit_setSuite##name##SuiteTeardown() {                                      \
        scunit_suite_setSuiteTeardown(scunit_suite##name, scunit_suite##name##SuiteTeardown); \
    }                                                                                         \
    static void scunit_suite##name##SuiteTeardown()

/**
 * @brief Defines and sets a test setup function for an `SCUnitSuite` with a given name.
 *
 * @note This macro is intended to be used at file scope and after defining an `SCUnitSuite` with
 * `SCUNIT_SUITE()` or `SCUNIT_PARTIAL_SUITE()`.
 *
 * Each `SCUnitSuite` can only have at most one test setup function.
 *
 * @param[in] name Name of the `SCUnitSuite` to define and set the test setup function for.
 */
#define SCUNIT_SUITE_BEFORE_EACH(name)                                                \
    static void scunit_suite##name##TestSetup();                                      \
    [[gnu::constructor(102)]]                                                         \
    static void scunit_setSuite##name##TestSetup() {                                  \
        scunit_suite_setTestSetup(scunit_suite##name, scunit_suite##name##TestSetup); \
    }                                                                                 \
    static void scunit_suite##name##TestSetup()

/**
 * @brief Defines and sets a test teardown function for an `SCUnitSuite` with a given name.
 *
 * @note This macro is intended to be used at file scope and after defining an `SCUnitSuite` with
 * `SCUNIT_SUITE()` or `SCUNIT_PARTIAL_SUITE()`.
 *
 * Each `SCUnitSuite` can only have at most one test teardown function.
 *
 * @param[in] name Name of the `SCUnitSuite` to define and set the test teardown function for.
 */
#define SCUNIT_SUITE_AFTER_EACH(name)                                                       \
    static void scunit_suite##name##TestTeardown();                                         \
    [[gnu::constructor(102)]]                                                               \
    static void scunit_setSuite##name##TestTeardown() {                                     \
        scunit_suite_setTestTeardown(scunit_suite##name, scunit_suite##name##TestTeardown); \
    }                                                                                       \
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
        SCUnitError error;                                                                       \
        scunit_suite_registerTest(                                                               \
            scunit_suite##suite,                                                                 \
            #name,                                                                               \
            scunit_suite##suite##Test##name,                                                     \
            &error                                                                               \
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
 * @brief Initializes a new `SCUnitSuite` with a given name.
 *
 * @warning The `name` is copied internally for reasons of safety. If you pass a dynamically
 * allocated string, you are responsible for deallocating it yourself.
 *
 * @param[in]  name  A null-terminated string for the name of the `SCUnitSuite`.
 * @param[out] error `SCUNIT_ERROR_OUT_OF_MEMORY` if an out-of-memory condition occurred,
 *                   otherwise `SCUNIT_ERROR_NONE`.
 * @return A pointer to a new initialized `SCUnitSuite` on success, otherwise a `nullptr`.
 */
SCUnitSuite* scunit_suite_new(const char* name, SCUnitError* error);

/**
 * @brief Gets the name of a given `SCUnitSuite`.
 *
 * @warning The returned name is a direct reference to the internal name of the `SCUnitSuite`.
 * It must not be modified nor deallocated manually.
 *
 * @param[in] suite `SCUnitSuite` to get the name of.
 * @return The name of the given `SCUnitSuite`.
 */
const char* scunit_suite_getName(const SCUnitSuite* suite);

/**
 * @brief Sets a suite setup function for a given `SCUnitSuite`.
 *
 * @note Each `SCUnitSuite` can only have one `SCUnitSuiteSetup` function. Multiple calls to this
 * function will cause the current one to be replaced.
 *
 * @param[in, out] suite      `SCUnitSuite` to set the `SCUnitSuiteSetup` of.
 * @param[in]      suiteSetup `SCUnitSuiteSetup` to set. If equal to `nullptr`, causes the current
 *                            one to be unset.
 */
void scunit_suite_setSuiteSetup(SCUnitSuite* suite, SCUnitSuiteSetup suiteSetup);

/**
 * @brief Sets a suite teardown function for a given `SCUnitSuite`.
 *
 * @note Each `SCUnitSuite` can only have one `SCUnitSuiteTeardown` function. Multiple calls to this
 * function will cause the current one to be replaced.
 *
 * @param[in, out] suite         `SCUnitSuite` to set the `SCUnitSuiteTeardown` of.
 * @param[in]      suiteTeardown `SCUnitSuiteTeardown` to set. If equal to `nullptr`, causes the
 *                               current one to be unset.
 */
void scunit_suite_setSuiteTeardown(SCUnitSuite* suite, SCUnitSuiteTeardown suiteTeardown);

/**
 * @brief Sets a test setup function for a given `SCUnitSuite`.
 *
 * @note Each `SCUnitSuite` can only have one `SCUnitTestSetup` function. Multiple calls to this
 * function will cause the current one to be replaced.
 *
 * @param[in, out] suite     `SCUnitSuite` to set the `SCUnitTestSetup` of.
 * @param[in]      testSetup `SCUnitTestSetup` to set. If equal to `nullptr`, causes the current one
 *                           to be unset.
 */
void scunit_suite_setTestSetup(SCUnitSuite* suite, SCUnitTestSetup testSetup);

/**
 * @brief Sets a test teardown function for a given `SCUnitSuite`.
 *
 * @note Each `SCUnitSuite` can only have one `SCUnitTestTeardown` function. Multiple calls to this
 * function will cause the current one to be replaced.
 *
 * @param[in, out] suite        `SCUnitSuite` to set the `SCUnitTestTeardown` of.
 * @param[in]      testTeardown `SCUnitTestTeardown` to set. If equal to `nullptr`, causes the
 *                              current one to be unset.
 */
void scunit_suite_setTestTeardown(SCUnitSuite* suite, SCUnitTestTeardown testTeardown);

/**
 * @brief Registers a test function to be executed as part of a given `SCUnitSuite`.
 *
 * @note Each `SCUnitSuite` supports an arbitrary number of tests.
 *
 * @warning The `name` is copied internally for reasons of safety. If you pass a dynamically
 * allocated string, you are responsible for deallocating it yourself.
 *
 * @param[in, out] suite        `SCUnitSuite` to register the `SCUnitTestFunction` for.
 * @param[in]      name         A null-terminated string for the name of the test.
 * @param[in]      testFunction `SCUnitTestFunction` to register.
 * @param[out]     error        `SCUNIT_ERROR_OUT_OF_MEMORY` if an out-of-memory condition occurred,
 *                              otherwise `SCUNIT_ERROR_NONE`.
 */
void scunit_suite_registerTest(
    SCUnitSuite* suite,
    const char* name,
    SCUnitTestFunction testFunction,
    SCUnitError* error
);

/**
 * @brief Runs a given `SCUnitSuite`.
 *
 * @note This function produces a lot of useful diagnostic output on `stdout` and `stderr`, such as
 * names of suites and tests, results, time measurements, detailed error messages whenever an
 * assertion fails and a summary at the end.
 *
 * This function respects the current `SCUnitColoredOutput` state set by calling
 * `scunit_setColoredOutput()`. If currently set to `SCUNIT_COLORED_OUTPUT_DISABLED`, the default
 * terminal color is used for the diagnostic output.
 *
 * @param[in]  suite   `SCUnitSuite` to run.
 * @param[out] summary An `SCUnitSummary` produced as the result.
 * @param[out] error   `SCUNIT_ERROR_OUT_OF_MEMORY` if an out-of-memory condition occurred,
 *                     `SCUNIT_ERROR_OPENING_STREAM_FAILED`, `SCUNIT_ERROR_READING_STREAM_FAILED`,
 *                     `SCUNIT_ERROR_WRITING_STREAM_FAILED` or `SCUNIT_ERROR_CLOSING_STREAM_FAILED`
 *                     if opening, reading from, writing to or closing a stream failed,
 *                     `SCUNIT_ERROR_WRITING_BUFFER_FAILED` if writing to a buffer failed,
 *                     `SCUNIT_ERROR_TIMER_FAILED` if an `SCUnitTimer` failed,
 *                     `SCUNIT_ERROR_UNKNOWN_RESULT` if an unknown test result is encountered (a
 *                     sign of a serious programming error) and `SCUNIT_ERROR_NONE` otherwise.
 */
void scunit_suite_run(const SCUnitSuite* suite, SCUnitSummary* summary, SCUnitError* error);

/**
 * @brief Deallocates a given `SCUnitSuite`.
 *
 * @note For convenience, `suite` is allowed to be `nullptr`.
 *
 * @warning Suites created using the `SCUNIT_SUITE()` or `SCUNIT_PARTIAL_SUITE()` macros or
 * registered using `scunit_registerSuite()` from `<SCUnit/scunit.h>` are owned by SCUnit itself.
 * You must not deallocate these suites manually.
 *
 * Any use of the `SCUnitSuite` after it has been deallocated results in undefined behavior.
 *
 * @param[in, out] suite `SCUnitSuite` to deallocate.
 */
void scunit_suite_free(SCUnitSuite* suite);

#endif