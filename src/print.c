#include <inttypes.h>
#include <string.h>
#include <SCUnit/memory.h>
#include <SCUnit/print.h>

/** @brief Size used for initially allocating a buffer. */
static constexpr int64_t INITIAL_BUFFER_SIZE = 128;

/** @brief Growth factor used for resizing a buffer. */
static constexpr int64_t GROWTH_FACTOR = 2;

/** @brief Escape code to start printing using a fore- and background color. */
static const char* const COLOR_START = "\033[%" PRId32 ";%" PRId32 "m";

/** @brief Escape code to reset the fore- and background color. */
static const char* const COLOR_RESET = "\033[0m";

/** @brief Numbers of the foreground colors used in escape codes. */
static constexpr int32_t FOREGROUND_COLORS[SCUNIT_COLOR_BRIGHT_DEFAULT + 1] = {
    [SCUNIT_COLOR_DARK_BLACK] = 30,
    [SCUNIT_COLOR_DARK_RED] = 31,
    [SCUNIT_COLOR_DARK_GREEN] = 32,
    [SCUNIT_COLOR_DARK_YELLOW] = 33,
    [SCUNIT_COLOR_DARK_BLUE] = 34,
    [SCUNIT_COLOR_DARK_MAGENTA] = 35,
    [SCUNIT_COLOR_DARK_CYAN] = 36,
    [SCUNIT_COLOR_DARK_WHITE] = 37,
    [SCUNIT_COLOR_DARK_DEFAULT] = 39,
    [SCUNIT_COLOR_BRIGHT_BLACK] = 90,
    [SCUNIT_COLOR_BRIGHT_RED] = 91,
    [SCUNIT_COLOR_BRIGHT_GREEN] = 92,
    [SCUNIT_COLOR_BRIGHT_YELLOW] = 93,
    [SCUNIT_COLOR_BRIGHT_BLUE] = 94,
    [SCUNIT_COLOR_BRIGHT_MAGENTA] = 95,
    [SCUNIT_COLOR_BRIGHT_CYAN] = 96,
    [SCUNIT_COLOR_BRIGHT_WHITE] = 97,
    [SCUNIT_COLOR_BRIGHT_DEFAULT] = 99
};

/** @brief Numbers of the background colors used in escape codes. */
static constexpr int32_t BACKGROUND_COLORS[SCUNIT_COLOR_BRIGHT_DEFAULT + 1] = {
    [SCUNIT_COLOR_DARK_BLACK] = 40,
    [SCUNIT_COLOR_DARK_RED] = 41,
    [SCUNIT_COLOR_DARK_GREEN] = 42,
    [SCUNIT_COLOR_DARK_YELLOW] = 43,
    [SCUNIT_COLOR_DARK_BLUE] = 44,
    [SCUNIT_COLOR_DARK_MAGENTA] = 45,
    [SCUNIT_COLOR_DARK_CYAN] = 46,
    [SCUNIT_COLOR_DARK_WHITE] = 47,
    [SCUNIT_COLOR_DARK_DEFAULT] = 49,
    [SCUNIT_COLOR_BRIGHT_BLACK] = 100,
    [SCUNIT_COLOR_BRIGHT_RED] = 101,
    [SCUNIT_COLOR_BRIGHT_GREEN] = 102,
    [SCUNIT_COLOR_BRIGHT_YELLOW] = 103,
    [SCUNIT_COLOR_BRIGHT_BLUE] = 104,
    [SCUNIT_COLOR_BRIGHT_MAGENTA] = 105,
    [SCUNIT_COLOR_BRIGHT_CYAN] = 106,
    [SCUNIT_COLOR_BRIGHT_WHITE] = 107,
    [SCUNIT_COLOR_BRIGHT_DEFAULT] = 109
};

/**
 * @brief Current state of the colored output that determines whether color is used when printing
 * formatted messages to streams and buffers.
 */
static SCUnitColoredOutput currentColoredOutput = SCUNIT_COLORED_OUTPUT_ENABLED;

SCUnitColoredOutput scunit_getColoredOutput() {
    return currentColoredOutput;
}

