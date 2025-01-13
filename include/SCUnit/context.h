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
 * @brief Allocates and initializes a new `SCUnitContext`.
 *
 * @note Allocation of an `SCUnitContext` is handled automatically by the `SCUnitSuite` containing
 * the relevant test. You should not use this function, it is intended for internal use only.
 *
 * @param[out] error `SCUNIT_ERROR_OUT_OF_MEMORY` if an out-of-memory condition occurred,
 *                   otherwise `SCUNIT_ERROR_NONE`.
 * @return A pointer to a new initialized `SCUnitContext` on success, otherwise a `nullptr`.
 */
SCUnitContext* scunit_context_new(SCUnitError* error);

/**
 * @brief Resets a given `SCUnitContext`.
 *
 * @note This function is used by an `SCUnitSuite` to reuse contexts and avoid allocating a
 * completely new one for every single test. It is generally intended for internal use.
 *
 * @param[in, out] context `SCUnitContext` to reset.
 */
void scunit_context_reset(SCUnitContext* context);

/**
 * @brief Gets the `SCUnitResult` of a given `SCUnitContext`.
 *
 * @param[in] context `SCUnitContext` to get the `SCUnitResult` of.
 * @return The `SCUnitResult` of the given `SCUnitContext`.
 */
SCUnitResult scunit_context_getResult(const SCUnitContext* context);

/**
 * @brief Sets the `SCUnitResult` of a given `SCUnitContext`.
 *
 * @param[in, out] context `SCUnitContext` to set the `SCUnitResult` of.
 * @param[in]      result  `SCUnitResult` to set.
 * @param[out]     error   `SCUNIT_ERROR_ARGUMENT_OUT_OF_RANGE` if `result` is not a valid
 *                         `SCUnitResult`, otherwise `SCUNIT_ERROR_NONE`.
 */
void scunit_context_setResult(SCUnitContext* context, SCUnitResult result, SCUnitError* error);

/**
 * @brief Gets the message of a given `SCUnitContext`.
 *
 * @warning The message returned via the `message` parameter is a direct reference to the internal
 * message of the `SCUnitContext`. It must not be modified nor deallocated manually.
 *
 * @param[in] context `SCUnitContext` to get the message of.
 * @return The message of the given `SCUnitContext`.
 */
const char* scunit_context_getMessage(const SCUnitContext* context);

/**
 * @brief Overwrites the message of a given `SCUnitContext` with a formatted string.
 *
 * @param[in, out] context `SCUnitContext` to overwrite the message of.
 * @param[in]      format  A null-terminated format string following the same conventions as the
 *                         standard `printf` family of functions.
 * @param[in]      ...     Any number of additional arguments to be formatted and written based on
 *                         the given format string.
 * @return `SCUNIT_ERROR_OUT_OF_MEMORY` if an out-of-memory condition occurred,
 * `SCUNIT_ERROR_WRITING_BUFFER_FAILED` if overwriting the message failed and `SCUNIT_ERROR_NONE`
 * otherwise.
 */
SCUnitError scunit_context_setMessage(SCUnitContext* context, const char* format, ...);

/**
 * @brief Overwrites the message of a given `SCUnitContext` with a formatted and colored string.
 *
 * @note This function respects the current colored output state set by calling
 * `scunit_setColoredOutput()`. If currently set to `SCUNIT_COLORED_OUTPUT_NEVER`, `foreground` and
 * `background` are ignored and the default color is used instead. See `<SCUnit/scunit.h>` for more
 * information.
 *
 * @param[in, out] context    `SCUnitContext` to overwrite the message of.
 * @param[in]      foreground An `SCUnitColor` to use as the foreground color.
 * @param[in]      background An `SCUnitColor` to use as the background color.
 * @param[in]      format     A null-terminated format string following the same conventions as the
 *                            standard `printf` family of functions.
 * @param[in]      ...        Any number of additional arguments to be formatted and written based
 *                            on the given format string.
 * @return `SCUNIT_ERROR_ARGUMENT_OUT_OF_RANGE` if `foreground` or `background` is not a valid
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
 * @brief Appends a formatted string to the message of a given `SCUnitContext`.
 *
 * @param[in, out] context `SCUnitContext` to append to.
 * @param[in]      format  A null-terminated format string following the same conventions as the
 *                         standard `printf` family of functions.
 * @param[in]      ...     Any number of additional arguments to be formatted and written based on
 *                         the given format string.
 * @return `SCUNIT_ERROR_OUT_OF_MEMORY` if an out-of-memory condition occurred,
 * `SCUNIT_ERROR_WRITING_BUFFER_FAILED` if appending to the message failed and `SCUNIT_ERROR_NONE`
 * otherwise.
 */
