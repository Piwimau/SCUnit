#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <SCUnit/context.h>
#include <SCUnit/memory.h>

struct SCUnitContext {

    /** @brief Result of this `SCUnitContext`. */
    SCUnitResult result;

    /**
     * @brief Size of the `message` buffer of this `SCUnitContext` (including the terminating `\0`
     * byte).
     */
    int64_t size;

    /**
     * @brief Message of this `SCUnitContext`, initially an empty string.
     *
     * @note This is a dynamically allocated string with a capacity of `size` bytes.
     */
    char* message;

};

/** @brief Size used for initially allocating a buffer. */
static constexpr int64_t INITIAL_BUFFER_SIZE = 128;

/** @brief Growth factor used for resizing a buffer. */
static constexpr int64_t GROWTH_FACTOR = 2;

/** @brief Number of context lines to be read and included around a line of a failed assertion. */
static constexpr int64_t CONTEXT_LINES = 2;

SCUnitContext* scunit_context_new(SCUnitError* error) {
    SCUnitContext* context = SCUNIT_MALLOC(sizeof(SCUnitContext));
    if (context == nullptr) {
        *error = SCUNIT_ERROR_OUT_OF_MEMORY;
        return nullptr;
    }
    context->result = SCUNIT_RESULT_PASS;
    context->size = INITIAL_BUFFER_SIZE;
    context->message = SCUNIT_CALLOC(INITIAL_BUFFER_SIZE, sizeof(char));
    if (context->message == nullptr) {
        SCUNIT_FREE(context);
        *error = SCUNIT_ERROR_OUT_OF_MEMORY;
        return nullptr;
    }
    *error = SCUNIT_ERROR_NONE;
    return context;
}

void scunit_context_reset(SCUnitContext* context) {
    context->result = SCUNIT_RESULT_PASS;
    context->message[0] = '\0';
}

SCUnitResult scunit_context_getResult(const SCUnitContext* context) {
    return context->result;
}

void scunit_context_setResult(SCUnitContext* context, SCUnitResult result, SCUnitError* error) {
    if ((result < SCUNIT_RESULT_PASS) || (result > SCUNIT_RESULT_FAIL)) {
        *error = SCUNIT_ERROR_ARGUMENT_OUT_OF_RANGE;
        return;
    }
    context->result = result;
    *error = SCUNIT_ERROR_NONE;
}

const char* scunit_context_getMessage(const SCUnitContext* context) {
    return context->message;
}

SCUnitError scunit_context_setMessage(SCUnitContext* context, const char* format, ...) {
    va_list args;
    va_start(args, format);
    SCUnitError error = scunit_vrsnprintf(&context->message, &context->size, format, args);
    va_end(args);
    return error;
}

SCUnitError scunit_context_setColoredMessage(
    SCUnitContext* context,
    SCUnitColor foreground,
    SCUnitColor background,
    const char* format,
    ...
) {
    va_list args;
    va_start(args, format);
    SCUnitError error = scunit_vrsnprintfc(
        &context->message,
        &context->size,
        foreground,
        background,
        format,
        args
    );
    va_end(args);
    return error;
}

SCUnitError scunit_context_appendMessage(SCUnitContext* context, const char* format, ...) {
    va_list args;
    va_start(args, format);
    SCUnitError error = scunit_vrasnprintf(&context->message, &context->size, format, args);
    va_end(args);
    return error;
}

SCUnitError scunit_context_appendColoredMessage(
    SCUnitContext* context,
    SCUnitColor foreground,
    SCUnitColor background,
    const char* format,
    ...
) {
    va_list args;
    va_start(args, format);
    SCUnitError error = scunit_vrasnprintfc(
        &context->message,
        &context->size,
        foreground,
        background,
        format,
        args
    );
    va_end(args);
    return error;
}

/**
 * @brief Reads characters from a given input stream into a dynamically resized buffer until a
 * newline (`\n`) or the end-of-file condition is reached.
 *
 * @note If a terminating newline is found, it is not included in the buffer.
 *
 * @warning This function assumes that the stream content is UTF-8 encoded and processes the input
 * accordingly. If the input contains invalid UTF-8 sequences, the behavior is undefined.
 *
 * If resizing `*buffer` fails due to an out-of-memory condition, both `*buffer` and
 * `*size` retain the state they were in before the failed operation and
 * `SCUNIT_ERROR_OUT_OF_MEMORY` is returned. Note that the content of the buffer pointed to by
 * `*buffer` is indeterminate in this case. Any characters read from `stream` in the meantime are
 * not recovered.
 *
 * If reading from `stream` fails for whatever reason, both `*buffer` and `*size` retain the state
 * they were in before the failed operation and `SCUNIT_ERROR_READING_STREAM_FAILED` is returned.
 * Note that the content of the buffer pointed to by `*buffer` is indeterminate in this case as well
 * and any characters read from `stream` in the meantime are not recovered.
 *
 * @param[in, out] stream    Input stream to read a single line from, which must be opened in text
 *                           mode. The stream's content is assumed to be UTF-8 encoded.
 * @param[in, out] buffer    Dynamically allocated buffer to write the line to (resized as
 *                           necessary).
 * @param[in, out] size      Size of the buffer (including the terminating `\0` byte). The size is
 *                           updated whenever `*buffer` is resized.
 * @param[out]     moreLines Whether more lines are available to be read.
 * @param[out]     error     `SCUNIT_ERROR_ARGUMENT_OUT_OF_RANGE` if `*size` is negative,
 *                           if `*buffer` is `nullptr` and `*size` is not equal to zero or if
 *                           `*buffer` is not `nullptr` and `*size` is equal to zero,
 *                           `SCUNIT_ERROR_OUT_OF_MEMORY` if resizing `*buffer` failed due to an
 *                           out-of-memory condition, `SCUNIT_ERROR_READING_STREAM_FAILED` if
 *                           reading from `stream` failed and `SCUNIT_ERROR_NONE` otherwise.
 */