void scunit_setColoredOutput(SCUnitColoredOutput coloredOutput, SCUnitError* error) {
    if ((coloredOutput < SCUNIT_COLORED_OUTPUT_DISABLED)
            || (coloredOutput > SCUNIT_COLORED_OUTPUT_ENABLED)) {
        *error = SCUNIT_ERROR_ARGUMENT_OUT_OF_RANGE;
        return;
    }
    currentColoredOutput = coloredOutput;
    *error = SCUNIT_ERROR_NONE;
}

SCUnitError scunit_printf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    SCUnitError error = scunit_vprintf(format, args);
    va_end(args);
    return error;
}

SCUnitError scunit_vprintf(const char* format, va_list args) {
    return (vprintf(format, args) < 0) ? SCUNIT_ERROR_WRITING_STREAM_FAILED : SCUNIT_ERROR_NONE;
}

SCUnitError scunit_printfc(
    SCUnitColor foreground,
    SCUnitColor background,
    const char* format,
    ...
) {
    va_list args;
    va_start(args, format);
    SCUnitError error = scunit_vprintfc(foreground, background, format, args);
    va_end(args);
    return error;
}

/**
 * @brief Determines if a given value is a valid `SCUnitColor`.
 *
 * @param[in] color Value to check.
 * @return `true` if the given value is a valid `SCUnitColor`, otherwise `false`.
 */
static inline bool isValidColor(SCUnitColor color) {
    return (color >= SCUNIT_COLOR_DARK_BLACK) && (color <= SCUNIT_COLOR_BRIGHT_DEFAULT);
}

SCUnitError scunit_vprintfc(
    SCUnitColor foreground,
    SCUnitColor background,
    const char* format,
    va_list args
) {
    if (!isValidColor(foreground) || !isValidColor(background)) {
        return SCUNIT_ERROR_ARGUMENT_OUT_OF_RANGE;
    }
    if (currentColoredOutput == SCUNIT_COLORED_OUTPUT_ENABLED) {
        int result = printf(
            COLOR_START,
            FOREGROUND_COLORS[foreground],
            BACKGROUND_COLORS[background]
        );
        if (result < 0) {
            return SCUNIT_ERROR_WRITING_STREAM_FAILED;
        }
    }
    if (vprintf(format, args) < 0) {
        return SCUNIT_ERROR_WRITING_STREAM_FAILED;
    }
    if ((currentColoredOutput == SCUNIT_COLORED_OUTPUT_ENABLED) && (printf(COLOR_RESET) < 0)) {
        return SCUNIT_ERROR_WRITING_STREAM_FAILED;
    }
    return SCUNIT_ERROR_NONE;
}

SCUnitError scunit_fprintf(FILE* stream, const char* format, ...) {
    va_list args;
    va_start(args, format);
    SCUnitError error = scunit_vfprintf(stream, format, args);
    va_end(args);
    return error;
}

SCUnitError scunit_vfprintf(FILE* stream, const char* format, va_list args) {
    return (vfprintf(stream, format, args) < 0)
        ? SCUNIT_ERROR_WRITING_STREAM_FAILED
        : SCUNIT_ERROR_NONE;
}

SCUnitError scunit_fprintfc(
    FILE* stream,
    SCUnitColor foreground,
    SCUnitColor background,
    const char* format,
    ...
) {
    va_list args;
    va_start(args, format);
    SCUnitError error = scunit_vfprintfc(stream, foreground, background, format, args);
    va_end(args);
    return error;
}

SCUnitError scunit_vfprintfc(
    FILE* stream,
    SCUnitColor foreground,
    SCUnitColor background,
    const char* format,
    va_list args
) {
    if (!isValidColor(foreground) || !isValidColor(background)) {
        return SCUNIT_ERROR_ARGUMENT_OUT_OF_RANGE;
    }
    if (currentColoredOutput == SCUNIT_COLORED_OUTPUT_ENABLED) {
        int result = fprintf(
            stream,
            COLOR_START,
            FOREGROUND_COLORS[foreground],
            BACKGROUND_COLORS[background]
        );
        if (result < 0) {
            return SCUNIT_ERROR_WRITING_STREAM_FAILED;
        }
    }
    if (vfprintf(stream, format, args) < 0) {
        return SCUNIT_ERROR_WRITING_STREAM_FAILED;
    }
    if ((currentColoredOutput == SCUNIT_COLORED_OUTPUT_ENABLED)
            && (fprintf(stream, COLOR_RESET) < 0)) {
        return SCUNIT_ERROR_WRITING_STREAM_FAILED;
    }
    return SCUNIT_ERROR_NONE;
}

