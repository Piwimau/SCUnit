/**
 * @file suite.c
 *
 * @brief Implementation of a suite to group logically related tests together.
 *
 * @note See the corresponding header file `<SCUnit/suite.h>` for more information on the interface,
 * intended usage and some general implementation notes.
 *
 * @author Philipp Raschka
 */

#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <string.h>
#include <SCUnit/memory.h>
#include <SCUnit/print.h>
#include <SCUnit/suite.h>
#include <SCUnit/timer.h>

/** @brief Represents a simple test which is part of an `SCUnitSuite`. */
typedef struct SCUnitTest {

    /**
     * @brief Name of this `SCUnitTest`.
     *
     * @note This is a dynamically allocated string managed by the corresponding `SCUnitSuite`.
     */
    char* name;

    /** @brief Test function to be executed. */
    SCUnitTestFunction testFunction;

} SCUnitTest;

struct SCUnitSuite {

    /**
     * @brief Name of this `SCUnitSuite`.
     *
     * @note This is a dynamically allocated string managed by this `SCUnitSuite`.
     */
    char* name;

    /**
     * @brief Optional suite setup function to run before executing all tests in this `SCUnitSuite`.
     *
     * @note Each `SCUnitSuite` can only have one `SCUnitSetup` function. If equal to `nullptr`,
     * nothing is registered to be run.
     */
    SCUnitSetup setup;

    /**
     * @brief Optional suite teardown function to run after executing all tests in this
     * `SCUnitSuite`.
     *
     * @note Each `SCUnitSuite` can only have one `SCUnitTeardown` function. If equal to `nullptr`,
     * nothing is registered to be run.
     */
    SCUnitTeardown teardown;

    /**
     * @brief Optional test setup function to run before each individual test in this `SCUnitSuite`.
     *
     * @note Each `SCUnitSuite` can only have one `SCUnitTestSetup` function. If equal to `nullptr`,
     * nothing is registered to be run.
     */
    SCUnitTestSetup testSetup;

    /**
     * @brief Optional test teardown function to run after each individual test in this
     * `SCUnitSuite`.
     *
     * @note Each `SCUnitSuite` can only have one `SCUnitTestTeardown` function. If equal to
     * `nullptr`, nothing is registered to be run.
     */
    SCUnitTestTeardown testTeardown;

    /**
     * @brief Tests to be executed as part of this `SCUnitSuite`.
     *
     * @note This is a dynamically resized array with storage for `capacity` elements and
     * `registeredTests` registered tests.
     */
    SCUnitTest* tests;

    /** @brief Capacity of this `SCUnitSuite` for registering tests. */
    int64_t capacity;

    /** @brief Number of registered tests in this `SCUnitSuite`. */
    int64_t registeredTests;

};

/** @brief Growth factor used for resizing the array of tests. */
static constexpr int64_t GROWTH_FACTOR = 2;

SCUnitError scunit_suite_new(const char* name, SCUnitSuite** suite) {
    if ((name == nullptr) || (suite == nullptr)) {
        return SCUNIT_ERROR_ARGUMENT_NULL;
    }
    *suite = SCUNIT_CALLOC(1, sizeof(SCUnitSuite));
    if (*suite == nullptr) {
        return SCUNIT_ERROR_OUT_OF_MEMORY;
    }
    (*suite)->name = strdup(name);
    if ((*suite)->name == nullptr) {
        SCUNIT_FREE(*suite);
        *suite = nullptr;
        return SCUNIT_ERROR_OUT_OF_MEMORY;
    }
    return SCUNIT_ERROR_NONE;
}

SCUnitError scunit_suite_name(const SCUnitSuite* suite, const char** name) {
    if ((suite == nullptr) || (name == nullptr)) {
        return SCUNIT_ERROR_ARGUMENT_NULL;
    }
    *name = suite->name;
    return SCUNIT_ERROR_NONE;
}

SCUnitError scunit_suite_registerSetup(SCUnitSuite* suite, SCUnitSetup setup) {
    if (suite == nullptr) {
        return SCUNIT_ERROR_ARGUMENT_NULL;
    }
    suite->setup = setup;
    return SCUNIT_ERROR_NONE;
}

SCUnitError scunit_suite_registerTeardown(SCUnitSuite* suite, SCUnitTeardown teardown) {
    if (suite == nullptr) {
        return SCUNIT_ERROR_ARGUMENT_NULL;
    }
    suite->teardown = teardown;
    return SCUNIT_ERROR_NONE;
}

SCUnitError scunit_suite_registerTestSetup(SCUnitSuite* suite, SCUnitTestSetup testSetup) {
    if (suite == nullptr) {
        return SCUNIT_ERROR_ARGUMENT_NULL;
    }
    suite->testSetup = testSetup;
    return SCUNIT_ERROR_NONE;
}

SCUnitError scunit_suite_setTestTeardown(SCUnitSuite* suite, SCUnitTestTeardown testTeardown) {
    if (suite == nullptr) {
        return SCUNIT_ERROR_ARGUMENT_NULL;
    }
    suite->testTeardown = testTeardown;
    return SCUNIT_ERROR_NONE;
}

