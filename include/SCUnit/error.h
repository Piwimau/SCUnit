#ifndef SCUNIT_ERROR_H
#define SCUNIT_ERROR_H

/**
 * @brief Represents an enumeration of error codes used to indicate various success or failure
 * conditions.
 */
typedef enum SCUnitError {

    /** @brief Indicates that an operation was successful (i. e. no error occurred). */
    SCUNIT_ERROR_NONE,

    /**
     * @brief Indicates that one of the arguments of a function was outside the range of accepted
     * values.
     *
     * @note This should not occur under normal circumstances. It is usually a sign of a serious
     * programming error.
     */
    SCUNIT_ERROR_ARGUMENT_OUT_OF_RANGE,

    /** @brief Indicates that a memory allocation failed due to an out-of-memory condition. */
    SCUNIT_ERROR_OUT_OF_MEMORY,

    /** @brief Indicates that opening a stream failed. */
    SCUNIT_ERROR_OPENING_STREAM_FAILED,

    /** @brief Indicates that reading from a stream failed. */
    SCUNIT_ERROR_READING_STREAM_FAILED,

    /** @brief Indicates that writing to a stream failed. */
    SCUNIT_ERROR_WRITING_STREAM_FAILED,

    /** @brief Indicates that writing to a buffer failed. */
    SCUNIT_ERROR_WRITING_BUFFER_FAILED,

    /** @brief Indicates that closing a stream failed. */
    SCUNIT_ERROR_CLOSING_STREAM_FAILED,

    /**
     * @brief Indicates that (re-)starting or stopping an `SCUnitTimer` failed.
     *
     * @note See the documentation in `<SCUnit/timer.h>` to find out why this error may have
     * occurred.
     */
    SCUNIT_ERROR_TIMER_FAILED,

    /**
     * @brief Indicates that an `SCUnitTimer` is already or still running.
     *
     * @note See the documentation in `<SCUnit/timer.h>` to find out why this error may have
     * occurred. It is usually a sign of a serious programming error.
     */
    SCUNIT_ERROR_TIMER_RUNNING,

    /**
     * @brief Indicates that an `SCUnitTimer` is not running yet or anymore.
     *
     * @note See the documentation in `<SCUnit/timer.h>` to find out why this error may have
     * occurred. It is usually a sign of a serious programming error.
     */
    SCUNIT_ERROR_TIMER_NOT_RUNNING

} SCUnitError;

#endif