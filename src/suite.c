#include <inttypes.h>
#include <string.h>
#include <SCUnit/memory.h>
#include <SCUnit/print.h>
#include <SCUnit/random.h>
#include <SCUnit/scunit.h>
#include <SCUnit/suite.h>
#include <SCUnit/timer.h>

/** @brief Represents a test which is part of an `SCUnitSuite`. */
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
     * @brief Optional suite setup function to execute before executing all tests in this
     * `SCUnitSuite`.
     *
     * @note Each `SCUnitSuite` can only have one `SCUnitSuiteSetup` function. If equal to
     * `nullptr`, nothing is registered to be executed.
     */
    SCUnitSuiteSetup suiteSetup;

    /**
     * @brief Optional suite teardown function to execute after executing all tests in this
     * `SCUnitSuite`.
     *
     * @note Each `SCUnitSuite` can only have one `SCUnitSuiteTeardown` function. If equal to
     * `nullptr`, nothing is registered to be executed.
     */
    SCUnitSuiteTeardown suiteTeardown;

    /**
     * @brief Optional test setup function to execute before each individual test in this
     * `SCUnitSuite`.
     *
     * @note Each `SCUnitSuite` can only have one `SCUnitTestSetup` function. If equal to `nullptr`,
     * nothing is registered to be executed.
     */
    SCUnitTestSetup testSetup;

    /**
     * @brief Optional test teardown function to execute after each individual test in this
     * `SCUnitSuite`.
     *
     * @note Each `SCUnitSuite` can only have one `SCUnitTestTeardown` function. If equal to
     * `nullptr`, nothing is registered to be executed.
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

extern SCUnitRandom* random;

SCUnitSuite* scunit_suite_new(const char* name) {
    SCUnitSuite* suite = SCUNIT_MALLOC(sizeof(SCUnitSuite));
    if (suite == nullptr) {
        return nullptr;
    }
    *suite = (SCUnitSuite) { };
    suite->name = strdup(name);
    if (suite->name == nullptr) {
        SCUNIT_FREE(suite);
        return nullptr;
    }
    return suite;
}

const char* scunit_suite_getName(const SCUnitSuite* suite) {
    return suite->name;
}

void scunit_suite_setSuiteSetup(SCUnitSuite* suite, SCUnitSuiteSetup suiteSetup) {
    suite->suiteSetup = suiteSetup;
}

void scunit_suite_setSuiteTeardown(SCUnitSuite* suite, SCUnitSuiteTeardown suiteTeardown) {
    suite->suiteTeardown = suiteTeardown;
}

void scunit_suite_setTestSetup(SCUnitSuite* suite, SCUnitTestSetup testSetup) {
    suite->testSetup = testSetup;
}

void scunit_suite_setTestTeardown(SCUnitSuite* suite, SCUnitTestTeardown testTeardown) {
    suite->testTeardown = testTeardown;
}

SCUnitError scunit_suite_registerTest(
    SCUnitSuite* suite,
    const char* name,
    SCUnitTestFunction testFunction
) {
    if (suite->registeredTests >= suite->capacity) {
        int64_t newCapacity = (suite->capacity == 0) ? 1 : suite->capacity * GROWTH_FACTOR;
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

SCUnitError scunit_suite_execute(const SCUnitSuite* suite, SCUnitSummary* summary) {
    SCUnitError error = SCUNIT_ERROR_NONE;
    // Tests can be executed in a sequential or random order. This means that we may need to shuffle
    // the indices of the tests. If no tests are registered, `testIndices` is a `nullptr` since
    // allocating an array of size zero results in implementation-defined behavior (which we try to
    // avoid).
    int64_t* testIndices = nullptr;
    if (suite->registeredTests > 0) {
        testIndices = SCUNIT_MALLOC(suite->registeredTests * sizeof(int64_t));
        if (testIndices == nullptr) {
            error = SCUNIT_ERROR_OUT_OF_MEMORY;
            goto testIndicesAllocationFailed;
        }
    }
    // We initialize the indices of the tests in the order they were registered using
    // `[[gnu::constructor]]`, which is apparently in reversed order.
    for (int64_t i = 0; i < suite->registeredTests; i++) {
        testIndices[i] = suite->registeredTests - 1 - i;
    }
    if (scunit_getOrder() == SCUNIT_ORDER_RANDOM) {
        for (int64_t i = suite->registeredTests - 1; i > 0; i--) {
            int64_t j = scunit_random_int64(random, 0, i);
            int64_t temp = testIndices[i];
            testIndices[i] = testIndices[j];
            testIndices[j] = temp;
        }
    }
    SCUnitTimer* suiteTimer = scunit_timer_new();
    if (suiteTimer == nullptr) {
        error = SCUNIT_ERROR_OUT_OF_MEMORY;
        goto suiteTimerAllocationFailed;
    }
    SCUnitTimer* testTimer = scunit_timer_new();
    if (testTimer == nullptr) {
        error = SCUNIT_ERROR_OUT_OF_MEMORY;
        goto testTimerAllocationFailed;
    }
    SCUnitContext* context = scunit_context_new();
    if (context == nullptr) {
        error = SCUNIT_ERROR_OUT_OF_MEMORY;
        goto contextAllocationFailed;
    }
    scunit_printf("--- Suite ");
    scunit_printfc(SCUNIT_COLOR_DARK_CYAN, SCUNIT_COLOR_DARK_DEFAULT, "%s", suite->name);
    scunit_printf(" ---\n\n");
    *summary = (SCUnitSummary) { };
    error = scunit_timer_start(suiteTimer);
    if (error != SCUNIT_ERROR_NONE) {
        goto failed;
    }
    if (suite->suiteSetup != nullptr) {
        suite->suiteSetup();
    }
    for (int64_t i = 0; i < suite->registeredTests; i++) {
        if (suite->testSetup != nullptr) {
            suite->testSetup();
        }
        const SCUnitTest* test = &suite->tests[testIndices[i]];
        scunit_printf("(%" PRId64 "/%" PRId64 ") Executing test ", i + 1, suite->registeredTests);
        scunit_printfc(SCUNIT_COLOR_DARK_CYAN, SCUNIT_COLOR_DARK_DEFAULT, "%s", test->name);
        scunit_printf("... ");
        // Reuse the context for every test to avoid some unnecessary memory allocations.
        scunit_context_reset(context);
        error = scunit_timer_start(testTimer);
        if (error != SCUNIT_ERROR_NONE) {
            goto failed;
        }
        test->testFunction(context);
        error = scunit_timer_stop(testTimer);
        if (error != SCUNIT_ERROR_NONE) {
            goto failed;
        }
        SCUnitResult result = scunit_context_getResult(context);
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
                scunit_fprintfc(
                    stderr,
                    SCUNIT_COLOR_DARK_RED,
                    SCUNIT_COLOR_DARK_DEFAULT,
                    "Encountered an unexpected test result '%d'.\n",
                    result
                );
                exit(EXIT_FAILURE);
        }
        SCUnitMeasurement wallTimeMeasurement = scunit_timer_getWallTime(testTimer, &error);
        SCUnitMeasurement cpuTimeMeasurement = scunit_timer_getCPUTime(testTimer, &error);
        scunit_fprintf(
            (result == SCUNIT_RESULT_FAIL) ? stderr : stdout,
            " [Wall: %.3F %s, CPU: %.3F %s]\n",
            wallTimeMeasurement.time,
            wallTimeMeasurement.timeUnitString,
            cpuTimeMeasurement.time,
            cpuTimeMeasurement.timeUnitString
        );
        const char* message = scunit_context_getMessage(context);
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
    if (suite->suiteTeardown != nullptr) {
        suite->suiteTeardown();
    }
    error = scunit_timer_stop(suiteTimer);
    if (error != SCUNIT_ERROR_NONE) {
        goto failed;
    }
    SCUnitMeasurement wallTimeMeasurement = scunit_timer_getWallTime(suiteTimer, &error);
    SCUnitMeasurement cpuTimeMeasurement = scunit_timer_getCPUTime(suiteTimer, &error);
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
failed:
    scunit_context_free(context);
contextAllocationFailed:
    scunit_timer_free(testTimer);
testTimerAllocationFailed:
    scunit_timer_free(suiteTimer);
suiteTimerAllocationFailed:
    SCUNIT_FREE(testIndices);
testIndicesAllocationFailed:
    return error;
}

void scunit_suite_free(SCUnitSuite* suite) {
    if (suite != nullptr) {
        SCUNIT_FREE(suite->name);
        for (int64_t i = 0; i < suite->registeredTests; i++) {
            SCUNIT_FREE(suite->tests[i].name);
        }
        SCUNIT_FREE(suite->tests);
        SCUNIT_FREE(suite);
    }
}