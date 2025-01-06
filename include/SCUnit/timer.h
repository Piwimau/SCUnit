#ifndef SCUNIT_TIMER_H
#define SCUNIT_TIMER_H

#include <SCUnit/error.h>

/** @brief Represents a simple timer for measuring the execution time of a block of code. */
typedef struct SCUnitTimer SCUnitTimer;

/** @brief Represents an enumeration of the different time units returned by an `SCUnitTimer`. */
typedef enum SCUnitTimeUnit {

    /** @brief Indicates that elapsed time is measured in nanoseconds (1 ns = 10^(-9) seconds). */
    SCUNIT_TIME_UNIT_NANOSECONDS,

    /** @brief Indicates that elapsed time is measured in microseconds (1 us = 10^(-6) seconds). */
    SCUNIT_TIME_UNIT_MICROSECONDS,

    /** @brief Indicates that elapsed time is measured in milliseconds (1 ms = 10^(-3) seconds). */
    SCUNIT_TIME_UNIT_MILLISECONDS,

    /** @brief Indicates that elapsed time is measured in seconds (1 s = 1 second). */
    SCUNIT_TIME_UNIT_SECONDS,

    /** @brief Indicates that elapsed time is measured in minutes (1 min = 60 seconds). */
    SCUNIT_TIME_UNIT_MINUTES,

    /** @brief Indicates that elapsed time is measured in hours (1 h = 3600 seconds). */
    SCUNIT_TIME_UNIT_HOURS

} SCUnitTimeUnit;

/** @brief Represents a simple measurement of some elapsed time. */
typedef struct SCUnitMeasurement {

    /**
     * @brief Elapsed time of the `SCUnitMeasurement`.
     *
     * @note The corresponding time unit is indicated by the `timeUnit` member.
     */
    double time;

    /**
     * @brief Time unit of the `SCUnitMeasurement` as a string that can be used for printing.
     *
     * @warning This string is stored in static memory and must not be modified nor deallocated.
     */
    const char* timeUnitString;

    /**
     * @brief Time unit of the `SCUnitMeasurement`.
     *
     * @note Use the `timeUnitString` member when you want to print the time unit.
     */
    SCUnitTimeUnit timeUnit;

} SCUnitMeasurement;

/**
 * @brief Initializes a new `SCUnitTimer`.
 *
 * @param[out] timer A new initialized `SCUnitTimer`.
 * @return `SCUNIT_ERROR_ARGUMENT_NULL` if `timer` is `nullptr`, `SCUNIT_ERROR_OUT_OF_MEMORY` if an
 * out-of-memory condition occurred and `SCUNIT_ERROR_NONE` otherwise.
 */
SCUnitError scunit_timer_new(SCUnitTimer** timer);

/**
 * @brief Starts measuring time using a given `SCUnitTimer`.
 *
 * @attention An already running timer (see `scunit_timer_isRunning()`) cannot be started again.
 * If you wish to restart measuring at the current point in time, use `scunit_timer_restart()`
 * instead.
 *
 * @param[in, out] timer `SCUnitTimer` to start measuring time with.
 * @return `SCUNIT_ERROR_ARGUMENT_NULL` if `timer` is `nullptr`, `SCUNIT_ERROR_TIMER_RUNNING` if
 * `timer` is already running, `SCUNIT_ERROR_TIMER_FAILED` if starting `timer` failed and
 * `SCUNIT_ERROR_NONE` otherwise.
 */
SCUnitError scunit_timer_start(SCUnitTimer* timer);

/**
 * @brief Restarts measuring time using a given `SCUnitTimer`.
 *
 * @attention An `SCUnitTimer` must already be running in order to to restart it.
 *
 * @param[in, out] timer `SCUnitTimer` to restart measuring time with.
 * @return `SCUNIT_ERROR_ARGUMENT_NULL` if `timer` is `nullptr`, `SCUNIT_ERROR_TIMER_NOT_RUNNING` if
 * `timer` is not running already, `SCUNIT_ERROR_TIMER_FAILED` if restarting `timer` failed and
 * `SCUNIT_ERROR_NONE` otherwise.
 */
SCUnitError scunit_timer_restart(SCUnitTimer* timer);

/**
 * @brief Stops measuring time using a given `SCUnitTimer`.
 *
 * @param[in, out] timer `SCUnitTimer` to stop measuring time with.
 * @return `SCUNIT_ERROR_ARGUMENT_NULL` if `timer` is `nullptr`, `SCUNIT_ERROR_TIMER_NOT_RUNNING` if
 * `timer` is not running, `SCUNIT_ERROR_TIMER_FAILED` if stopping `timer` failed and
 * `SCUNIT_ERROR_NONE` otherwise.
 */
SCUnitError scunit_timer_stop(SCUnitTimer* timer);

/**
 * @brief Determines if a given `SCUnitTimer` is running.
 *
 * @param[in]  timer     `SCUnitTimer` to examine.
 * @param[out] isRunning `true` if the given `SCUnitTimer` is running, otherwise `false`.
 * @return `SCUNIT_ERROR_ARGUMENT_NULL` if `timer` or `isRunning` is `nullptr` and
 * `SCUNIT_ERROR_NONE` otherwise.
 */
SCUnitError scunit_timer_isRunning(const SCUnitTimer* timer, bool* isRunning);

/**
 * @brief Returns the elapsed wall time measured by a given `SCUnitTimer`.
 *
 * @param[in]  timer               `SCUnitTimer` to examine.
 * @param[out] wallTimeMeasurement `SCUnitMeasurement` for the elapsed wall time.
 * @return `SCUNIT_ERROR_ARGUMENT_NULL` if `timer` or `wallTimeMeasurement` is `nullptr`,
 * `SCUNIT_ERROR_TIMER_RUNNING` if `timer` is still running and `SCUNIT_ERROR_NONE` otherwise.
 */
SCUnitError scunit_timer_getWallTime(
    const SCUnitTimer* timer,
    SCUnitMeasurement* wallTimeMeasurement
);

/**
 * @brief Returns the elapsed CPU time measured by a given `SCUnitTimer`.
 *
 * @param[in]  timer              `SCUnitTimer` to examine.
 * @param[out] cpuTimeMeasurement `SCUnitMeasurement` for the elapsed CPU time.
 * @return `SCUNIT_ERROR_ARGUMENT_NULL` if `timer` or `cpuTimeMeasurement` is `nullptr`,
 * `SCUNIT_ERROR_TIMER_RUNNING` if `timer` is still running and `SCUNIT_ERROR_NONE` otherwise.
 */
SCUnitError scunit_timer_getCPUTime(
    const SCUnitTimer* timer,
    SCUnitMeasurement* cpuTimeMeasurement
);

/**
 * @brief Deallocates any remaining resources of a given `SCUnitTimer`.
 *
 * @note It is allowed to deallocate a non-existing `SCUnitTimer`, i. e. `*timer` may be `nullptr`,
 * but `timer` itself is not allowed to be `nullptr`.
 *
 * @param[in, out] timer `SCUnitTimer` to deallocate the resources of.
 * @return `SCUNIT_ERROR_ARGUMENT_NULL` if `timer` is `nullptr` and `SCUNIT_ERROR_NONE` otherwise.
 */
SCUnitError scunit_timer_free(SCUnitTimer** timer);

#endif