SCUnitError scunit_rsnprintf(char** buffer, int64_t* size, const char* format, ...) {
    va_list args;
    va_start(args, format);
    SCUnitError error = scunit_vrsnprintf(buffer, size, format, args);
    va_end(args);
    return error;
}

/**
 * @brief Ensures that a given buffer has at least a given required size by resizing if necessary.
 *
 * @note For convenience, `*buffer` is allowed to be `nullptr`, in which case `*size` must be equal
 * to zero (and vice versa).
 *
 * @param[in, out] buffer       Buffer that may need to be resized to have at least the required
 *                              size.
 * @param[in, out] size         Current size of the buffer (including the terminating `\0` byte).
 *                              The size is updated if `*buffer` is resized.
 * @param[in]      requiredSize Minimum size of the buffer to ensure.
 * @param[out]     error        `SCUNIT_ERROR_ARGUMENT_OUT_OF_RANGE` if `*size` or `requiredSize`
 *                              is negative, if `*buffer` is `nullptr` and `*size` is not equal to
 *                              zero or if `*buffer` is not `nullptr` and `*size` is equal to zero,
 *                              `SCUNIT_ERROR_OUT_OF_MEMORY` if an resizing the buffer failed due to
 *                              an out-of-memory condition and `SCUNIT_ERROR_NONE` otherwise.
 */
static inline void ensureSize(
    char** buffer,
    int64_t* size,
    int64_t requiredSize,
    SCUnitError* error
) {
    if ((*size < 0) || (requiredSize < 0) || ((*buffer == nullptr) != (*size == 0))) {
        *error = SCUNIT_ERROR_ARGUMENT_OUT_OF_RANGE;
        return;
    }
    if (*size < requiredSize) {
        int64_t newSize = (*size == 0) ? 1 : *size;
        while (newSize < requiredSize) {
            newSize *= GROWTH_FACTOR;
        }
        char* newBuffer = SCUNIT_REALLOC(*buffer, newSize);
        if (newBuffer == nullptr) {
            *error = SCUNIT_ERROR_OUT_OF_MEMORY;
            return;
        }
        *buffer = newBuffer;
        *size = newSize;
    }
    *error = SCUNIT_ERROR_NONE;
}

SCUnitError scunit_vrsnprintf(char** buffer, int64_t* size, const char* format, va_list args) {
    if ((*size < 0) || ((*buffer == nullptr) != (*size == 0))) {
        return SCUNIT_ERROR_ARGUMENT_OUT_OF_RANGE;
    }
    SCUnitError error;
    if (*buffer == nullptr) {
        ensureSize(buffer, size, INITIAL_BUFFER_SIZE, &error);
        if (error != SCUNIT_ERROR_NONE) {
            return error;
        }
        // Ensure the buffer is null-terminated since `ensureSize()` uses `SCUNIT_REALLOC()`
        // internally, which does not zero-initialize new memory areas of an expanded buffer.
        (*buffer)[0] = '\0';
    }
    va_list argsCopy;
    va_copy(argsCopy, args);
    int64_t length = vsnprintf(nullptr, 0, format, argsCopy);
    va_end(argsCopy);
    if (length < 0) {
        return SCUNIT_ERROR_WRITING_BUFFER_FAILED;
    }
    ensureSize(buffer, size, length + 1, &error);
    if (error != SCUNIT_ERROR_NONE) {
        return error;
    }
    if (vsnprintf(*buffer, *size, format, args) < 0) {
        return SCUNIT_ERROR_WRITING_BUFFER_FAILED;
    }
    return SCUNIT_ERROR_NONE;
}

