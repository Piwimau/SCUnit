/**
 * @file context.h
 *
 * @brief A context available in tests that stores important information, such as the test's
 * `SCUnitResult` and optionally a message to be printed after the test has finished.
 *
 * @note Under normal circumstances, you should not even care about the existence of this context,
 * as SCUnit takes care of everything when using the `SCUnitSuite` from `<SCUnit/suite.h>`
 * and the assertion macros from `<SCUnit/assert.h>`.
 *
 * However, if you want to do something more sophisticated or implement a custom behavior,
 * you may use the `SCUnitContext` to your advantage. Every test for SCUnit has the following
 * function signature (as seen in `<SCUnit/suite.h>`):
 *
 * void exampleTest([[maybe_unused]] SCUnitContext* scunit_context);
 *
 * As such, the `SCUnitContext` is available in each test through the special parameter
 * `scunit_context` (named like this to avoid conflicts with your local variables). Take a look at
 * the interface below to see what can be done with an `SCUnitContext`.
 *
 * @attention The allocation and deallocation of an `SCUnitContext` is handled automatically by the
 * `SCUnitSuite` containing the relevant test. You should therefore not use the corresponding
 * functions `scunit_context_new()` and `scunit_context_free()`, as these are for internal use only.
 *
 * @warning An `SCUnitContext` is not thread-safe. This should not be a problem under normal
 * circumstances, as the context is local to the relevant test (unless you hand out a reference).
 * Concurrent access to a single `SCUnitContext` instance by multiple threads may result in
 * undefined behavior. It is your responsibility to ensure that any access is properly synchronized
 * when used in a multi-threaded context.
 *
 * @author Philipp Raschka
 */

#ifndef SCUNIT_CONTEXT_H
#define SCUNIT_CONTEXT_H

#include <stdint.h>
#include <SCUnit/error.h>
#include <SCUnit/print.h>

/** @brief Represents a context available in tests that stores important information. */
typedef struct SCUnitContext SCUnitContext;

/** @brief Represents an enumeration of the different results a test may have. */
typedef enum SCUnitResult {

    /** @brief Indicates that a test passed (automatically or because it was forced to). */
    SCUNIT_RESULT_PASS,

    /** @brief Indicates that a test was forced to be skipped. */
    SCUNIT_RESULT_SKIP,

    /** @brief Indicates that a test failed (automatically or because it was forced to). */
    SCUNIT_RESULT_FAIL

} SCUnitResult;

/**
 * @brief Initializes a new `SCUnitContext`.
 *
 * @attention Allocation of an `SCUnitContext` is handled automatically by the `SCUnitSuite`
 * containing the relevant test. You should therefore not use this function, it is intended for
 * internal use only.
 *
 * @param[out] context A new initialized `SCUnitContext`.
 * @return `SCUNIT_ERROR_ARGUMENT_NULL` if `context` is `nullptr`, `SCUNIT_ERROR_OUT_OF_MEMORY` if
 * an out-of-memory condition occurred and `SCUNIT_ERROR_NONE` otherwise.
 */
SCUnitError scunit_context_new(SCUnitContext** context);

/**
 * @brief Resets a given `SCUnitContext`.
 *
 * @details This resets the test's result to `SCUNIT_RESULT_PASS` (tests are initially assumed to
 * pass) and clears the message to printed after the test has finished.
 *
 * @attention This function is used by an `SCUnitSuite` to reuse contexts and therefore avoid
 * allocating a completely new one for every test. It is generally intended for internal use.
 *
 * @param[in, out] context `SCUnitContext` to reset.
 * @return `SCUNIT_ERROR_ARGUMENT_NULL` if `context` is `nullptr` and `SCUNIT_ERROR_NONE` otherwise.
 */
SCUnitError scunit_context_reset(SCUnitContext* context);

/**
 * @brief Gets the `SCUnitResult` of a test's `SCUnitContext`.
 *
 * @param[in]  context `SCUnitContext` to examine.
 * @param[out] result  `SCUnitResult` of the test's `SCUnitContext`.
 * @return `SCUNIT_ERROR_ARGUMENT_NULL` if `context` or `result` is `nullptr` and
 * `SCUNIT_ERROR_NONE` otherwise.
 */
SCUnitError scunit_context_getResult(const SCUnitContext* context, SCUnitResult* result);

