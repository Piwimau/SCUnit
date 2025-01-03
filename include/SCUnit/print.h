/**
 * @file print.h
 *
 * @brief A utility module providing various functions for printing formatted and colored messages
 * to streams and buffers.
 *
 * @details Most of the functions are simply wrappers around the standard C counterparts from
 * `<stdio.h>` and are deliberately named the same for familiarity. However, they provide better
 * parameter validation and clearer error semantics.
 *
 * Functions with the suffix `c` (as in `colored`) allow for printing messages with two additional
 * arguments of type `SCUnitColor` for the foreground and background color. Note that colors are
 * only incorporated into the message if the colored output is currently set to
 * `SCUNIT_COLORED_OUTPUT_ENABLED`. Otherwise, the functions simply ignore the colors and use the
 * default color of your terminal instead.
 *
 * Colored output is enabled by default, but can be checked at any time with the function
 * `scunit_getColoredOutput()`. If your terminal does not support colored output or you want to
 * disable it (for example to print the output to a file), simply call `scunit_setColoredOutput()`
 * with the argument `SCUNIT_COLORED_OUTPUT_DISABLED`. Another way to achieve this is to set the
 * option `--colored-output=disabled` when running the test executable. Note that this option is
 * only respected if you explicitly call `scunit_main()` from `<scunit.h>`, which parses any command
 * line arguments passed to the test executable.
 *
 * @warning Examining and setting the colored output via `scunit_getColoredOutput()` and
 * `scunit_setColoredOutput()` is not thread-safe. Concurrent access by multiple threads may result
 * in undefined behavior. It is your responsibility to ensure that any access is properly
 * synchronized when used in a multi-threaded context.
 *
 * @author Philipp Raschka
 */

#ifndef SCUNIT_PRINT_H
#define SCUNIT_PRINT_H

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <SCUnit/error.h>

/** @brief Represents an enumeration of the states the colored output may be in. */
typedef enum SCUnitColoredOutput {

    /** @brief Indicates that the colored output is currently disabled. */
    SCUNIT_COLORED_OUTPUT_DISABLED,

    /** @brief Indicates that the colored output is currently enabled. */
    SCUNIT_COLORED_OUTPUT_ENABLED

} SCUnitColoredOutput;

/** @brief Represents a color that may be used as the fore- or background for printing. */
typedef enum SCUnitColor {

    /** @brief Indicates a dark black color used as the fore- or background for printing. */
    SCUNIT_COLOR_DARK_BLACK,

    /** @brief Indicates a dark red color used as the fore- or background for printing. */
    SCUNIT_COLOR_DARK_RED,

    /** @brief Indicates a dark green color used as the fore- or background for printing. */
    SCUNIT_COLOR_DARK_GREEN,

    /** @brief Indicates a dark yellow color used as the fore- or background for printing. */
    SCUNIT_COLOR_DARK_YELLOW,

    /** @brief Indicates a dark blue color used as the fore- or background for printing. */
    SCUNIT_COLOR_DARK_BLUE,

    /** @brief Indicates a dark magenta color used as the fore- or background for printing. */
    SCUNIT_COLOR_DARK_MAGENTA,

    /** @brief Indicates a dark cyan color used as the fore- or background for printing. */
    SCUNIT_COLOR_DARK_CYAN,

    /** @brief Indicates a dark white color used as the fore- or background for printing. */
    SCUNIT_COLOR_DARK_WHITE,

    /** @brief Indicates a dark default color used as the fore- or background for printing. */
    SCUNIT_COLOR_DARK_DEFAULT,

    /** @brief Indicates a bright black color used as the fore- or background for printing. */
    SCUNIT_COLOR_BRIGHT_BLACK,

    /** @brief Indicates a bright red color used as the fore- or background for printing. */
    SCUNIT_COLOR_BRIGHT_RED,

    /** @brief Indicates a bright green color used as the fore- or background for printing. */
    SCUNIT_COLOR_BRIGHT_GREEN,

    /** @brief Indicates a bright yellow color used as the fore- or background for printing. */
    SCUNIT_COLOR_BRIGHT_YELLOW,

    /** @brief Indicates a bright blue color used as the fore- or background for printing. */
    SCUNIT_COLOR_BRIGHT_BLUE,

    /** @brief Indicates a bright magenta color used as the fore- or background for printing. */
    SCUNIT_COLOR_BRIGHT_MAGENTA,

    /** @brief Indicates a bright cyan color used as the fore- or background for printing. */
    SCUNIT_COLOR_BRIGHT_CYAN,

    /** @brief Indicates a bright white color used as the fore- or background for printing. */
    SCUNIT_COLOR_BRIGHT_WHITE,

    /** @brief Indicates a bright default color used as the fore- or background for printing. */
    SCUNIT_COLOR_BRIGHT_DEFAULT

} SCUnitColor;

