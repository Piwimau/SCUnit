/**
 * @file assert.h
 *
 * @brief This header defines a set of useful assertions to be used in tests.
 *
 * @details All assertions are implemented as macros, which is necessary because they modify the
 * special `SCUnitContext` parameter (see `<SCUnit/context.h>` for more information) only available
 * within test functions. An additional benefit is that information such as the file name
 * (`__FILE__`) or line number (`__LINE__`) can be included into an error message directly to
 * indicate where an assertion failed.
 *
 * SCUnit provides several useful assertion macros, which can be grouped according to their intended
 * usage or the types they operate on:
 *
 * - `SCUNIT_PASS()`, `SCUNIT_SKIP()` and `SCUNIT_FAIL()` terminate the current test with the
 *   corresponding `SCUnitResult`, which may be useful for debugging purposes or to temporarily
 *   circumvent subsequent assertions.
 *
 * - `SCUNIT_ASSERT()` is a generic assertion macro similar to the standard C `assert()` from
 *   `<assert.h>`. It supports arbitrary conditions, as long as the expression if of a scalar type.
 *
 * - `SCUNIT_ASSERT_TRUE()` and `SCUNIT_ASSERT_FALSE()` should be used instead of `SCUNIT_ASSERT()`
 *   for readability whenever an expression conceptually evaluates to a boolean value.
 *
 * - `SCUNIT_ASSERT_NULL()` and `SCUNIT_ASSERT_NOT_NULL()` are intended to be used with arbitrary
 *   pointer types and instead of the basic `SCUNIT_ASSERT()` for reasons of readability.
 *
 * - `SCUNIT_ASSERT_EQUAL()`, `SCUNIT_ASSERT_NOT_EQUAL()`, `SCUNIT_ASSERT_LESS()` and so on allow
 *   for testing relational expressions with arbitrary scalar types.
 *
 * These are of course not all, see below for the full set of assertions.
 *
 * @note All assertion macros print only the assertion that failed, which does not replace variables
 * and expressions with their actual values. This is done to avoid having to define specialized
 * macros for a potentially infinite number of types and format specifiers that could be used in.
 *
 * There is, however, a solution: All macros accept a variadic list of arguments, which can be used
 * to print a custom message after a test completes. The first argument must be a null-terminated
 * format string following the same conventions as the standard `printf` family of functions,
 * followed by any number of additional arguments to be written according to the format string.
 * Here is a simple example:
 *
 * SCUNIT_TEST(ExampleSuite, ExampleTest) {
 *     int actual = ...;
 *     SCUNIT_ASSERT_EQUAL(actual, 42, "Expected %d == %d.", actual, 42);
 * }
 *
 * This allows you to provide a detailed context message with the correct format specifiers for the
 * types used in the assertion macro.
 *
 * @attention The `SCUNIT_PASS()`, `SCUNIT_SKIP()` and `SCUNIT_FAIL()` macros, as well as all
 * assertion macros immediately terminate the current test if an assertion fails. This is an
 * intentional design choice and should be used to verify important conditions first (e. g. a
 * pointer being not `nullptr`, an index not out of range, ...) and avoid undefined behavior.
 *
 * @author Philipp Raschka
 */

#ifndef SCUNIT_ASSERT_H
#define SCUNIT_ASSERT_H

#include <stdlib.h>
#include <SCUnit/context.h>
#include <SCUnit/error.h>

/**
 * @brief Terminates the current test with a given `SCUnitResult`.
 *
 * @note This macro is intended for internal use only. It factors out the common implementation of
 * `SCUNIT_PASS()`, `SCUNIT_SKIP()` and `SCUNIT_FAIL()`. Use these macros directly instead.
 *
 * @attention If an unexpected error occurs while terminating the current test, an error message
 * is written to `stderr` and the program exits using `EXIT_FAILURE`.
 *
 * @param[in] result `SCUnitResult` to terminate the current test with.
 * @param[in] ...    An optional null-terminated format string following the same conventions as the
 *                   standard `printf` family of functions, followed by any number of additional
 *                   arguments to be written according to the format string.
 */