/**
 * @brief Sets the `SCUnitResult` of a test's `SCUnitContext`.
 *
 * @param[in, out] context `SCUnitContext` to modify.
 * @param[in]      result  New `SCUnitResult` of the test's `SCUnitContext`.
 * @return `SCUNIT_ERROR_ARGUMENT_NULL` if `context` is `nullptr`,
 * `SCUNIT_ERROR_ARGUMENT_OUT_OF_RANGE` if `result` is not a valid `SCUnitResult` and
 * `SCUNIT_ERROR_NONE` otherwise.
 */
SCUnitError scunit_context_setResult(SCUnitContext* context, SCUnitResult result);

/**
 * @brief Gets the message of a test's `SCUnitContext`.
 *
 * @warning The message returned via the `message` parameter is a direct reference to the internal
 * message of the `SCUnitContext`. It must not be modified nor deallocated manually.
 *
 * @param[in]  context `SCUnitContext` to examine.
 * @param[out] message  Message of the test's `SCUnitContext`.
 * @return `SCUNIT_ERROR_ARGUMENT_NULL` if `context` or `message` is `nullptr` and
 * `SCUNIT_ERROR_NONE` otherwise.
 */
SCUnitError scunit_context_getMessage(const SCUnitContext* context, const char** message);

/**
 * @brief Overwrites the message of a test's `SCUnitContext` with a given formatted string.
 *
 * @param[in, out] context `SCUnitContext` to modify.
 * @param[in]      format  A null-terminated format string following the same conventions as the
 *                         standard `printf` family of functions.
 * @param[in]      ...     Any number of additional arguments to be formatted and written based on
 *                         the given format string.
 * @return `SCUNIT_ERROR_ARGUMENT_NULL` if `context` or `format` is `nullptr`,
 * `SCUNIT_ERROR_OUT_OF_MEMORY` if an out-of-memory condition occurred,
 * `SCUNIT_ERROR_WRITING_BUFFER_FAILED` if overwriting the message failed and `SCUNIT_ERROR_NONE`
 * otherwise.
 */
SCUnitError scunit_context_setMessage(SCUnitContext* context, const char* format, ...);

/**
 * @brief Overwrites the message of a test's `SCUnitContext` with a given formatted and colored
 * string.
 *
 * @note This function respects the current `SCUnitColoredOutput` state set by calling
 * `scunit_setColoredOutput()`. If currently set to `SCUNIT_COLORED_OUTPUT_DISABLED`,
 * `foreground` and `background` are ignored and the default terminal color is used instead.
 * See `<SCUnit/print.h>` for more information.
 *
 * @param[in, out] context    `SCUnitContext` to modify.
 * @param[in]      foreground A `SCUnitColor` to use as the foreground color.
 * @param[in]      background A `SCUnitColor` to use as the background color.
 * @param[in]      format     A null-terminated format string following the same conventions as the
 *                            standard `printf` family of functions.
 * @param[in]      ...        Any number of additional arguments to be formatted and written based
 *                            on the given format string.
 * @return `SCUNIT_ERROR_ARGUMENT_NULL` if `context` or `format` is `nullptr`,
 * `SCUNIT_ERROR_ARGUMENT_OUT_OF_RANGE` if `foreground` or `background` is not a valid
 * `SCUnitColor`, `SCUNIT_ERROR_OUT_OF_MEMORY` if an out-of-memory condition occurred,
 * `SCUNIT_ERROR_WRITING_BUFFER_FAILED` if overwriting the message failed and `SCUNIT_ERROR_NONE`
 * otherwise.
 */
SCUnitError scunit_context_setColoredMessage(
    SCUnitContext* context,
    SCUnitColor foreground,
    SCUnitColor background,
    const char* format,
    ...
);

/**
 * @brief Appends a given formatted string to the message of a test's `SCUnitContext`.
 *
 * @param[in, out] context `SCUnitContext` to modify.
 * @param[in]      format  A null-terminated format string following the same conventions as the
 *                         standard `printf` family of functions.
 * @param[in]      ...     Any number of additional arguments to be formatted and written based on
 *                         the given format string.
 * @return `SCUNIT_ERROR_ARGUMENT_NULL` if `context` or `format` is `nullptr`,
 * `SCUNIT_ERROR_OUT_OF_MEMORY` if an out-of-memory condition occurred,
 * `SCUNIT_ERROR_WRITING_BUFFER_FAILED` if appending to the message failed and `SCUNIT_ERROR_NONE`
 * otherwise.
 */