/**
 * @brief Gets the current `SCUnitColoredOutput` state.
 *
 * @note The colored output is set to `SCUNIT_COLORED_OUTPUT_ENABLED` by default.
 *
 * @return `SCUNIT_COLORED_OUTPUT_DISABLED` if the colored output is currently disabled,
 * otherwise `SCUNIT_COLORED_OUTPUT_ENABLED`.
 */
SCUnitColoredOutput scunit_getColoredOutput();

/**
 * @brief Sets the current `SCUnitColoredOutput` state.
 *
 * @note The colored output is set to `SCUNIT_COLORED_OUTPUT_ENABLED` by default.
 *
 * @param[in] coloredOutput `SCUnitColoredOutput` to set the current state to.
 * @return `SCUNIT_ERROR_ARGUMENT_OUT_OF_RANGE` if `coloredOutput` is not a valid
 * `SCUnitColoredOutput` and `SCUNIT_ERROR_NONE` otherwise.
 */
SCUnitError scunit_setColoredOutput(SCUnitColoredOutput coloredOutput);

/**
 * @brief Writes a formatted string to the standard output stream.
 *
 * @param[in] format A null-terminated format string following the same conventions as the standard
 *                   `printf` family of functions.
 * @param[in] ...    Any number of additional arguments to be formatted and written based on the
 *                   given format string.
 * @return `SCUNIT_ERROR_ARGUMENT_NULL` if `format` is `nullptr`,
 * `SCUNIT_ERROR_WRITING_STREAM_FAILED` if writing to `stdout` failed and `SCUNIT_ERROR_NONE`
 * otherwise.
 */
SCUnitError scunit_printf(const char* format, ...);

/**
 * @brief Writes a formatted string to the standard output stream.
 *
 * @attention This function does not explicitly call `va_end()` with the `args` parameter. Instead,
 * the caller is expected to do so in order to clean up any remaining resources.
 *
 * @param[in] format A null-terminated format string following the same conventions as the standard
 *                   `printf` family of functions.
 * @param[in] args   A `va_list` of arguments to be formatted and written based on the given format
 *                   string.
 * @return `SCUNIT_ERROR_ARGUMENT_NULL` if `format` is `nullptr`,
 * `SCUNIT_ERROR_WRITING_STREAM_FAILED` if writing to `stdout` failed and `SCUNIT_ERROR_NONE`
 * otherwise.
 */
SCUnitError scunit_vprintf(const char* format, va_list args);

/**
 * @brief Writes a formatted and colored string to the standard output stream.
 *
 * @note This function respects the current `SCUnitColoredOutput` state set by calling
 * `scunit_setColoredOutput()`. If currently set to `SCUNIT_COLORED_OUTPUT_DISABLED`,
 * `foreground` and `background` are ignored and the default terminal color is used instead.
 *
 * @param[in] foreground A `SCUnitColor` to use as the foreground color.
 * @param[in] background A `SCUnitColor` to use as the background color.
 * @param[in] format     A null-terminated format string following the same conventions as the
 *                       standard `printf` family of functions.
 * @param[in] ...        Any number of additional arguments to be formatted and written based on the
 *                       given format string.
 * @return `SCUNIT_ERROR_ARGUMENT_OUT_OF_RANGE` if `foreground` or `background` is not a valid
 * `SCUnitColor`, `SCUNIT_ERROR_ARGUMENT_NULL` if `format` is `nullptr`,
 * `SCUNIT_ERROR_WRITING_STREAM_FAILED` if writing to `stdout` failed and `SCUNIT_ERROR_NONE`
 * otherwise.
 */
SCUnitError scunit_printfc(
    SCUnitColor foreground,
    SCUnitColor background,
    const char* format,
    ...
);

/**
 * @brief Writes a formatted and colored string to the standard output stream.
 *
 * @note This function respects the current `SCUnitColoredOutput` state set by calling
 * `scunit_setColoredOutput()`. If currently set to `SCUNIT_COLORED_OUTPUT_DISABLED`,
 * `foreground` and `background` are ignored and the default terminal color is used instead.
 *
 * @attention This function does not explicitly call `va_end()` with the `args` parameter. Instead,
 * the caller is expected to do so in order to clean up any remaining resources.
 *
 * @param[in] foreground A `SCUnitColor` to use as the foreground color.
 * @param[in] background A `SCUnitColor` to use as the background color.
 * @param[in] format     A null-terminated format string following the same conventions as the
 *                       standard `printf` family of functions.
 * @param[in] args       A `va_list` of arguments to be formatted and written based on the given
 *                       format string.
 * @return `SCUNIT_ERROR_ARGUMENT_OUT_OF_RANGE` if `foreground` or `background` is not a valid
 * `SCUnitColor`, `SCUNIT_ERROR_ARGUMENT_NULL` if `format` is `nullptr`,
 * `SCUNIT_ERROR_WRITING_STREAM_FAILED` if writing to `stdout` failed and `SCUNIT_ERROR_NONE`
 * otherwise.
 */