static void readLine(
    FILE* stream,
    char** buffer,
    int64_t* size,
    bool* moreLines,
    SCUnitError* error
) {
    if ((*size < 0) || ((*buffer == nullptr) != (*size == 0))) {
        *error = SCUNIT_ERROR_ARGUMENT_OUT_OF_RANGE;
        return;
    }
    if (*buffer == nullptr) {
        int64_t newSize = INITIAL_BUFFER_SIZE;
        char* newBuffer = SCUNIT_CALLOC(newSize, sizeof(char));
        if (newBuffer == nullptr) {
            *error = SCUNIT_ERROR_OUT_OF_MEMORY;
            return;
        }
        *buffer = newBuffer;
        *size = newSize;
    }
    int64_t index = 0;
    int c;
    while (((c = fgetc(stream)) != EOF) && (c != '\n')) {
        // Subtract one from `*size` to account for the terminating `\0` byte.
        if (index >= (*size - 1)) {
            int64_t newSize = *size * GROWTH_FACTOR;
            char* newBuffer = SCUNIT_REALLOC(*buffer, newSize);
            if (newBuffer == nullptr) {
                *error = SCUNIT_ERROR_OUT_OF_MEMORY;
                return;
            }
            *buffer = newBuffer;
            *size = newSize;
        }
        (*buffer)[index++] = (char) c;
    }
    *moreLines = (c == '\n');
    if (ferror(stream)) {
        *error = SCUNIT_ERROR_READING_STREAM_FAILED;
        return;
    }
    (*buffer)[index] = '\0';
    *error = SCUNIT_ERROR_NONE;
}

void scunit_context_appendFileContext(
    SCUnitContext* context,
    const char* filename,
    int64_t line,
    SCUnitError* error
) {
    if (line < 1) {
        *error = SCUNIT_ERROR_ARGUMENT_OUT_OF_RANGE;
        return;
    }
    FILE* file = fopen(filename, "r");
    if (file == nullptr) {
        *error = SCUNIT_ERROR_OPENING_STREAM_FAILED;
        return;
    }
    *error = SCUNIT_ERROR_NONE;
    // The relevant line might be at the very beginning of the file, in which case we cannot print
    // enough context lines before it. We simply fallback to using the very first line of the file
    // as the first context line.
    int64_t firstContextLine = (line > CONTEXT_LINES) ? line - CONTEXT_LINES : 1;
    int64_t lastContextLine = line + CONTEXT_LINES;
    // Line numbers are right-aligned for better readability. We therefore need to know how wide the
    // largest line number (in the last line) is to be able to do the formatting correctly.
    int64_t maxLineNumberWidth = ((int64_t) log10(lastContextLine)) + 1;
    char* buffer = nullptr;
    int64_t size = 0;
    bool moreLines = true;
    for (int64_t i = 1; (i <= lastContextLine) && moreLines; i++) {
        readLine(file, &buffer, &size, &moreLines, error);
        if (*error != SCUNIT_ERROR_NONE) {
            goto failed;
        }
        // Skip all irrelevant lines before the first context line.
        if (i < firstContextLine) {
            continue;
        }
        *error = scunit_rasnprintfc(
            &context->message,
            &context->size,
            SCUNIT_COLOR_DARK_CYAN,
            SCUNIT_COLOR_DARK_DEFAULT,
            "  %*d",
            maxLineNumberWidth,
            i
        );
        if (*error != SCUNIT_ERROR_NONE) {
            goto failed;
        }
        *error = scunit_rasnprintf(&context->message, &context->size, " | ", maxLineNumberWidth, i);
        if (*error != SCUNIT_ERROR_NONE) {
            goto failed;
        }
        *error = scunit_rasnprintfc(
            &context->message,
            &context->size,
            (i == line) ? SCUNIT_COLOR_DARK_RED : SCUNIT_COLOR_DARK_DEFAULT,
            SCUNIT_COLOR_DARK_DEFAULT,
            "%s\n",
            buffer
        );
        if (*error != SCUNIT_ERROR_NONE) {
            goto failed;
        }
    }
    if (fclose(file) == EOF) {
        *error = SCUNIT_ERROR_CLOSING_STREAM_FAILED;
    }
    SCUNIT_FREE(buffer);
    return;
failed:
    // Note that closing the file might fail, but we only care about the error that occurred first.
    fclose(file);
    SCUNIT_FREE(buffer);
}

void scunit_context_free(SCUnitContext* context) {
    if (context != nullptr) {
        SCUNIT_FREE(context->message);
        SCUNIT_FREE(context);
    }
}