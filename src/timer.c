#include <stdint.h>
#include <time.h>
#include <SCUnit/memory.h>
#include <SCUnit/timer.h>

/** @brief Represents a time interval broken down into seconds and nanoseconds. */
typedef struct timespec SCUnitTimespec;

struct SCUnitTimer {

    /** @brief Start point of measuring the elapsed wall time (in seconds). */
    double wallTimeStartSeconds;

    /** @brief End point of measuring the elapsed wall time (in seconds). */
    double wallTimeEndSeconds;

    /** @brief Start point of measuring the elapsed CPU time (in seconds). */
    double cpuTimeStartSeconds;

    /** @brief End point of measuring the elapsed CPU time (in seconds). */
    double cpuTimeEndSeconds;

    /** @brief Whether this `SCUnitTimer` is currently running. */
    bool isRunning;

};

/** @brief Number of nanoseconds in a single second (10^9 ns = 1 s). */
static constexpr int32_t NANOSECONDS_PER_SECOND = 1'000'000'000;

/** @brief Number of microseconds in a single second (10^6 us = 1 s). */
static constexpr int32_t MICROSECONDS_PER_SECOND = 1'000'000;

/** @brief Number of milliseconds in a single second (10^3 ns = 1 s). */
static constexpr int32_t MILLISECONDS_PER_SECOND = 1000;

/** @brief Number of seconds in a single minute (60 s = 1 min). */
static constexpr int32_t SECONDS_PER_MINUTE = 60;

/** @brief Number of seconds in a single hour (3600 s = 1 h). */
static constexpr int32_t SECONDS_PER_HOUR = 3600;

/** @brief String representations of the different time units. */
static const char* const TIME_UNIT_STRINGS[SCUNIT_TIME_UNIT_HOURS + 1] = {
    [SCUNIT_TIME_UNIT_NANOSECONDS] = "ns",
    [SCUNIT_TIME_UNIT_MICROSECONDS] = "us",
    [SCUNIT_TIME_UNIT_MILLISECONDS] = "ms",
    [SCUNIT_TIME_UNIT_SECONDS] = "s",
    [SCUNIT_TIME_UNIT_MINUTES] = "min",
    [SCUNIT_TIME_UNIT_HOURS] = "h"
};

SCUnitTimer* scunit_timer_new(SCUnitError* error) {
    SCUnitTimer* timer = SCUNIT_MALLOC(sizeof(SCUnitTimer));
    if (timer == nullptr) {
        *error = SCUNIT_ERROR_OUT_OF_MEMORY;
        return nullptr;
    }
    *timer = (SCUnitTimer) { };
    *error = SCUNIT_ERROR_NONE;
    return timer;
}

/**
 * @brief Converts a given `SCUnitTimespec` to seconds.
 *
 * @attention This function is for internal purposes only and does not validate `timespec`.
 *
 * @param[in] timespec `SCUnitTimespec` to convert to seconds.
 * @return The equivalent number of seconds represented by the given `SCUnitTimespec`.
 */
static inline double timespecToSeconds(SCUnitTimespec timespec) {
    return ((double) timespec.tv_sec) + (((double) timespec.tv_nsec) / NANOSECONDS_PER_SECOND);
}

void scunit_timer_start(SCUnitTimer* timer, SCUnitError* error) {
    if (timer->isRunning) {
        *error = SCUNIT_ERROR_TIMER_RUNNING;
        return;
    }
    SCUnitTimespec wallTimeStart;
    SCUnitTimespec cpuTimeStart;
    if ((clock_gettime(CLOCK_MONOTONIC, &wallTimeStart) < 0)
            || (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &cpuTimeStart) < 0)) {
        *error = SCUNIT_ERROR_TIMER_FAILED;
        return;
    }
    timer->wallTimeStartSeconds = timespecToSeconds(wallTimeStart);
    timer->cpuTimeStartSeconds = timespecToSeconds(cpuTimeStart);
    timer->isRunning = true;
    *error = SCUNIT_ERROR_NONE;
}

void scunit_timer_restart(SCUnitTimer* timer, SCUnitError* error) {
    if (!timer->isRunning) {
        *error = SCUNIT_ERROR_TIMER_NOT_RUNNING;
        return;
    }
    SCUnitTimespec wallTimeStart;
    SCUnitTimespec cpuTimeStart;
    if ((clock_gettime(CLOCK_MONOTONIC, &wallTimeStart) < 0)
            || (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &cpuTimeStart) < 0)) {
        *error = SCUNIT_ERROR_TIMER_FAILED;
        return;
    }
    timer->wallTimeStartSeconds = timespecToSeconds(wallTimeStart);
    timer->cpuTimeStartSeconds = timespecToSeconds(cpuTimeStart);
    *error = SCUNIT_ERROR_NONE;
}