SCUnitError scunit_vprintfc(
    SCUnitColor foreground,
    SCUnitColor background,
    const char* format,
    va_list args
);

/**
 * @brief Writes a formatted string to a given output stream.
 *
 * @param[in, out] stream Output stream to write the formatted string to.
 * @param[in]      format A null-terminated format string following the same conventions as the
 *                        standard `printf` family of functions.
 * @param[in]      ...    Any number of additional arguments to be formatted and written based on
 *                        the given format string.
 * @return `SCUNIT_ERROR_ARGUMENT_NULL` if `stream` or `format` is `nullptr`,
 * `SCUNIT_ERROR_WRITING_STREAM_FAILED` if writing to `stream` failed and `SCUNIT_ERROR_NONE`
 * otherwise.
 */
SCUnitError scunit_fprintf(FILE* stream, const char* format, ...);

/**
 * @brief Writes a formatted string to a given output stream.
 *
 * @attention This function does not explicitly call `va_end()` with the `args` parameter. Instead,
 * the caller is expected to do so in order to clean up any remaining resources.
 *
 * @param[in, out] stream Output stream to write the formatted string to.
 * @param[in]      format A null-terminated format string following the same conventions as the
 *                        standard `printf` family of functions.
 * @param[in]      args   A `va_list` of arguments to be formatted and written based on the given
 *                        format string.
 * @return `SCUNIT_ERROR_ARGUMENT_NULL` if `stream` or `format` is `nullptr`,
 * `SCUNIT_ERROR_WRITING_STREAM_FAILED` if writing to `stream` failed and `SCUNIT_ERROR_NONE`
 * otherwise.
 */
SCUnitError scunit_vfprintf(FILE* stream, const char* format, va_list args);

/**
 * @brief Writes a formatted and colored string to a given output stream.
 *
 * @note This function respects the current `SCUnitColoredOutput` state set by calling
 * `scunit_setColoredOutput()`. If currently set to `SCUNIT_COLORED_OUTPUT_DISABLED`,
 * `foreground` and `background` are ignored and the default terminal color is used instead.
 *
 * @param[in, out] stream     Output stream to write the formatted string to.
 * @param[in]      foreground A `SCUnitColor` to use as the foreground color.
 * @param[in]      background A `SCUnitColor` to use as the background color.
 * @param[in]      format     A null-terminated format string following the same conventions as the
 *                            standard `printf` family of functions.
 * @param[in]      ...        Any number of additional arguments to be formatted and written based
 *                            on the given format string.
 * @return `SCUNIT_ERROR_ARGUMENT_OUT_OF_RANGE` if `foreground` or `background` is not a valid
 * `SCUnitColor`, `SCUNIT_ERROR_ARGUMENT_NULL` if `stream` or `format` is `nullptr`,
 * `SCUNIT_ERROR_WRITING_STREAM_FAILED` if writing to `stream` failed and `SCUNIT_ERROR_NONE`
 * otherwise.
 */
SCUnitError scunit_fprintfc(
    FILE* stream,
    SCUnitColor foreground,
    SCUnitColor background,
    const char* format,
    ...
);

/**
 * @brief Writes a formatted and colored string to a given output stream.
 *
 * @note This function respects the current `SCUnitColoredOutput` state set by calling
 * `scunit_setColoredOutput()`. If currently set to `SCUNIT_COLORED_OUTPUT_DISABLED`,
 * `foreground` and `background` are ignored and the default terminal color is used instead.
 *
 * @attention This function does not explicitly call `va_end()` with the `args` parameter. Instead,
 * the caller is expected to do so in order to clean up any remaining resources.
 *
 * @param[in, out] stream     Output stream to write the formatted string to.
 * @param[in]      foreground A `SCUnitColor` to use as the foreground color.
 * @param[in]      background A `SCUnitColor` to use as the background color.
 * @param[in]      format     A null-terminated format string following the same conventions as the
 *                            standard `printf` family of functions.
 * @param[in]      args       A `va_list` of arguments to be formatted and written based on the
 *                            given format string.
 * @return `SCUNIT_ERROR_ARGUMENT_OUT_OF_RANGE` if `foreground` or `background` is not a valid
 * `SCUnitColor`, `SCUNIT_ERROR_ARGUMENT_NULL` if `stream` or `format` is `nullptr`,
 * `SCUNIT_ERROR_WRITING_STREAM_FAILED` if writing to `stream` failed and `SCUNIT_ERROR_NONE`
 * otherwise.
 */
SCUnitError scunit_vfprintfc(
    FILE* stream,
    SCUnitColor foreground,
    SCUnitColor background,
    const char* format,
    va_list args
);