SCUnitError scunit_context_appendMessage(SCUnitContext* context, const char* format, ...);

/**
 * @brief Appends a formatted and colored string to the message of a given `SCUnitContext`.
 *
 * @note This function respects the current colored output state set by calling
 * `scunit_setColoredOutput()`. If currently set to `SCUNIT_COLORED_OUTPUT_NEVER`, `foreground` and
 * `background` are ignored and the default color is used instead. See `<SCUnit/scunit.h>` for more
 * information.
 *
 * @param[in, out] context    `SCUnitContext` to append to.
 * @param[in]      foreground An `SCUnitColor` to use as the foreground color.
 * @param[in]      background An `SCUnitColor` to use as the background color.
 * @param[in]      format     A null-terminated format string following the same conventions as the
 *                            standard `printf` family of functions.
 * @param[in]      ...        Any number of additional arguments to be formatted and written based
 *                            on the given format string.
 * @return `SCUNIT_ERROR_ARGUMENT_OUT_OF_RANGE` if `foreground` or `background` is not a valid
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
 * @brief Appends the file context around a line to the message of a given `SCUnitContext`.
 *
 * @note This function respects the current colored output state set by calling
 * `scunit_setColoredOutput()`. If currently set to `SCUNIT_COLORED_OUTPUT_NEVER`, the default color
 * is used instead. See `<SCUnit/scunit.h>` for more information.
 *
 * @warning This function assumes that the file content is UTF-8 encoded and processes the input
 * accordingly. If the input contains invalid UTF-8 sequences, the behavior is undefined.
 *
 * @param[in, out] context  `SCUnitContext` to append to.
 * @param[in]      filename Name of the file to read the file context from.
 * @param[in]      line     Line around which the file context should be read.
 * @param[out]     error    `SCUNIT_ERROR_ARGUMENT_OUT_OF_RANGE` if `line` is less than one (lines
 *                          are one-based), `SCUNIT_ERROR_OPENING_STREAM_FAILED` if opening the file
 *                          with the name `filename` failed,
 *                          `SCUNIT_ERROR_READING_STREAM_FAILED` if reading the file with the name
 *                          `filename` failed, `SCUNIT_ERROR_CLOSING_STREAM_FAILED` if closing the
 *                          file with the name `filename` failed, `SCUNIT_ERROR_OUT_OF_MEMORY` if an
 *                          out-of-memory condition occurred,
 *                          `SCUNIT_ERROR_WRITING_BUFFER_FAILED` if appending the file context
 *                          failed and `SCUNIT_ERROR_NONE` otherwise.
 */
void scunit_context_appendFileContext(
    SCUnitContext* context,
    const char* filename,
    int64_t line,
    SCUnitError* error
);

/**
 * @brief Deallocates a given `SCUnitContext`.
 *
 * @note Deallocation of an `SCUnitContext` is handled automatically by the `SCUnitSuite` containing
 * the relevant test. You should not use this function, it is intended for internal use only.
 *
 * @warning Any use of the `SCUnitContext` after it has been deallocated results in undefined
 * behavior.
 *
 * @param[in, out] context `SCUnitContext` to deallocate.
 */
void scunit_context_free(SCUnitContext* context);

#endif