#define SCUNIT_TEST_TERMINATE(result, ...)                                                        \
    do {                                                                                          \
        SCUnitError scunit_error;                                                                 \
        __VA_OPT__(                                                                               \
            scunit_error = scunit_context_appendMessage(scunit_context, "\n  " __VA_ARGS__);      \
            if (scunit_error != SCUNIT_ERROR_NONE) {                                              \
                scunit_fprintfc(                                                                  \
                    stderr,                                                                       \
                    SCUNIT_COLOR_DARK_RED,                                                        \
                    SCUNIT_COLOR_DARK_DEFAULT,                                                    \
                    "An unexpected error occurred while appending a message to the test context " \
                        "(code %d).\n",                                                           \
                    scunit_error                                                                  \
                );                                                                                \
                exit(EXIT_FAILURE);                                                               \
            }                                                                                     \
            scunit_error = scunit_context_appendMessage(scunit_context, "\n\n");                  \
            if (scunit_error != SCUNIT_ERROR_NONE) {                                              \
                scunit_fprintfc(                                                                  \
                    stderr,                                                                       \
                SCUNIT_COLOR_DARK_RED,                                                            \
                SCUNIT_COLOR_DARK_DEFAULT,                                                        \
                    "An unexpected error occurred while appending a message to the test context " \
                        "(code %d).\n",                                                           \
                    scunit_error                                                                  \
                );                                                                                \
                exit(EXIT_FAILURE);                                                               \
            }                                                                                     \
        )                                                                                         \
        scunit_error = scunit_context_setResult(scunit_context, result);                          \
        if (scunit_error != SCUNIT_ERROR_NONE) {                                                  \
            scunit_fprintfc(                                                                      \
                stderr,                                                                           \
                SCUNIT_COLOR_DARK_RED,                                                            \
                SCUNIT_COLOR_DARK_DEFAULT,                                                        \
                "An unexpected error occurred while setting the result for the test context "     \
                    "(code %d).\n",                                                               \
                scunit_error                                                                      \
            );                                                                                    \
            exit(EXIT_FAILURE);                                                                   \
        }                                                                                         \
        return;                                                                                   \
    }                                                                                             \
    while (false)

/**
 * @brief Causes the current test to pass.
 *
 * @attention If an unexpected error occurs while terminating the current test, an error message
 * is written to `stderr` and the program exits using `EXIT_FAILURE`.
 *
 * @param[in] ... An optional null-terminated format string following the same conventions as the
 *                standard `printf` family of functions, followed by any number of additional
 *                arguments to be written according to the format string.
 */
#define SCUNIT_PASS(...) SCUNIT_TEST_TERMINATE(SCUNIT_RESULT_PASS __VA_OPT__(, __VA_ARGS__))

/**
 * @brief Causes the current test to be skipped.
 *
 * @attention If an unexpected error occurs while terminating the current test, an error message
 * is written to `stderr` and the program exits using `EXIT_FAILURE`.
 *
 * @param[in] ... An optional null-terminated format string following the same conventions as the
 *                standard `printf` family of functions, followed by any number of additional
 *                arguments to be written according to the format string.
 */
#define SCUNIT_SKIP(...) SCUNIT_TEST_TERMINATE(SCUNIT_RESULT_SKIP __VA_OPT__(, __VA_ARGS__))

/**
 * @brief Causes the current test to fail immediately.
 *
 * @attention If an unexpected error occurs while terminating the current test, an error message
 * is written to `stderr` and the program exits using `EXIT_FAILURE`.
 *
 * @param[in] ... An optional null-terminated format string following the same conventions as the
 *                standard `printf` family of functions, followed by any number of additional
 *                arguments to be written according to the format string.
 */
#define SCUNIT_FAIL(...) SCUNIT_TEST_TERMINATE(SCUNIT_RESULT_FAIL __VA_OPT__(, __VA_ARGS__))

/**
 * @brief Asserts that an arbitrary condition holds. If the assertion fails, writes an error message
 * to `stderr` and terminates the current test with `SCUNIT_RESULT_FAIL`.
 *
 * @note The expression of `condition` must be of a scalar type.
 *
 * @attention If an unexpected error occurs while terminating the current test, an error message
 * is written to `stderr` and the program exits using `EXIT_FAILURE`.
 *
 * @param[in] condition Condition to examine.
 * @param[in] ...       An optional null-terminated format string following the same conventions as
 *                      the standard `printf` family of functions, followed by any number of
 *                      additional arguments to be written according to the format string.
 */