SCUnitError scunit_context_appendMessage(SCUnitContext* context, const char* format, ...);

/**
 * @brief Appends a given formatted and colored string to the message of a test's `SCUnitContext`.
 *
 * @note This function respects the current `SCUnitColoredOutput` state set by calling
 * `scunit_setColoredOutput()`. If currently set to `SCUNIT_COLORED_OUTPUT_DISABLED`,
 * `foreground` and `background` are ignored and the default terminal color is used instead.
 * See `<SCUnit/print.h>` for more information.
 *
 * @param[in, out] context    `SCUnitContext` to modify.
 * @param[in]      foreground A `SCUnitColor` to use as the foreground color.
 * @param[in]      background A `SCUnitColor` to use as the background color.
 * @param[in]      format     A null-terminated format string following the same conventions as the
 *                            standard `printf` family of functions.
 * @param[in]      ...        Any number of additional arguments to be formatted and written based
 *                            on the given format string.
 * @return `SCUNIT_ERROR_ARGUMENT_NULL` if `context` or `format` is `nullptr`,
 * `SCUNIT_ERROR_ARGUMENT_OUT_OF_RANGE` if `foreground` or `background` is not a valid
 * `SCUnitColor`, `SCUNIT_ERROR_OUT_OF_MEMORY` if an out-of-memory condition occurred,
 * `SCUNIT_ERROR_WRITING_BUFFER_FAILED` if appending to the message failed and `SCUNIT_ERROR_NONE`
 * otherwise.
 */
SCUnitError scunit_context_appendColoredMessage(
    SCUnitContext* context,
    SCUnitColor foreground,
    SCUnitColor background,
    const char* format,
    ...
);

/**
 * @brief Appends the file context around a relevant line to the message of a test's
 * `SCUnitContext`.
 *
 * @note This function respects the current `SCUnitColoredOutput` state set by calling
 * `scunit_setColoredOutput()`. If currently set to `SCUNIT_COLORED_OUTPUT_DISABLED`,
 * `foreground` and `background` are ignored and the default terminal color is used instead.
 * See `<SCUnit/print.h>` for more information.
 *
 * @param[in, out] context  `SCUnitContext` to modify.
 * @param[in]      filename Name of the file to read the context from.
 * @param[in]      line     Relevant line around which the file context should be read.
 * @return `SCUNIT_ERROR_ARGUMENT_NULL` if `context` or `filename` is `nullptr`,
 * `SCUNIT_ERROR_ARGUMENT_OUT_OF_RANGE` if `line` is less than one (as file lines are one-based),
 * `SCUNIT_ERROR_OPENING_STREAM_FAILED` if opening the file with the name `filename` failed,
 * `SCUNIT_ERROR_READING_STREAM_FAILED` if reading the file with the name `filename` failed,
 * `SCUNIT_ERROR_CLOSING_STREAM_FAILED` if closing the file with the name `filename` failed,
 * `SCUNIT_ERROR_OUT_OF_MEMORY` if an out-of-memory condition occurred,
 * `SCUNIT_ERROR_WRITING_BUFFER_FAILED` if appending the file context failed and `SCUNIT_ERROR_NONE`
 * otherwise.
 */
SCUnitError scunit_context_appendFileContext(
    SCUnitContext* context,
    const char* filename,
    int64_t line
);

/**
 * @brief Deallocates any remaining resources of a given `SCUnitContext`.
 *
 * @note It is allowed to deallocate a non-existing `SCUnitContext`, i. e. `*context` may be
 * `nullptr`, but `context` itself is not allowed to be `nullptr`.
 *
 * @attention Deallocation of an `SCUnitContext` is handled automatically by the `SCUnitSuite`
 * containing the relevant test. You should therefore not use this function, it is intended for
 * internal use only.
 *
 * @param[in, out] context `SCUnitContext` to deallocate the resources of.
 * @return `SCUNIT_ERROR_ARGUMENT_NULL` if `context` is `nullptr` and `SCUNIT_ERROR_NONE` otherwise.
 */
SCUnitError scunit_context_free(SCUnitContext** context);

#endif