/**
 * @brief Writes a formatted string to a given dynamically allocated output buffer, resizing it as
 * necessary.
 *
 * @note For convenience, `*buffer` is allowed to be `nullptr`, in which case `*size` must be equal
 * to zero (and vice versa). `*buffer` is then allocated to a certain initial size by this function.
 *
 * @details If `buffer`, `size` or `format` is `nullptr`, all arguments remain unchanged and
 * `SCUNIT_ERROR_ARGUMENT_NULL` is returned. The same holds true if `*size` is negative,
 * if `*buffer` is `nullptr` and `*size` is not equal to zero or if `*buffer` is not `nullptr` and
 * `*size` is equal to zero, except in these cases `SCUNIT_ERROR_ARGUMENT_OUT_OF_RANGE` is returned.
 *
 * If no error occurs, the formatted string is written to `*buffer` (resizing it as necessary),
 * `*buffer` is null-terminated and `SCUNIT_ERROR_NONE` is returned.
 *
 * @warning If an out-of-memory condition occurs or if writing to `*buffer` fails, both `*buffer`
 * and `*size` retain the original state they were in before the failed operation. Note however that
 * the content of the string pointed to by `*buffer` is indeterminate in this case (i. e. it may
 * not be null-terminated).
 *
 * @param[in, out] buffer Dynamically allocated output buffer to write to. This buffer is resized
 *                        as necessary to fit the formatted string and is guaranteed to be
 *                        null-terminated if no error occurs.
 * @param[in, out] size   Size of the dynamically allocated output buffer (including the terminating
 *                        `\0` byte). The size is updated whenever `*buffer` is resized.
 * @param[in]      format A null-terminated format string following the same conventions as the
 *                        standard `printf` family of functions.
 * @param[in]      ...    Any number of additional arguments to be formatted and written based on
 *                        the given format string.
 * @return `SCUNIT_ERROR_ARGUMENT_NULL` if `buffer`, `size` or `format` is `nullptr`,
 * `SCUNIT_ERROR_ARGUMENT_OUT_OF_RANGE` if `*size` is negative, if `*buffer` is `nullptr` and
 * `*size` is not equal to zero or if `*buffer` is not `nullptr` and `*size` is equal to zero,
 * `SCUNIT_ERROR_OUT_OF_MEMORY` if resizing `*buffer` failed due to an out-of-memory condition,
 * `SCUNIT_ERROR_WRITING_BUFFER_FAILED` if writing to `*buffer` failed and `SCUNIT_ERROR_NONE`
 * otherwise.
 */
SCUnitError scunit_rsnprintf(char** buffer, int64_t* size, const char* format, ...);

/**
 * @brief Writes a formatted string to a given dynamically allocated output buffer, resizing it as
 * necessary.
 *
 * @note For convenience, `*buffer` is allowed to be `nullptr`, in which case `*size` must be equal
 * to zero (and vice versa). `*buffer` is then allocated to a certain initial size by this function.
 *
 * @details If `buffer`, `size` or `format` is `nullptr`, all arguments remain unchanged and
 * `SCUNIT_ERROR_ARGUMENT_NULL` is returned. The same holds true if `*size` is negative,
 * if `*buffer` is `nullptr` and `*size` is not equal to zero or if `*buffer` is not `nullptr` and
 * `*size` is equal to zero, except in these cases `SCUNIT_ERROR_ARGUMENT_OUT_OF_RANGE` is returned.
 *
 * If no error occurs, the formatted string is written to `*buffer` (resizing it as necessary),
 * `*buffer` is null-terminated and `SCUNIT_ERROR_NONE` is returned.
 *
 * @attention This function does not explicitly call `va_end()` with the `args` parameter. Instead,
 * the caller is expected to do so in order to clean up any remaining resources.
 *
 * @warning If an out-of-memory condition occurs or if writing to `*buffer` fails, both `*buffer`
 * and `*size` retain the original state they were in before the failed operation. Note however that
 * the content of the string pointed to by `*buffer` is indeterminate in this case (i. e. it may
 * not be null-terminated).
 *
 * @param[in, out] buffer Dynamically allocated output buffer to write to. This buffer is resized
 *                        as necessary to fit the formatted string and is guaranteed to be
 *                        null-terminated if no error occurs.
 * @param[in, out] size   Size of the dynamically allocated output buffer (including the terminating
 *                        `\0` byte). The size is updated whenever `*buffer` is resized.
 * @param[in]      format A null-terminated format string following the same conventions as the
 *                        standard `printf` family of functions.
 * @param[in]      args   A `va_list` of arguments to be formatted and written based on the given
 *                        format string.
 * @return `SCUNIT_ERROR_ARGUMENT_NULL` if `buffer`, `size` or `format` is `nullptr`,
 * `SCUNIT_ERROR_ARGUMENT_OUT_OF_RANGE` if `*size` is negative, if `*buffer` is `nullptr` and
 * `*size` is not equal to zero or if `*buffer` is not `nullptr` and `*size` is equal to zero,
 * `SCUNIT_ERROR_OUT_OF_MEMORY` if resizing `*buffer` failed due to an out-of-memory condition,
 * `SCUNIT_ERROR_WRITING_BUFFER_FAILED` if writing to `*buffer` failed and `SCUNIT_ERROR_NONE`
 * otherwise.
 */