SCUnitError scunit_rsnprintfc(
    char** buffer,
    int64_t* size,
    SCUnitColor foreground,
    SCUnitColor background,
    const char* format,
    ...
) {
    va_list args;
    va_start(args, format);
    SCUnitError error = scunit_vrsnprintfc(buffer, size, foreground, background, format, args);
    va_end(args);
    return error;
}

SCUnitError scunit_vrsnprintfc(
    char** buffer,
    int64_t* size,
    SCUnitColor foreground,
    SCUnitColor background,
    const char* format,
    va_list args
) {
    if (!isValidColor(foreground) || !isValidColor(background) || (*size < 0)
            || ((*buffer == nullptr) != (*size == 0))) {
        return SCUNIT_ERROR_ARGUMENT_OUT_OF_RANGE;
    }
    SCUnitError error;
    if (*buffer == nullptr) {
        ensureSize(buffer, size, INITIAL_BUFFER_SIZE, &error);
        if (error != SCUNIT_ERROR_NONE) {
            return error;
        }
        // Ensure the buffer is null-terminated since `ensureSize()` uses `SCUNIT_REALLOC()`
        // internally, which does not zero-initialize new memory areas of an expanded buffer.
        (*buffer)[0] = '\0';
    }
    int64_t offset = 0;
    if (currentColoredOutput == SCUNIT_COLORED_OUTPUT_ENABLED) {
        int64_t length = snprintf(
            nullptr,
            0,
            COLOR_START,
            FOREGROUND_COLORS[foreground],
            BACKGROUND_COLORS[background]
        );
        if (length < 0) {
            return SCUNIT_ERROR_WRITING_BUFFER_FAILED;
        }
        ensureSize(buffer, size, offset + length + 1, &error);
        if (error != SCUNIT_ERROR_NONE) {
            return error;
        }
        length = snprintf(
            *buffer + offset,
            *size - offset,
            COLOR_START,
            FOREGROUND_COLORS[foreground],
            BACKGROUND_COLORS[background]
        );
        if (length < 0) {
            return SCUNIT_ERROR_WRITING_BUFFER_FAILED;
        }
        offset += length;
    }
    va_list argsCopy;
    va_copy(argsCopy, args);
    int64_t length = vsnprintf(nullptr, 0, format, argsCopy);
    va_end(argsCopy);
    if (length < 0) {
        return SCUNIT_ERROR_WRITING_BUFFER_FAILED;
    }
    ensureSize(buffer, size, offset + length + 1, &error);
    if (error != SCUNIT_ERROR_NONE) {
        return error;
    }
    if (vsnprintf(*buffer + offset, *size - offset, format, args) < 0) {
        return SCUNIT_ERROR_WRITING_BUFFER_FAILED;
    }
    if (currentColoredOutput == SCUNIT_COLORED_OUTPUT_ENABLED) {
        offset += length;
        ensureSize(buffer, size, offset + strlen(COLOR_RESET) + 1, &error);
        if (error != SCUNIT_ERROR_NONE) {
            return error;
        }
        if (snprintf(*buffer + offset, *size - offset, COLOR_RESET) < 0) {
            return SCUNIT_ERROR_WRITING_BUFFER_FAILED;
        }
    }
    return SCUNIT_ERROR_NONE;
}

SCUnitError scunit_rasnprintf(char** buffer, int64_t* size, const char* format, ...) {
    va_list args;
    va_start(args, format);
    SCUnitError error = scunit_vrasnprintf(buffer, size, format, args);
    va_end(args);
    return error;
}