#define SCUNIT_ASSERT(condition, ...)                                                             \
    do {                                                                                          \
        if (!(condition)) {                                                                       \
            SCUnitError scunit_error = scunit_context_appendMessage(                              \
                scunit_context,                                                                   \
                "\n  Assertion failed in %s:%d:\n\n",                                             \
                __FILE__,                                                                         \
                __LINE__                                                                          \
            );                                                                                    \
            if (scunit_error != SCUNIT_ERROR_NONE) {                                              \
                scunit_fprintfc(                                                                  \
                    stderr,                                                                       \
                    SCUNIT_COLOR_DARK_RED,                                                        \
                    SCUNIT_COLOR_DARK_DEFAULT,                                                    \
                    "An unexpected error occurred while appending a message to the test context " \
                        "(code %d).\n",                                                           \
                    scunit_error                                                                  \
                );                                                                                \
                exit(EXIT_FAILURE);                                                               \
            }                                                                                     \
            scunit_error = scunit_context_appendFileContext(scunit_context, __FILE__, __LINE__);  \
            if (scunit_error != SCUNIT_ERROR_NONE) {                                              \
                scunit_fprintfc(                                                                  \
                    stderr,                                                                       \
                    SCUNIT_COLOR_DARK_RED,                                                        \
                    SCUNIT_COLOR_DARK_DEFAULT,                                                    \
                    "An unexpected error occurred while appending the file context to the test "  \
                        "context (code %d).\n",                                                   \
                    scunit_error                                                                  \
                );                                                                                \
                exit(EXIT_FAILURE);                                                               \
            }                                                                                     \
            scunit_error = scunit_context_appendMessage(scunit_context, "\n");                    \
            if (scunit_error != SCUNIT_ERROR_NONE) {                                              \
                scunit_fprintfc(                                                                  \
                    stderr,                                                                       \
                    SCUNIT_COLOR_DARK_RED,                                                        \
                    SCUNIT_COLOR_DARK_DEFAULT,                                                    \
                    "An unexpected error occurred while appending a message to the test context " \
                        "(code %d).\n",                                                           \
                    scunit_error                                                                  \
                );                                                                                \
                exit(EXIT_FAILURE);                                                               \
            }                                                                                     \
            __VA_OPT__(                                                                           \
                scunit_error = scunit_context_appendMessage(scunit_context, "  " __VA_ARGS__);    \
                if (scunit_error != SCUNIT_ERROR_NONE) {                                          \
                    scunit_fprintfc(                                                              \
                        stderr,                                                                   \
                    SCUNIT_COLOR_DARK_RED,                                                        \
                    SCUNIT_COLOR_DARK_DEFAULT,                                                    \
                        "An unexpected error occurred while appending a message to the test "     \
                            "context (code %d).\n",                                               \
                        scunit_error                                                              \
                    );                                                                            \
                    exit(EXIT_FAILURE);                                                           \
                }                                                                                 \
                scunit_error = scunit_context_appendMessage(scunit_context, "\n\n");              \
                if (scunit_error != SCUNIT_ERROR_NONE) {                                          \
                    scunit_fprintfc(                                                              \
                        stderr,                                                                   \
                    SCUNIT_COLOR_DARK_RED,                                                        \
                    SCUNIT_COLOR_DARK_DEFAULT,                                                    \
                        "An unexpected error occurred while appending a message to the test "     \
                            "context (code %d).\n",                                               \
                        scunit_error                                                              \
                    );                                                                            \
                    exit(EXIT_FAILURE);                                                           \
                }                                                                                 \
            )                                                                                     \
            scunit_error = scunit_context_setResult(scunit_context, SCUNIT_RESULT_FAIL);          \
            if (scunit_error != SCUNIT_ERROR_NONE) {                                              \
                scunit_fprintfc(                                                                  \
                    stderr,                                                                       \
                    SCUNIT_COLOR_DARK_RED,                                                        \
                    SCUNIT_COLOR_DARK_DEFAULT,                                                    \
                    "An unexpected error occurred while setting the result for the test context " \
                        "(code %d).\n",                                                           \
                    scunit_error                                                                  \
                );                                                                                \
                exit(EXIT_FAILURE);                                                               \
            }                                                                                     \
            return;                                                                               \
        }                                                                                         \
    }                                                                                             \
    while (false)