SCUnitError scunit_vrsnprintf(char** buffer, int64_t* size, const char* format, va_list args);

/**
 * @brief Writes a formatted and colored string to a given dynamically allocated output buffer,
 * resizing it as necessary.
 *
 * @note For convenience, `*buffer` is allowed to be `nullptr`, in which case `*size` must be equal
 * to zero (and vice versa). `*buffer` is then allocated to a certain initial size by this function.
 *
 * This function respects the current `SCUnitColoredOutput` state set by calling
 * `scunit_setColoredOutput()`. If currently set to `SCUNIT_COLORED_OUTPUT_DISABLED`,
 * `foreground` and `background` are ignored and the default terminal color is used instead.
 *
 * @details If `buffer`, `size` or `format` is `nullptr`, all arguments remain unchanged and
 * `SCUNIT_ERROR_ARGUMENT_NULL` is returned. The same holds true if `foreground` or `background` is
 * not a valid `SCUnitColor`, if `*size` is negative, if `*buffer` is `nullptr` and `*size` is not
 * equal to zero or if `*buffer` is not `nullptr` and `*size` is equal to zero, except in these
 * cases `SCUNIT_ERROR_ARGUMENT_OUT_OF_RANGE` is returned.
 *
 * If no error occurs, the formatted string is written to `*buffer` (resizing it as necessary),
 * `*buffer` is null-terminated and `SCUNIT_ERROR_NONE` is returned.
 *
 * @warning If an out-of-memory condition occurs or if writing to `*buffer` fails, both `*buffer`
 * and `*size` retain the original state they were in before the failed operation. Note however that
 * the content of the string pointed to by `*buffer` is indeterminate in this case (i. e. it may
 * not be null-terminated).
 *
 * @param[in, out] buffer     Dynamically allocated output buffer to write to. This buffer is
 *                            resized as necessary to fit the formatted string and is guaranteed to
 *                            be null-terminated if no error occurs.
 * @param[in, out] size       Size of the dynamically allocated output buffer (including the
 *                            terminating `\0` byte). The size is updated whenever `*buffer` is
 *                            resized.
 * @param[in]      foreground A `SCUnitColor` to use as the foreground color.
 * @param[in]      background A `SCUnitColor` to use as the background color.
 * @param[in]      format     A null-terminated format string following the same conventions as the
 *                            standard `printf` family of functions.
 * @param[in]      ...        Any number of additional arguments to be formatted and written based
 *                            on the given format string.
 * @return `SCUNIT_ERROR_ARGUMENT_NULL` if `buffer`, `size` or `format` is `nullptr`,
 * `SCUNIT_ERROR_ARGUMENT_OUT_OF_RANGE` if `foreground` or `background` is not a valid
 * `SCUnitColor`, if `*size` is negative, if `*buffer` is `nullptr` and `*size` is not equal to zero
 * or if `*buffer` is not `nullptr` and `*size` is equal to zero, `SCUNIT_ERROR_OUT_OF_MEMORY` if
 * resizing `*buffer` failed due to an out-of-memory condition,
 * `SCUNIT_ERROR_WRITING_BUFFER_FAILED` if writing to `*buffer` failed and `SCUNIT_ERROR_NONE`
 * otherwise.
 */
SCUnitError scunit_rsnprintfc(
    char** buffer,
    int64_t* size,
    SCUnitColor foreground,
    SCUnitColor background,
    const char* format,
    ...
);

