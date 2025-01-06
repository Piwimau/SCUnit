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
        scunit_context_setResult(scunit_context, result, &scunit_error);                          \
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
            scunit_context_appendFileContext(scunit_context, __FILE__, __LINE__, &scunit_error);  \
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
            scunit_context_setResult(scunit_context, SCUNIT_RESULT_FAIL, &scunit_error);          \
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