void scunit_timer_stop(SCUnitTimer* timer, SCUnitError* error) {
    if (!timer->isRunning) {
        *error = SCUNIT_ERROR_TIMER_NOT_RUNNING;
        return;
    }
    SCUnitTimespec wallTimeEnd;
    SCUnitTimespec cpuTimeEnd;
    if ((clock_gettime(CLOCK_MONOTONIC, &wallTimeEnd) < 0)
            || (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &cpuTimeEnd) < 0)) {
        *error = SCUNIT_ERROR_TIMER_FAILED;
        return;
    }
    timer->wallTimeEndSeconds = timespecToSeconds(wallTimeEnd);
    timer->cpuTimeEndSeconds = timespecToSeconds(cpuTimeEnd);
    timer->isRunning = false;
    *error = SCUNIT_ERROR_NONE;
}

bool scunit_timer_isRunning(const SCUnitTimer* timer) {
    return timer->isRunning;
}

/**
 * @brief Adjusts an `SCUnitMeasurement` for an elapsed time in seconds by converting it to an
 * appropriate `SCUnitTimeUnit`.
 *
 * @param[in, out] elapsedTimeMeasurement `SCUnitMeasurement` to adjust.
 */
static inline void adjustMeasurement(SCUnitMeasurement* elapsedTimeMeasurement) {
    if (elapsedTimeMeasurement->time < (1.0 / MICROSECONDS_PER_SECOND)) {
        elapsedTimeMeasurement->time *= NANOSECONDS_PER_SECOND;
        elapsedTimeMeasurement->timeUnit = SCUNIT_TIME_UNIT_NANOSECONDS;
    }
    else if (elapsedTimeMeasurement->time < (1.0 / MILLISECONDS_PER_SECOND)) {
        elapsedTimeMeasurement->time *= MICROSECONDS_PER_SECOND;
        elapsedTimeMeasurement->timeUnit = SCUNIT_TIME_UNIT_MICROSECONDS;
    }
    else if (elapsedTimeMeasurement->time < 1.0) {
        elapsedTimeMeasurement->time *= MILLISECONDS_PER_SECOND;
        elapsedTimeMeasurement->timeUnit = SCUNIT_TIME_UNIT_MILLISECONDS;
    }
    else if (elapsedTimeMeasurement->time < SECONDS_PER_MINUTE) {
        elapsedTimeMeasurement->timeUnit = SCUNIT_TIME_UNIT_SECONDS;
    }
    else if (elapsedTimeMeasurement->time < SECONDS_PER_HOUR) {
        elapsedTimeMeasurement->time /= SECONDS_PER_MINUTE;
        elapsedTimeMeasurement->timeUnit = SCUNIT_TIME_UNIT_MINUTES;
    }
    else {
        elapsedTimeMeasurement->time /= SECONDS_PER_HOUR;
        elapsedTimeMeasurement->timeUnit = SCUNIT_TIME_UNIT_HOURS;
    }
    elapsedTimeMeasurement->timeUnitString = TIME_UNIT_STRINGS[elapsedTimeMeasurement->timeUnit];
}

SCUnitMeasurement scunit_timer_getWallTime(const SCUnitTimer* timer, SCUnitError* error) {
    if (timer->isRunning) {
        *error = SCUNIT_ERROR_TIMER_RUNNING;
        return (SCUnitMeasurement) {
            .time = 0.0,
            .timeUnit = SCUNIT_TIME_UNIT_NANOSECONDS,
            .timeUnitString = TIME_UNIT_STRINGS[SCUNIT_TIME_UNIT_NANOSECONDS]
        };
    }
    SCUnitMeasurement wallTimeMeasurement = {
        .time = timer->wallTimeEndSeconds - timer->wallTimeStartSeconds
    };
    adjustMeasurement(&wallTimeMeasurement);
    *error = SCUNIT_ERROR_NONE;
    return wallTimeMeasurement;
}

SCUnitMeasurement scunit_timer_getCPUTime(const SCUnitTimer* timer, SCUnitError* error) {
    if (timer->isRunning) {
        *error = SCUNIT_ERROR_TIMER_RUNNING;
        return (SCUnitMeasurement) {
            .time = 0.0,
            .timeUnit = SCUNIT_TIME_UNIT_NANOSECONDS,
            .timeUnitString = TIME_UNIT_STRINGS[SCUNIT_TIME_UNIT_NANOSECONDS]
        };
    }
    SCUnitMeasurement cpuTimeMeasurement = {
        .time = timer->cpuTimeEndSeconds - timer->cpuTimeStartSeconds
    };
    adjustMeasurement(&cpuTimeMeasurement);
    *error = SCUNIT_ERROR_NONE;
    return cpuTimeMeasurement;
}

void scunit_timer_free(SCUnitTimer* timer) {
    SCUNIT_FREE(timer);
}