/**
 * @brief Writes a formatted and colored string to a given dynamically allocated output buffer,
 * resizing it as necessary.
 *
 * @note For convenience, `*buffer` is allowed to be `nullptr`, in which case `*size` must be equal
 * to zero (and vice versa). `*buffer` is then allocated to a certain initial size by this function.
 *
 * This function respects the current `SCUnitColoredOutput` state set by calling
 * `scunit_setColoredOutput()`. If currently set to `SCUNIT_COLORED_OUTPUT_DISABLED`,
 * `foreground` and `background` are ignored and the default terminal color is used instead.
 *
 * @details If `buffer`, `size` or `format` is `nullptr`, all arguments remain unchanged and
 * `SCUNIT_ERROR_ARGUMENT_NULL` is returned. The same holds true if `foreground` or `background` is
 * not a valid `SCUnitColor`, if `*size` is negative, if `*buffer` is `nullptr` and `*size` is not
 * equal to zero or if `*buffer` is not `nullptr` and `*size` is equal to zero, except in these
 * cases `SCUNIT_ERROR_ARGUMENT_OUT_OF_RANGE` is returned.
 *
 * If no error occurs, the formatted string is written to `*buffer` (resizing it as necessary),
 * `*buffer` is null-terminated and `SCUNIT_ERROR_NONE` is returned.
 *
 * @attention This function does not explicitly call `va_end()` with the `args` parameter. Instead,
 * the caller is expected to do so in order to clean up any remaining resources.
 *
 * @warning If an out-of-memory condition occurs or if writing to `*buffer` fails, both `*buffer`
 * and `*size` retain the original state they were in before the failed operation. Note however that
 * the content of the string pointed to by `*buffer` is indeterminate in this case (i. e. it may
 * not be null-terminated).
 *
 * @param[in, out] buffer     Dynamically allocated output buffer to write to. This buffer is
 *                            resized as necessary to fit the formatted string and is guaranteed to
 *                            be null-terminated if no error occurs.
 * @param[in, out] size       Size of the dynamically allocated output buffer (including the
 *                            terminating `\0` byte). The size is updated whenever `*buffer` is
 *                            resized.
 * @param[in]      foreground A `SCUnitColor` to use as the foreground color.
 * @param[in]      background A `SCUnitColor` to use as the background color.
 * @param[in]      format     A null-terminated format string following the same conventions as the
 *                            standard `printf` family of functions.
 * @param[in]      args       A `va_list` of arguments to be formatted and written based on the
 *                            given format string.
 * @return `SCUNIT_ERROR_ARGUMENT_NULL` if `buffer`, `size` or `format` is `nullptr`,
 * `SCUNIT_ERROR_ARGUMENT_OUT_OF_RANGE` if `foreground` or `background` is not a valid
 * `SCUnitColor`, if `*size` is negative, if `*buffer` is `nullptr` and `*size` is not equal to zero
 * or if `*buffer` is not `nullptr` and `*size` is equal to zero, `SCUNIT_ERROR_OUT_OF_MEMORY` if
 * resizing `*buffer` failed due to an out-of-memory condition,
 * `SCUNIT_ERROR_WRITING_BUFFER_FAILED` if writing to `*buffer` failed and `SCUNIT_ERROR_NONE`
 * otherwise.
 */
SCUnitError scunit_vrsnprintfc(
    char** buffer,
    int64_t* size,
    SCUnitColor foreground,
    SCUnitColor background,
    const char* format,
    va_list args
);

/**
 * @brief Appends a formatted string to a given dynamically allocated output buffer, resizing it as
 * necessary.
 *
 * @note For convenience, `*buffer` is allowed to be `nullptr`, in which case `*size` must be equal
 * to zero (and vice versa). `*buffer` is then allocated to a certain initial size by this function.
 *
 * @details If `buffer`, `size` or `format` is `nullptr`, all arguments remain unchanged and
 * `SCUNIT_ERROR_ARGUMENT_NULL` is returned. The same holds true if `*size` is negative,
 * if `*buffer` is `nullptr` and `*size` is not equal to zero or if `*buffer` is not `nullptr` and
 * `*size` is equal to zero, except in these cases `SCUNIT_ERROR_ARGUMENT_OUT_OF_RANGE` is returned.
 *
 * If no error occurs, the formatted string is appended to `*buffer` (resizing it as necessary) at
 * the previous terminating `\0` byte (overwriting it with the first byte of the formatted string),
 * `*buffer` is null-terminated and `SCUNIT_ERROR_NONE` is returned.
 *
 * @warning If an out-of-memory condition occurs or if appending to `*buffer` fails, both `*buffer`
 * and `*size` retain the original state they were in before the failed operation. Note however that
 * the content of the string pointed to by `*buffer` is indeterminate in this case (i. e. it may
 * not be null-terminated).
 *
 * @param[in, out] buffer Dynamically allocated output buffer to append to. This buffer is resized
 *                        as necessary to fit the formatted string and is guaranteed to be
 *                        null-terminated if no error occurs.
 * @param[in, out] size   Size of the dynamically allocated output buffer (including the terminating
 *                        `\0` byte). The size is updated whenever `*buffer` is resized.
 * @param[in]      format A null-terminated format string following the same conventions as the
 *                        standard `printf` family of functions.
 * @param[in]      ...    Any number of additional arguments to be formatted and appended based on
 *                        the given format string.
 * @return `SCUNIT_ERROR_ARGUMENT_NULL` if `buffer`, `size` or `format` is `nullptr`,
 * `SCUNIT_ERROR_ARGUMENT_OUT_OF_RANGE` if `*size` is negative, if `*buffer` is `nullptr` and
 * `*size` is not equal to zero or if `*buffer` is not `nullptr` and `*size` is equal to zero,
 * `SCUNIT_ERROR_OUT_OF_MEMORY` if resizing `*buffer` failed due to an out-of-memory condition,
 * `SCUNIT_ERROR_WRITING_BUFFER_FAILED` if appending to `*buffer` failed and `SCUNIT_ERROR_NONE`
 * otherwise.
 */
SCUnitError scunit_rasnprintf(char** buffer, int64_t* size, const char* format, ...);

