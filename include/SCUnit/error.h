/**
 * @file error.h
 *
 * @brief This header provides the `SCUnitError` enumeration used as a standardized error handling
 * mechanism throughout SCUnit.
 *
 * @details Functions in the SCUnit API typically return an `SCUnitError` value to indicate various
 * success or failure conditions. You are generally expected to check the return value of such
 * functions unless you are absolutely certain that the documented errors cannot possibly occur.
 *
 * @author Philipp Raschka
 */

#ifndef SCUNIT_ERROR_H
#define SCUNIT_ERROR_H

/**
 * @brief Represents an enumeration of error codes used by SCUnit to indicate various success or
 * failure conditions.
 */
typedef enum SCUnitError {

    /** @brief Indicates that an operation was successful (i. e. no error occurred). */
    SCUNIT_ERROR_NONE,

    /**
     * @brief Indicates that one of the required arguments of a function was `nullptr`.
     *
     * @note This should not occur under normal circumstances. It is usually a sign of a serious
     * programming error.
     */
    SCUNIT_ERROR_ARGUMENT_NULL,

    /**
     * @brief Indicates that one of the arguments of a function was out of range.
     *
     * @note This should not occur under normal circumstances. It is usually a sign of a serious
     * programming error.
     */
    SCUNIT_ERROR_ARGUMENT_OUT_OF_RANGE,

    /** @brief Indicates that a memory allocation failed due to an out-of-memory condition. */
    SCUNIT_ERROR_OUT_OF_MEMORY,

    /** @brief Indicates that opening a stream (e. g. a file) failed. */
    SCUNIT_ERROR_OPENING_STREAM_FAILED,

    /** @brief Indicates that reading from a stream (e. g. a file) failed. */
    SCUNIT_ERROR_READING_STREAM_FAILED,

    /** @brief Indicates that writing to a stream (e. g. a file) failed. */
    SCUNIT_ERROR_WRITING_STREAM_FAILED,

    /** @brief Indicates that writing to a buffer failed. */
    SCUNIT_ERROR_WRITING_BUFFER_FAILED,

    /** @brief Indicates that closing a stream (e. g. a file) failed. */
    SCUNIT_ERROR_CLOSING_STREAM_FAILED,

    /**
     * @brief Indicates that (re-)starting or stopping an `SCUnitTimer` failed.
     *
     * @note See the documentation in `<SCUnit/timer.h>` to find out why this error could have
     * occurred.
     */
    SCUNIT_ERROR_TIMER_FAILED,

    /**
     * @brief Indicates that an `SCUnitTimer` is already or still running.
     *
     * @note See the documentation in `<SCUnit/timer.h>` to find out why this error could have
     * occurred. It is usually a sign of a serious programming error.
     */
    SCUNIT_ERROR_TIMER_RUNNING,

    /**
     * @brief Indicates that an `SCUnitTimer` is not running (yet or anymore).
     *
     * @note See the documentation in `<SCUnit/timer.h>` to find out why this error could have
     * occurred. It is usually a sign of a serious programming error.
     */
    SCUNIT_ERROR_TIMER_NOT_RUNNING,

    /**
     * @brief Indicates that an unknown `SCUnitResult` was encountered in a test.
     *
     * @note This should not occur under normal circumstances. It is usually a sign of a serious
     * programming error.
     */
    SCUNIT_ERROR_UNKNOWN_RESULT

} SCUnitError;

#endif