/**
 * @brief Asserts that a condition evaluates to `true`. If the assertion fails, writes an error
 * message to `stderr` and terminates the current test with `SCUNIT_RESULT_FAIL`.
 *
 * @attention If an unexpected error occurs while terminating the current test, an error message
 * is written to `stderr` and the program exits using `EXIT_FAILURE`.
 *
 * @param[in] condition Condition to examine.
 * @param[in] ...       An optional null-terminated format string following the same conventions as
 *                      the standard `printf` family of functions, followed by any number of
 *                      additional arguments to be written according to the format string.
 */
#define SCUNIT_ASSERT_TRUE(condition, ...) \
    SCUNIT_ASSERT((condition) __VA_OPT__(, __VA_ARGS__))

/**
 * @brief Asserts that a condition evaluates to `false`. If the assertion fails, writes an error
 * message to `stderr` and terminates the current test with `SCUNIT_RESULT_FAIL`.
 *
 * @attention If an unexpected error occurs while terminating the current test, an error message
 * is written to `stderr` and the program exits using `EXIT_FAILURE`.
 *
 * @param[in] condition Condition to examine.
 * @param[in] ...       An optional null-terminated format string following the same conventions as
 *                      the standard `printf` family of functions, followed by any number of
 *                      additional arguments to be written according to the format string.
 */
#define SCUNIT_ASSERT_FALSE(condition, ...) \
    SCUNIT_ASSERT(!(condition) __VA_OPT__(, __VA_ARGS__))

/**
 * @brief Asserts that a pointer is `nullptr`. If the assertion fails, writes an error message to
 * `stderr` and terminates the current test with `SCUNIT_RESULT_FAIL`.
 *
 * @attention If an unexpected error occurs while terminating the current test, an error message
 * is written to `stderr` and the program exits using `EXIT_FAILURE`.
 *
 * @param[in] pointer Pointer to examine.
 * @param[in] ...     An optional null-terminated format string following the same conventions as
 *                    the standard `printf` family of functions, followed by any number of
 *                    additional arguments to be written according to the format string.
 */
#define SCUNIT_ASSERT_NULL(pointer, ...) \
    SCUNIT_ASSERT((pointer) == nullptr __VA_OPT__(, __VA_ARGS__))

/**
 * @brief Asserts that a pointer is not `nullptr`. If the assertion fails, writes an error message
 * to `stderr` and terminates the current test with `SCUNIT_RESULT_FAIL`.
 *
 * @attention If an unexpected error occurs while terminating the current test, an error message
 * is written to `stderr` and the program exits using `EXIT_FAILURE`.
 *
 * @param[in] pointer Pointer to examine.
 * @param[in] ...     An optional null-terminated format string following the same conventions as
 *                    the standard `printf` family of functions, followed by any number of
 *                    additional arguments to be written according to the format string.
 */
#define SCUNIT_ASSERT_NOT_NULL(pointer, ...) \
    SCUNIT_ASSERT((pointer) != nullptr __VA_OPT__(, __VA_ARGS__))

/**
 * @brief Asserts that an actual value is equal to an expected one. If the assertion fails,
 * writes an error message to `stderr` and terminates the current test with `SCUNIT_RESULT_FAIL`.
 *
 * @attention If an unexpected error occurs while terminating the current test, an error message
 * is written to `stderr` and the program exits using `EXIT_FAILURE`.
 *
 * @param[in] actual   Actual value for the comparison.
 * @param[in] expected Expected value for the comparison.
 * @param[in] ...      An optional null-terminated format string following the same conventions as
 *                     the standard `printf` family of functions, followed by any number of
 *                     additional arguments to be written according to the format string.
 */
#define SCUNIT_ASSERT_EQUAL(actual, expected, ...) \
    SCUNIT_ASSERT((actual) == (expected) __VA_OPT__(, __VA_ARGS__))