/**
 * @brief Appends a formatted string to a given dynamically allocated output buffer, resizing it as
 * necessary.
 *
 * @note For convenience, `*buffer` is allowed to be `nullptr`, in which case `*size` must be equal
 * to zero (and vice versa). `*buffer` is then allocated to a certain initial size by this function.
 *
 * @details If `buffer`, `size` or `format` is `nullptr`, all arguments remain unchanged and
 * `SCUNIT_ERROR_ARGUMENT_NULL` is returned. The same holds true if `*size` is negative,
 * if `*buffer` is `nullptr` and `*size` is not equal to zero or if `*buffer` is not `nullptr` and
 * `*size` is equal to zero, except in these cases `SCUNIT_ERROR_ARGUMENT_OUT_OF_RANGE` is returned.
 *
 * If no error occurs, the formatted string is appended to `*buffer` (resizing it as necessary) at
 * the previous terminating `\0` byte (overwriting it with the first byte of the formatted string),
 * `*buffer` is null-terminated and `SCUNIT_ERROR_NONE` is returned.
 *
 * @attention This function does not explicitly call `va_end()` with the `args` parameter. Instead,
 * the caller is expected to do so in order to clean up any remaining resources.
 *
 * @warning If an out-of-memory condition occurs or if appending to `*buffer` fails, both `*buffer`
 * and `*size` retain the original state they were in before the failed operation. Note however that
 * the content of the string pointed to by `*buffer` is indeterminate in this case (i. e. it may
 * not be null-terminated).
 *
 * @param[in, out] buffer Dynamically allocated output buffer to append to. This buffer is resized
 *                        as necessary to fit the formatted string and is guaranteed to be
 *                        null-terminated if no error occurs.
 * @param[in, out] size   Size of the dynamically allocated output buffer (including the terminating
 *                        `\0` byte). The size is updated whenever `*buffer` is resized.
 * @param[in]      format A null-terminated format string following the same conventions as the
 *                        standard `printf` family of functions.
 * @param[in]      args   A `va_list` of arguments to be formatted and appended based on the given
 *                        format string.
 * @return `SCUNIT_ERROR_ARGUMENT_NULL` if `buffer`, `size` or `format` is `nullptr`,
 * `SCUNIT_ERROR_ARGUMENT_OUT_OF_RANGE` if `*size` is negative, if `*buffer` is `nullptr` and
 * `*size` is not equal to zero or if `*buffer` is not `nullptr` and `*size` is equal to zero,
 * `SCUNIT_ERROR_OUT_OF_MEMORY` if resizing `*buffer` failed due to an out-of-memory condition,
 * `SCUNIT_ERROR_WRITING_BUFFER_FAILED` if appending to `*buffer` failed and `SCUNIT_ERROR_NONE`
 * otherwise.
 */
SCUnitError scunit_vrasnprintf(char** buffer, int64_t* size, const char* format, va_list args);

/**
 * @brief Appends a formatted and colored string to a given dynamically allocated output buffer,
 * resizing it as necessary.
 *
 * @note For convenience, `*buffer` is allowed to be `nullptr`, in which case `*size` must be equal
 * to zero (and vice versa). `*buffer` is then allocated to a certain initial size by this function.
 *
 * This function respects the current `SCUnitColoredOutput` state set by calling
 * `scunit_setColoredOutput()`. If currently set to `SCUNIT_COLORED_OUTPUT_DISABLED`,
 * `foreground` and `background` are ignored and the default terminal color is used instead.
 *
 * @details If `buffer`, `size` or `format` is `nullptr`, all arguments remain unchanged and
 * `SCUNIT_ERROR_ARGUMENT_NULL` is returned. The same holds true if `foreground` or `background` is
 * not a valid `SCUnitColor`, if `*size` is negative, if `*buffer` is `nullptr` and `*size` is not
 * equal to zero or if `*buffer` is not `nullptr` and `*size` is equal to zero, except in these
 * cases `SCUNIT_ERROR_ARGUMENT_OUT_OF_RANGE` is returned.
 *
 * If no error occurs, the formatted string is appended to `*buffer` (resizing it as necessary) at
 * the previous terminating `\0` byte (overwriting it with the first byte of the formatted string),
 * `*buffer` is null-terminated and `SCUNIT_ERROR_NONE` is returned.
 *
 * @warning If an out-of-memory condition occurs or if appending to `*buffer` fails, both `*buffer`
 * and `*size` retain the original state they were in before the failed operation. Note however that
 * the content of the string pointed to by `*buffer` is indeterminate in this case (i. e. it may
 * not be null-terminated).
 *
 * @param[in, out] buffer     Dynamically allocated output buffer to append to. This buffer is
 *                            resized as necessary to fit the formatted string and is guaranteed to
 *                            be null-terminated if no error occurs.
 * @param[in, out] size       Size of the dynamically allocated output buffer (including the
 *                            terminating `\0` byte). The size is updated whenever `*buffer` is
 *                            resized.
 * @param[in]      foreground A `SCUnitColor` to use as the foreground color.
 * @param[in]      background A `SCUnitColor` to use as the background color.
 * @param[in]      format     A null-terminated format string following the same conventions as the
 *                            standard `printf` family of functions.
 * @param[in]      ...        Any number of additional arguments to be formatted and appended based
 *                            on the given format string.
 * @return `SCUNIT_ERROR_ARGUMENT_NULL` if `buffer`, `size` or `format` is `nullptr`,
 * `SCUNIT_ERROR_ARGUMENT_OUT_OF_RANGE` if `foreground` or `background` is not a valid
 * `SCUnitColor`, if `*size` is negative, if `*buffer` is `nullptr` and `*size` is not equal to zero
 * or if `*buffer` is not `nullptr` and `*size` is equal to zero, `SCUNIT_ERROR_OUT_OF_MEMORY` if
 * resizing `*buffer` failed due to an out-of-memory condition,
 * `SCUNIT_ERROR_WRITING_BUFFER_FAILED` if appending to `*buffer` failed and `SCUNIT_ERROR_NONE`
 * otherwise.
 */