SCUnitError scunit_vrasnprintf(char** buffer, int64_t* size, const char* format, va_list args) {
    if ((*size < 0) || ((*buffer == nullptr) != (*size == 0))) {
        return SCUNIT_ERROR_ARGUMENT_OUT_OF_RANGE;
    }
    SCUnitError error;
    if (*buffer == nullptr) {
        ensureSize(buffer, size, INITIAL_BUFFER_SIZE, &error);
        if (error != SCUNIT_ERROR_NONE) {
            return error;
        }
        // Ensure the buffer is null-terminated since `ensureSize()` uses `SCUNIT_REALLOC()`
        // internally, which does not zero-initialize new memory areas of an expanded buffer.
        (*buffer)[0] = '\0';
    }
    va_list argsCopy;
    va_copy(argsCopy, args);
    int64_t length = vsnprintf(nullptr, 0, format, argsCopy);
    va_end(argsCopy);
    if (length < 0) {
        return SCUNIT_ERROR_WRITING_BUFFER_FAILED;
    }
    int64_t offset = strnlen(*buffer, *size);
    ensureSize(buffer, size, offset + length + 1, &error);
    if (error != SCUNIT_ERROR_NONE) {
        return error;
    }
    if (vsnprintf(*buffer + offset, *size - offset, format, args) < 0) {
        return SCUNIT_ERROR_WRITING_BUFFER_FAILED;
    }
    return SCUNIT_ERROR_NONE;
}

SCUnitError scunit_rasnprintfc(
    char** buffer,
    int64_t* size,
    SCUnitColor foreground,
    SCUnitColor background,
    const char* format,
    ...
) {
    va_list args;
    va_start(args, format);
    SCUnitError error = scunit_vrasnprintfc(buffer, size, foreground, background, format, args);
    va_end(args);
    return error;
}

SCUnitError scunit_vrasnprintfc(
    char** buffer,
    int64_t* size,
    SCUnitColor foreground,
    SCUnitColor background,
    const char* format,
    va_list args
) {
    if (!isValidColor(foreground) || !isValidColor(background) || (*size < 0)
            || ((*buffer == nullptr) != (*size == 0))) {
        return SCUNIT_ERROR_ARGUMENT_OUT_OF_RANGE;
    }
    SCUnitError error;
    if (*buffer == nullptr) {
        ensureSize(buffer, size, INITIAL_BUFFER_SIZE, &error);
        if (error != SCUNIT_ERROR_NONE) {
            return error;
        }
        // Ensure the buffer is null-terminated since `ensureSize()` uses `SCUNIT_REALLOC()`
        // internally, which does not zero-initialize new memory areas of an expanded buffer.
        (*buffer)[0] = '\0';
    }
    int64_t offset = strnlen(*buffer, *size);
    if (currentColoredOutput == SCUNIT_COLORED_OUTPUT_ENABLED) {
        int64_t length = snprintf(
            nullptr,
            0,
            COLOR_START,
            FOREGROUND_COLORS[foreground],
            BACKGROUND_COLORS[background]
        );
        if (length < 0) {
            return SCUNIT_ERROR_WRITING_BUFFER_FAILED;
        }
        ensureSize(buffer, size, offset + length + 1, &error);
        if (error != SCUNIT_ERROR_NONE) {
            return error;
        }
        length = snprintf(
            *buffer + offset,
            *size - offset,
            COLOR_START,
            FOREGROUND_COLORS[foreground],
            BACKGROUND_COLORS[background]
        );
        if (length < 0) {
            return SCUNIT_ERROR_WRITING_BUFFER_FAILED;
        }
        offset += length;
    }
    va_list argsCopy;
    va_copy(argsCopy, args);
    int64_t length = vsnprintf(nullptr, 0, format, argsCopy);
    va_end(argsCopy);
    if (length < 0) {
        return SCUNIT_ERROR_WRITING_BUFFER_FAILED;
    }
    ensureSize(buffer, size, offset + length + 1, &error);
    if (error != SCUNIT_ERROR_NONE) {
        return error;
    }
    if (vsnprintf(*buffer + offset, *size - offset, format, args) < 0) {
        return SCUNIT_ERROR_WRITING_BUFFER_FAILED;
    }
    if (currentColoredOutput == SCUNIT_COLORED_OUTPUT_ENABLED) {
        offset += length;
        ensureSize(buffer, size, offset + strlen(COLOR_RESET) + 1, &error);
        if (error != SCUNIT_ERROR_NONE) {
            return error;
        }
        if (snprintf(*buffer + offset, *size - offset, COLOR_RESET) < 0) {
            return SCUNIT_ERROR_WRITING_BUFFER_FAILED;
        }
    }
    return SCUNIT_ERROR_NONE;
}