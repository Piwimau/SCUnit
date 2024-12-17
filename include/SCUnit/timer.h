/**
 * @file timer.h
 *
 * @brief A simple timer for measuring the execution time of a block of code.
 *
 * @details The `SCUnitTimer` is designed to be a general purpose, easy-to-use timer to get a rough
 * estimate for the execution time of a block of code. It is not intended to be a sophisticated,
 * high-resolution timer suitable for microbenchmarking.
 *
 * To measure the execution time of a function or a block of code, first construct a new
 * `SCUnitTimer` using `scunit_timer_new()` and start timing with `scunit_timer_start()`. Note that
 * repeated calls to `scunit_timer_start()` with an already running timer are guaranteed to fail.
 * If you wish to restart measuring at the current point in time, use `scunit_timer_restart()`
 * instead.
 *
 * Before an `SCUnitTimer` can be queried for the elapsed time, it must be stopped with a call to
 * `scunit_timer_stop()`. After that, both the elapsed wall and CPU time can be queried using
 * `scunit_timer_wallTime()` and `scunit_timer_cpuTime()` respectively. The difference is the
 * following:
 *
 * - Wall time (also known as real or wall-clock time) measures the actual time it took for a
 *   calculation to run from start to finish. This is useful for evaluating the overall performance
 *   or usability of an application.
 *
 * - CPU time only measures the time the processor is actively working on the calculation. As a
 *   result, CPU time may be considerably less than wall time if the process had to wait for system
 *   resources or due to unlucky scheduling. However, CPU time may also be considerably larger than
 *   wall time, as is often the case when running multi-threaded applications on a multi-core
 *   processor. For example, if an application uses two threads to perform a calculation that takes
 *   one second to complete, the wall time would be around one second, while the CPU time might be
 *   as much as two seconds if the threads were run in parallel on two different cores.
 *
 * Both `scunit_timer_wallTime()` and `scunit_timer_cpuTime()` return the elapsed time as a double
 * precision floating-point number. The `SCUnitTimer` automatically determines an appropriate
 * `SCUnitTimeUnit' for this value (based on certain internal thresholds).
 *
 * Finally, a call to `scunit_timer_free()` is required to deallocate the resources.
 *
 * @note The `SCUnitTimer` is implemented internally using the POSIX function
 * [`clock_gettime()`](https://man7.org/linux/man-pages/man3/clock_gettime.3.html), which might not
 * be available on all platforms (especially on Windows/MSVC, although I can highly recommend
 * [MSYS2](https://www.msys2.org/) as a solution). This is mainly due to the lack of other portable
 * options in the C standard library:
 *
 * - [`clock()`](https://en.cppreference.com/w/c/chrono/clock) from `<time.h>` is supposed to return
 *   the approximate CPU time. However, it actually returns the wall time on Windows and so it
 *   cannot be used reliably for at one of the two different measured times.
 *
 * - [`timespec_get()`](https://en.cppreference.com/w/c/chrono/timespec_get) is part of the standard
 *   since `C11` and also available via `<time.h>`, but this function can only be used to calculate
 *   the wall time.
 *
 * If `clock_gettime()` is not available on your platform and you are comfortable with using one of
 * the many platform-specific options (e. g.
 * [`QueryPerformanceCounter()`](https://learn.microsoft.com/en-us/windows/win32/api/profileapi/) on
 * Windows), you could also switch out the internal implementation of the `SCUnitTimer`.
 *
 * @warning An `SCUnitTimer` is not thread-safe. Concurrent access to a single timer instance by
 * multiple threads may result in undefined behavior. It is your responsibility to ensure that any
 * access is properly synchronized when used in a multi-threaded context.
 *
 * @author Philipp Raschka
 */

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
 * @param[out] isRunning `True` if the given `SCUnitTimer` is running, otherwise `false`.
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
SCUnitError scunit_timer_wallTime(const SCUnitTimer* timer, SCUnitMeasurement* wallTimeMeasurement);

/**
 * @brief Returns the elapsed CPU time measured by a given `SCUnitTimer`.
 *
 * @param[in]  timer              `SCUnitTimer` to examine.
 * @param[out] cpuTimeMeasurement `SCUnitMeasurement` for the elapsed CPU time.
 * @return `SCUNIT_ERROR_ARGUMENT_NULL` if `timer` or `cpuTimeMeasurement` is `nullptr`,
 * `SCUNIT_ERROR_TIMER_RUNNING` if `timer` is still running and `SCUNIT_ERROR_NONE` otherwise.
 */
SCUnitError scunit_timer_cpuTime(const SCUnitTimer* timer, SCUnitMeasurement* cpuTimeMeasurement);

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