/**
 * @brief Asserts that an actual value is not equal to an expected one. If the assertion fails,
 * writes an error message to `stderr` and terminates the current test with `SCUNIT_RESULT_FAIL`.
 *
 * @attention If an unexpected error occurs while terminating the current test, an error message
 * is written to `stderr` and the program exits using `EXIT_FAILURE`.
 *
 * @param[in] actual   Actual value for the comparison.
 * @param[in] expected Expected value for the comparison.
 * @param[in] ...      An optional null-terminated format string following the same conventions as
 *                     the standard `printf` family of functions, followed by any number of
 *                     additional arguments to be written according to the format string.
 */
#define SCUNIT_ASSERT_NOT_EQUAL(actual, expected, ...) \
    SCUNIT_ASSERT((actual) != (expected) __VA_OPT__(, __VA_ARGS__))

/**
 * @brief Asserts that an actual value is less than an expected one. If the assertion fails,
 * writes an error message to `stderr` and terminates the current test with `SCUNIT_RESULT_FAIL`.
 *
 * @attention If an unexpected error occurs while terminating the current test, an error message
 * is written to `stderr` and the program exits using `EXIT_FAILURE`.
 *
 * @param[in] actual   Actual value for the comparison.
 * @param[in] expected Expected value for the comparison.
 * @param[in] ...      An optional null-terminated format string following the same conventions as
 *                     the standard `printf` family of functions, followed by any number of
 *                     additional arguments to be written according to the format string.
 */
#define SCUNIT_ASSERT_LESS(actual, expected, ...) \
    SCUNIT_ASSERT((actual) < (expected) __VA_OPT__(, __VA_ARGS__))

/**
 * @brief Asserts that an actual value is less than or equal to an expected one. If the assertion
 * fails, writes an error message to `stderr` and terminates the current test with
 * `SCUNIT_RESULT_FAIL`.
 *
 * @attention If an unexpected error occurs while terminating the current test, an error message
 * is written to `stderr` and the program exits using `EXIT_FAILURE`.
 *
 * @param[in] actual   Actual value for the comparison.
 * @param[in] expected Expected value for the comparison.
 * @param[in] ...      An optional null-terminated format string following the same conventions as
 *                     the standard `printf` family of functions, followed by any number of
 *                     additional arguments to be written according to the format string.
 */
#define SCUNIT_ASSERT_LESS_OR_EQUAL(actual, expected, ...) \
    SCUNIT_ASSERT((actual) <= (expected) __VA_OPT__(, __VA_ARGS__))

/**
 * @brief Asserts that an actual value is greater than an expected one. If the assertion fails,
 * writes an error message to `stderr` and terminates the current test with `SCUNIT_RESULT_FAIL`.
 *
 * @attention If an unexpected error occurs while terminating the current test, an error message
 * is written to `stderr` and the program exits using `EXIT_FAILURE`.
 *
 * @param[in] actual   Actual value for the comparison.
 * @param[in] expected Expected value for the comparison.
 * @param[in] ...      An optional null-terminated format string following the same conventions as
 *                     the standard `printf` family of functions, followed by any number of
 *                     additional arguments to be written according to the format string.
 */
#define SCUNIT_ASSERT_GREATER(actual, expected, ...) \
    SCUNIT_ASSERT((actual) > (expected) __VA_OPT__(, __VA_ARGS__))

/**
 * @brief Asserts that an actual value is greater than or equal to an expected one. If the assertion
 * fails, writes an error message to `stderr` and terminates the current test with
 * `SCUNIT_RESULT_FAIL`.
 *
 * @attention If an unexpected error occurs while terminating the current test, an error message
 * is written to `stderr` and the program exits using `EXIT_FAILURE`.
 *
 * @param[in] actual   Actual value for the comparison.
 * @param[in] expected Expected value for the comparison.
 * @param[in] ...      An optional null-terminated format string following the same conventions as
 *                     the standard `printf` family of functions, followed by any number of
 *                     additional arguments to be written according to the format string.
 */
#define SCUNIT_ASSERT_GREATER_OR_EQUAL(actual, expected, ...) \
    SCUNIT_ASSERT((actual) >= (expected) __VA_OPT__(, __VA_ARGS__))