SCUnitError scunit_rasnprintfc(
    char** buffer,
    int64_t* size,
    SCUnitColor foreground,
    SCUnitColor background,
    const char* format,
    ...
);

/**
 * @brief Appends a formatted and colored string to a given dynamically allocated output buffer,
 * resizing it as necessary.
 *
 * @note For convenience, `*buffer` is allowed to be `nullptr`, in which case `*size` must be equal
 * to zero (and vice versa). `*buffer` is then allocated to a certain initial size by this function.
 *
 * This function respects the current `SCUnitColoredOutput` state set by calling
 * `scunit_setColoredOutput()`. If currently set to `SCUNIT_COLORED_OUTPUT_DISABLED`,
 * `foreground` and `background` are ignored and the default terminal color is used instead.
 *
 * @details If `buffer`, `size` or `format` is `nullptr`, all arguments remain unchanged and
 * `SCUNIT_ERROR_ARGUMENT_NULL` is returned. The same holds true if `foreground` or `background` is
 * not a valid `SCUnitColor`, if `*size` is negative, if `*buffer` is `nullptr` and `*size` is not
 * equal to zero or if `*buffer` is not `nullptr` and `*size` is equal to zero, except in these
 * cases `SCUNIT_ERROR_ARGUMENT_OUT_OF_RANGE` is returned.
 *
 * If no error occurs, the formatted string is appended to `*buffer` (resizing it as necessary) at
 * the previous terminating `\0` byte (overwriting it with the first byte of the formatted string),
 * `*buffer` is null-terminated and `SCUNIT_ERROR_NONE` is returned.
 *
 * @attention This function does not explicitly call `va_end()` with the `args` parameter. Instead,
 * the caller is expected to do so in order to clean up any remaining resources.
 *
 * @warning If an out-of-memory condition occurs or if appending to `*buffer` fails, both `*buffer`
 * and `*size` retain the original state they were in before the failed operation. Note however that
 * the content of the string pointed to by `*buffer` is indeterminate in this case (i. e. it may
 * not be null-terminated).
 *
 * @param[in, out] buffer     Dynamically allocated output buffer to append to. This buffer is
 *                            resized as necessary to fit the formatted string and is guaranteed to
 *                            be null-terminated if no error occurs.
 * @param[in, out] size       Size of the dynamically allocated output buffer (including the
 *                            terminating `\0` byte). The size is updated whenever `*buffer` is
 *                            resized.
 * @param[in]      foreground A `SCUnitColor` to use as the foreground color.
 * @param[in]      background A `SCUnitColor` to use as the background color.
 * @param[in]      format     A null-terminated format string following the same conventions as the
 *                            standard `printf` family of functions.
 * @param[in]      args       A `va_list` of arguments to be formatted and appended based on the
 *                            given format string.
 * @return `SCUNIT_ERROR_ARGUMENT_NULL` if `buffer`, `size` or `format` is `nullptr`,
 * `SCUNIT_ERROR_ARGUMENT_OUT_OF_RANGE` if `foreground` or `background` is not a valid
 * `SCUnitColor`, if `*size` is negative, if `*buffer` is `nullptr` and `*size` is not equal to zero
 * or if `*buffer` is not `nullptr` and `*size` is equal to zero, `SCUNIT_ERROR_OUT_OF_MEMORY` if
 * resizing `*buffer` failed due to an out-of-memory condition,
 * `SCUNIT_ERROR_WRITING_BUFFER_FAILED` if appending to `*buffer` failed and `SCUNIT_ERROR_NONE`
 * otherwise.
 */
SCUnitError scunit_vrasnprintfc(
    char** buffer,
    int64_t* size,
    SCUnitColor foreground,
    SCUnitColor background,
    const char* format,
    va_list args
);

#endif