SCUnitError scunit_suite_registerTest(
    SCUnitSuite* suite,
    const char* name,
    SCUnitTestFunction testFunction
) {
    if ((suite == nullptr) || (name == nullptr) || (testFunction == nullptr)) {
        return SCUNIT_ERROR_ARGUMENT_NULL;
    }
    if (suite->registeredTests >= suite->capacity) {
        // Add one for the edge case of `suite->capacity` being zero (which is the case initially).
        int64_t newCapacity = (suite->capacity * GROWTH_FACTOR) + 1;
        SCUnitTest* newTests = SCUNIT_REALLOC(suite->tests, newCapacity * sizeof(SCUnitTest));
        if (newTests == nullptr) {
            return SCUNIT_ERROR_OUT_OF_MEMORY;
        }
        suite->tests = newTests;
        suite->capacity = newCapacity;
    }
    char* nameCopy = strdup(name);
    if (nameCopy == nullptr) {
        return SCUNIT_ERROR_OUT_OF_MEMORY;
    }
    suite->tests[suite->registeredTests++] = (SCUnitTest) {
        .name = nameCopy,
        .testFunction = testFunction
    };
    return SCUNIT_ERROR_NONE;
}

SCUnitError scunit_suite_run(const SCUnitSuite* suite, SCUnitSummary* summary) {
    if ((suite == nullptr) || (summary == nullptr)) {
        return SCUNIT_ERROR_ARGUMENT_NULL;
    }
    *summary = (SCUnitSummary) { };
    scunit_printf("--- Suite ");
    scunit_printfc(SCUNIT_COLOR_DARK_CYAN, SCUNIT_COLOR_DARK_DEFAULT, "%s", suite->name);
    scunit_printf(" ---\n\n");
    SCUnitTimer* suiteTimer;
    SCUnitError error = scunit_timer_new(&suiteTimer);
    if (error != SCUNIT_ERROR_NONE) {
        goto fail;
    }
    SCUnitTimer* testTimer;
    error = scunit_timer_new(&testTimer);
    if (error != SCUNIT_ERROR_NONE) {
        goto fail;
    }
    SCUnitContext* context;
    error = scunit_context_new(&context);
    if (error != SCUNIT_ERROR_NONE) {
        goto fail;
    }
    error = scunit_timer_start(suiteTimer);
    if (error != SCUNIT_ERROR_NONE) {
        goto fail;
    }
    if (suite->setup != nullptr) {
        suite->setup();
    }
    for (int64_t i = 0; i < suite->registeredTests; i++) {
        if (suite->testSetup != nullptr) {
            suite->testSetup();
        }
        // Run tests in the order they are originally defined in, which is apparently reversed in
        // relation to how they are registered using `[[gnu::constructor]]`. Note that SCUnit does
        // not guarantee the order in which tests or suites are run anywhere, so we are free to
        // implement this however we want.
        const SCUnitTest* test = &suite->tests[suite->registeredTests - 1 - i];
        scunit_printf("(%" PRId64 "/%" PRId64 ") Running test ", i + 1, suite->registeredTests);
        scunit_printfc(SCUNIT_COLOR_DARK_CYAN, SCUNIT_COLOR_DARK_DEFAULT, "%s", test->name);
        scunit_printf("... ");
        // Reuse the context for every test to avoid unnecessary memory allocations.
        scunit_context_reset(context);
        error = scunit_timer_start(testTimer);
        if (error != SCUNIT_ERROR_NONE) {
            goto fail;
        }
        test->testFunction(context);
        error = scunit_timer_stop(testTimer);
        if (error != SCUNIT_ERROR_NONE) {
            goto fail;
        }
        SCUnitResult result;
        scunit_context_getResult(context, &result);
        switch (result) {
            case SCUNIT_RESULT_PASS:
                scunit_printfc(SCUNIT_COLOR_DARK_BLACK, SCUNIT_COLOR_DARK_GREEN, " PASS ");
                summary->passedTests++;
                break;
            case SCUNIT_RESULT_SKIP:
                scunit_printfc(SCUNIT_COLOR_DARK_BLACK, SCUNIT_COLOR_DARK_YELLOW, " SKIP ");
                summary->skippedTests++;
                break;
            case SCUNIT_RESULT_FAIL:
                scunit_fprintfc(
                    stderr,
                    SCUNIT_COLOR_DARK_BLACK,
                    SCUNIT_COLOR_DARK_RED,
                    " FAIL "
                );
                summary->failedTests++;
                break;
            default:
                // This should not happen under normal circumstances and is a sign of a serious
                // programming error.
                error = SCUNIT_ERROR_UNKNOWN_RESULT;
                goto fail;
        }
        SCUnitMeasurement wallTimeMeasurement;
        SCUnitMeasurement cpuTimeMeasurement;
        scunit_timer_wallTime(testTimer, &wallTimeMeasurement);
        scunit_timer_cpuTime(testTimer, &cpuTimeMeasurement);
        scunit_fprintf(
            (result == SCUNIT_RESULT_FAIL) ? stderr : stdout,
            " [Wall: %.3F %s, CPU: %.3F %s]\n",
            wallTimeMeasurement.time,
            wallTimeMeasurement.timeUnitString,
            cpuTimeMeasurement.time,
            cpuTimeMeasurement.timeUnitString
        );
        const char* message;
        scunit_context_getMessage(context, &message);
        if (*message != '\0') {
            scunit_fprintf((result == SCUNIT_RESULT_FAIL) ? stderr : stdout, "%s", message);
        }
        else if (i == (suite->registeredTests - 1)) {
            scunit_printf("\n");
        }
        if (suite->testTeardown != nullptr) {
            suite->testTeardown();
        }
    }
    if (suite->teardown != nullptr) {
        suite->teardown();
    }
    error = scunit_timer_stop(suiteTimer);
    if (error != SCUNIT_ERROR_NONE) {
        goto fail;
    }
    scunit_timer_free(&testTimer);
    scunit_context_free(&context);
    SCUnitMeasurement wallTimeMeasurement;
    SCUnitMeasurement cpuTimeMeasurement;
    scunit_timer_wallTime(suiteTimer, &wallTimeMeasurement);
    scunit_timer_cpuTime(suiteTimer, &cpuTimeMeasurement);
    scunit_timer_free(&suiteTimer);
    scunit_printf("Tests: ");
    scunit_printfc(
        (summary->passedTests > 0) ? SCUNIT_COLOR_DARK_GREEN : SCUNIT_COLOR_DARK_DEFAULT,
        SCUNIT_COLOR_DARK_DEFAULT,
        "%" PRId64 " ",
        summary->passedTests
    );
    scunit_printf("Passed (");
    scunit_printfc(
        (summary->passedTests > 0) ? SCUNIT_COLOR_DARK_GREEN : SCUNIT_COLOR_DARK_DEFAULT,
        SCUNIT_COLOR_DARK_DEFAULT,
        "%.2F%%",
        (suite->registeredTests > 0)
            ? (((double) summary->passedTests) / suite->registeredTests) * 100.0
            : 0.0
    );
    scunit_printf("), ");
    scunit_printfc(
        (summary->skippedTests > 0) ? SCUNIT_COLOR_DARK_YELLOW : SCUNIT_COLOR_DARK_DEFAULT,
        SCUNIT_COLOR_DARK_DEFAULT,
        "%" PRId64 " ",
        summary->skippedTests
    );
    scunit_printf("Skipped (");
    scunit_printfc(
        (summary->skippedTests > 0) ? SCUNIT_COLOR_DARK_YELLOW : SCUNIT_COLOR_DARK_DEFAULT,
        SCUNIT_COLOR_DARK_DEFAULT,
        "%.2F%%",
        (suite->registeredTests > 0)
            ? (((double) summary->skippedTests) / suite->registeredTests) * 100.0
            : 0.0
    );
    scunit_printf("), ");
    scunit_printfc(
        (summary->failedTests > 0) ? SCUNIT_COLOR_DARK_RED : SCUNIT_COLOR_DARK_DEFAULT,
        SCUNIT_COLOR_DARK_DEFAULT,
        "%" PRId64 " ",
        summary->failedTests
    );
    scunit_printf("Failed (");
    scunit_printfc(
        (summary->failedTests > 0) ? SCUNIT_COLOR_DARK_RED : SCUNIT_COLOR_DARK_DEFAULT,
        SCUNIT_COLOR_DARK_DEFAULT,
        "%.2F%%",
        (suite->registeredTests > 0)
            ? (((double) summary->failedTests) / suite->registeredTests) * 100.0
            : 0.0
    );
    scunit_printf(
        "), %" PRId64 " Total\nWall: %.3F %s, CPU: %.3F %s\n\n",
        suite->registeredTests,
        wallTimeMeasurement.time,
        wallTimeMeasurement.timeUnitString,
        cpuTimeMeasurement.time,
        cpuTimeMeasurement.timeUnitString
    );
    return SCUNIT_ERROR_NONE;
fail:
    scunit_timer_free(&suiteTimer);
    scunit_timer_free(&testTimer);
    scunit_context_free(&context);
    return error;
}

SCUnitError scunit_suite_free(SCUnitSuite** suite) {
    if (suite == nullptr) {
        return SCUNIT_ERROR_ARGUMENT_NULL;
    }
    if (*suite != nullptr) {
        SCUNIT_FREE((*suite)->name);
        (*suite)->name = nullptr;
        for (int64_t i = 0; i < (*suite)->registeredTests; i++) {
            SCUNIT_FREE((*suite)->tests[i].name);
            (*suite)->tests[i].name = nullptr;
        }
        SCUNIT_FREE((*suite)->tests);
        (*suite)->tests = nullptr;
        (*suite)->capacity = 0;
        (*suite)->registeredTests = 0;
        SCUNIT_FREE(*suite);
        *suite = nullptr;
    }
    return SCUNIT_ERROR_NONE;
}