/**
 * @brief Returns the absolute difference between two values.
 *
 * @note This macro is intended for internal purposes only.
 *
 * @param[in] a First value for the computing the absolute difference.
 * @param[in] b Second value for computing the absolute difference.
 * @return The absolute difference between the two values.
 */
#define SCUNIT_ABS_DIFF(a, b) (((a) > (b)) ? (a) - (b) : (b) - (a))

/**
 * @brief Asserts that an actual and an expected value are within a given delta. If the assertion
 * fails, writes an error message to `stderr` and terminates the current test with
 * `SCUNIT_RESULT_FAIL`.
 *
 * @attention If an unexpected error occurs while terminating the current test, an error message
 * is written to `stderr` and the program exits using `EXIT_FAILURE`.
 *
 * @param[in] actual   Actual value for the comparison.
 * @param[in] expected Expected value for the comparison.
 * @param[in] delta    Delta the two values are expected to be within.
 * @param[in] ...      An optional null-terminated format string following the same conventions as
 *                     the standard `printf` family of functions, followed by any number of
 *                     additional arguments to be written according to the format string.
 */
#define SCUNIT_ASSERT_NEAR(actual, expected, delta, ...) \
    SCUNIT_ASSERT((SCUNIT_ABS_DIFF((actual), (expected)) <= (delta)) __VA_OPT__(, __VA_ARGS__))

/**
 * @brief Asserts that an actual and an expected value are not within a given delta. If the
 * assertion fails, writes an error message to `stderr` and terminates the current test with
 * `SCUNIT_RESULT_FAIL`.
 *
 * @attention If an unexpected error occurs while terminating the current test, an error message
 * is written to `stderr` and the program exits using `EXIT_FAILURE`.
 *
 * @param[in] actual   Actual value for the comparison.
 * @param[in] expected Expected value for the comparison.
 * @param[in] delta    Delta the two values are expected to be within.
 * @param[in] ...      An optional null-terminated format string following the same conventions as
 *                     the standard `printf` family of functions, followed by any number of
 *                     additional arguments to be written according to the format string.
 */
#define SCUNIT_ASSERT_NOT_NEAR(actual, expected, delta, ...) \
    SCUNIT_ASSERT(SCUNIT_ABS_DIFF((actual), (expected)) > (delta) __VA_OPT__(, __VA_ARGS__))

/**
 * @brief Asserts that an actual value is within a range (inclusive). If the assertion fails,
 * writes an error message to `stderr` and terminates the current test with `SCUNIT_RESULT_FAIL`.
 *
 * @attention If an unexpected error occurs while terminating the current test, an error message
 * is written to `stderr` and the program exits using `EXIT_FAILURE`.
 *
 * @param[in] actual Actual value to examine.
 * @param[in] lower  Lower bound of the range (inclusive).
 * @param[in] upper  Upper bound of the range (inclusive).
 * @param[in] ...    An optional null-terminated format string following the same conventions as the
 *                   standard `printf` family of functions, followed by any number of additional
 *                   arguments to be written according to the format string.
 */
#define SCUNIT_ASSERT_IN_RANGE(actual, lower, upper, ...) \
    SCUNIT_ASSERT(((value) >= (lower)) && ((value) <= (upper)) __VA_OPT__(, __VA_ARGS__))

/**
 * @brief Asserts that an actual value is not within a range (inclusive). If the assertion fails,
 * writes an error message to `stderr` and terminates the current test with `SCUNIT_RESULT_FAIL`.
 *
 * @attention If an unexpected error occurs while terminating the current test, an error message
 * is written to `stderr` and the program exits using `EXIT_FAILURE`.
 *
 * @param[in] actual Actual value to examine.
 * @param[in] lower  Lower bound of the range (inclusive).
 * @param[in] upper  Upper bound of the range (inclusive).
 * @param[in] ...    An optional null-terminated format string following the same conventions as the
 *                   standard `printf` family of functions, followed by any number of additional
 *                   arguments to be written according to the format string.
 */
#define SCUNIT_ASSERT_NOT_IN_RANGE(actual, lower, upper, ...) \
    SCUNIT_ASSERT(((value) < (lower)) || ((value) > (upper)) __VA_OPT__(, __VA_ARGS__))

#endif