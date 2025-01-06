#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SCUnit/scunit.h>

/** @brief Version information of SCUnit. */
static constexpr SCUnitVersion VERSION = { .major = 0, .minor = 2, .patch = 1 };

/** @brief Growth factor used for resizing the array of suites. */
static constexpr int64_t GROWTH_FACTOR = 2;

/**
 * @brief Suites registered to be run automatically by SCUnit.
 *
 * @note This is a dynamically resized array with storage for `capacity` elements and
 * `registeredSuites` registered suites.
 */
static SCUnitSuite** suites;

/** @brief Capacity for registering suites. */
static int64_t capacity;

/** @brief Number of registered suites. */
static int64_t registeredSuites;

SCUnitVersion scunit_getVersion() {
    return VERSION;
}

SCUnitError scunit_registerSuite(SCUnitSuite* suite) {
    if (suite == nullptr) {
        return SCUNIT_ERROR_ARGUMENT_NULL;
    }
    if (registeredSuites >= capacity) {
        // Add one for the edge case of `capacity` being zero (which is the case initially).
        int64_t newCapacity = (capacity * GROWTH_FACTOR) + 1;
        SCUnitSuite** newSuites = SCUNIT_REALLOC(suites, newCapacity * sizeof(SCUnitSuite*));
        if (newSuites == nullptr) {
            return SCUNIT_ERROR_OUT_OF_MEMORY;
        }
        suites = newSuites;
        capacity = newCapacity;
    }
    suites[registeredSuites++] = suite;
    return SCUNIT_ERROR_NONE;
}

/**
 * @brief Parses the command line arguments passed to the test executable.
 *
 * @attention If any unexpected error occurs (this includes `argc` being negative, `argv` being
 * `nullptr` or passing an unknown option to the test executable), an error message is printed to
 * `stderr` and the program exits using `EXIT_FAILURE`.
 *
 * Note that the program immediately exits with `EXIT_SUCCESS` if the help (`-h` or `--help`) or
 * version (`-v` or `--version`) option is present in `argv`.
 *
 * @param[in] argc Number of command line arguments passed to the test executable.
 * @param[in] argv Command line arguments passed to the test executable.
 */
static void scunit_parseArguments(int argc, char** argv) {
    if (argc < 0) {
        scunit_fprintfc(
            stderr,
            SCUNIT_COLOR_DARK_RED,
            SCUNIT_COLOR_DARK_DEFAULT,
            "A negative number of arguments was supplied.\n"
        );
        exit(EXIT_FAILURE);
    }
    if (argv == nullptr) {
        scunit_fprintfc(
            stderr,
            SCUNIT_COLOR_DARK_RED,
            SCUNIT_COLOR_DARK_DEFAULT,
            "A null pointer was supplied for the arguments.\n"
        );
        exit(EXIT_FAILURE);
    }
    for (int i = 1; i < argc; i++) {
        if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0)) {
            scunit_printf("Usage: <executable> [");
            scunit_printfc(SCUNIT_COLOR_DARK_CYAN, SCUNIT_COLOR_DARK_DEFAULT, "OPTION");
            scunit_printf("]...\n\nOptions:\n");
            scunit_printfc(SCUNIT_COLOR_DARK_CYAN, SCUNIT_COLOR_DARK_DEFAULT, "  -h");
            scunit_printf(", ");
            scunit_printfc(SCUNIT_COLOR_DARK_CYAN, SCUNIT_COLOR_DARK_DEFAULT, "--help");
            scunit_printf("                           Print this help message and exit.\n");
            scunit_printfc(SCUNIT_COLOR_DARK_CYAN, SCUNIT_COLOR_DARK_DEFAULT, "  -v");
            scunit_printf(", ");
            scunit_printfc(SCUNIT_COLOR_DARK_CYAN, SCUNIT_COLOR_DARK_DEFAULT, "--version");
            scunit_printf("                        Print version information and exit.\n");
            scunit_printfc(
                SCUNIT_COLOR_DARK_CYAN,
                SCUNIT_COLOR_DARK_DEFAULT,
                "  --colored-output"
            );
            scunit_printf(
                "={disabled|enabled}  Enable or disable colored output (default = enabled).\n"
                "                                       Only has an effect on subsequent options.\n"
            );
            exit(EXIT_SUCCESS);
        }
        else if ((strcmp(argv[i], "-v") == 0) || (strcmp(argv[i], "--version") == 0)) {
            scunit_printf(
                "SCUnit %" PRId32 ".%" PRId32 ".%" PRId32 "\n",
                VERSION.major,
                VERSION.minor,
                VERSION.patch
            );
            exit(EXIT_SUCCESS);
        }
        else if (strcmp(argv[i], "--colored-output=disabled") == 0) {
            scunit_setColoredOutput(SCUNIT_COLORED_OUTPUT_DISABLED);
        }
        else if (strcmp(argv[i], "--colored-output=enabled") == 0) {
            scunit_setColoredOutput(SCUNIT_COLORED_OUTPUT_ENABLED);
        }
        else {
            scunit_fprintf(stderr, "Unknown option ");
            scunit_fprintfc(
                stderr,
                SCUNIT_COLOR_DARK_RED,
                SCUNIT_COLOR_DARK_DEFAULT,
                "%s",
                argv[i]
            );
            scunit_fprintf(stderr, ". Try option ");
            scunit_fprintfc(stderr, SCUNIT_COLOR_DARK_CYAN, SCUNIT_COLOR_DARK_DEFAULT, "-h");
            scunit_fprintf(stderr, " or ");
            scunit_fprintfc(stderr, SCUNIT_COLOR_DARK_CYAN, SCUNIT_COLOR_DARK_DEFAULT, "--help");
            scunit_fprintf(stderr, " for more information.\n");
            exit(EXIT_FAILURE);
        }
    }
}

/**
 * @brief Runs all registered suites.
 *
 * @attention If any unexpected error occurs, an error message is printed to `stderr` and the
 * program exits using `EXIT_FAILURE`.
 *
 * @return `EXIT_SUCCESS` if all tests passed, otherwise `EXIT_FAILURE`.
 */
static int scunit_runSuites() {
    SCUnitTimer* timer;
    SCUnitError error = scunit_timer_new(&timer);
    if (error != SCUNIT_ERROR_NONE) {
        scunit_fprintfc(
            stderr,
            SCUNIT_COLOR_DARK_RED,
            SCUNIT_COLOR_DARK_DEFAULT,
            "An unexpected error occurred while running the suites (code %d).\n",
            error
        );
        exit(EXIT_FAILURE);
    }
    error = scunit_timer_start(timer);
    if (error != SCUNIT_ERROR_NONE) {
        scunit_fprintfc(
            stderr,
            SCUNIT_COLOR_DARK_RED,
            SCUNIT_COLOR_DARK_DEFAULT,
            "An unexpected error occurred while running the suites (code %d).\n",
            error
        );
        exit(EXIT_FAILURE);
    }
    int64_t failedSuites = 0;
    SCUnitSummary summary = { };
    for (int64_t i = 0; i < registeredSuites; i++) {
        SCUnitSummary suiteSummary = { };
        // Run suites in the order they are originally defined in, which is apparently reversed in
        // relation to how they are registered using `[[gnu::constructor]]`. Note that SCUnit does
        // not guarantee the order in which tests or suites are run anywhere, so we are free to
        // implement this however we want.
        error = scunit_suite_run(suites[registeredSuites - 1 - i], &suiteSummary);
        if (error != SCUNIT_ERROR_NONE) {
            const char* name;
            scunit_suite_getName(suites[registeredSuites - 1 - i], &name);
            scunit_fprintfc(
                stderr,
                SCUNIT_COLOR_DARK_RED,
                SCUNIT_COLOR_DARK_DEFAULT,
                "An unexpected error occurred while running the suite %s (code %d).\n",
                name,
                error
            );
            exit(EXIT_FAILURE);
        }
        if (suiteSummary.failedTests > 0) {
            failedSuites++;
        }
        summary.passedTests += suiteSummary.passedTests;
        summary.skippedTests += suiteSummary.skippedTests;
        summary.failedTests += suiteSummary.failedTests;
    }
    error = scunit_timer_stop(timer);
    if (error != SCUNIT_ERROR_NONE) {
        scunit_fprintfc(
            stderr,
            SCUNIT_COLOR_DARK_RED,
            SCUNIT_COLOR_DARK_DEFAULT,
            "An unexpected error occurred while running the suites (code %d).\n",
            error
        );
        exit(EXIT_FAILURE);
    }
    SCUnitMeasurement wallTimeMeasurement;
    SCUnitMeasurement cpuTimeMeasurement;
    scunit_timer_getWallTime(timer, &wallTimeMeasurement);
    scunit_timer_getCPUTime(timer, &cpuTimeMeasurement);
    scunit_timer_free(&timer);
    scunit_printf("--- ");
    scunit_printfc(SCUNIT_COLOR_DARK_CYAN, SCUNIT_COLOR_DARK_DEFAULT, "Summary");
    scunit_printf(" ---\n\nSuites: ");
    int64_t passedSuites = registeredSuites - failedSuites;
    scunit_printfc(
        (passedSuites > 0) ? SCUNIT_COLOR_DARK_GREEN : SCUNIT_COLOR_DARK_DEFAULT,
        SCUNIT_COLOR_DARK_DEFAULT,
        "%" PRId64 " ",
        passedSuites
    );
    scunit_printf("Passed (");
    scunit_printfc(
        (passedSuites > 0) ? SCUNIT_COLOR_DARK_GREEN : SCUNIT_COLOR_DARK_DEFAULT,
        SCUNIT_COLOR_DARK_DEFAULT,
        "%.2F%%",
        (registeredSuites > 0) ? (((double) passedSuites) / registeredSuites) * 100.0 : 0.0
    );
    scunit_printf("), ");
    scunit_printfc(
        (failedSuites > 0) ? SCUNIT_COLOR_DARK_RED : SCUNIT_COLOR_DARK_DEFAULT,
        SCUNIT_COLOR_DARK_DEFAULT,
        "%" PRId64 " ",
        failedSuites
    );
    scunit_printf("Failed (");
    scunit_printfc(
        (failedSuites > 0) ? SCUNIT_COLOR_DARK_RED : SCUNIT_COLOR_DARK_DEFAULT,
        SCUNIT_COLOR_DARK_DEFAULT,
        "%.2F%%",
        (registeredSuites > 0) ? (((double) failedSuites) / registeredSuites) * 100.0 : 0.0
    );
    scunit_printf("), %" PRId64 " Total\nTests: ", registeredSuites);
    scunit_printfc(
        (summary.passedTests > 0) ? SCUNIT_COLOR_DARK_GREEN : SCUNIT_COLOR_DARK_DEFAULT,
        SCUNIT_COLOR_DARK_DEFAULT,
        "%" PRId64 " ",
        summary.passedTests
    );
    scunit_printf("Passed (");
    int64_t totalTests = summary.passedTests + summary.skippedTests + summary.failedTests;
    scunit_printfc(
        (summary.passedTests > 0) ? SCUNIT_COLOR_DARK_GREEN : SCUNIT_COLOR_DARK_DEFAULT,
        SCUNIT_COLOR_DARK_DEFAULT,
        "%.2F%%",
        (totalTests > 0) ? (((double) summary.passedTests) / totalTests) * 100.0 : 0.0
    );
    scunit_printf("), ");
    scunit_printfc(
        (summary.skippedTests > 0) ? SCUNIT_COLOR_DARK_YELLOW : SCUNIT_COLOR_DARK_DEFAULT,
        SCUNIT_COLOR_DARK_DEFAULT,
        "%" PRId64 " ",
        summary.skippedTests
    );
    scunit_printf("Skipped (");
    scunit_printfc(
        (summary.skippedTests > 0) ? SCUNIT_COLOR_DARK_YELLOW : SCUNIT_COLOR_DARK_DEFAULT,
        SCUNIT_COLOR_DARK_DEFAULT,
        "%.2F%%",
        (totalTests > 0) ? (((double) summary.skippedTests) / totalTests) * 100.0 : 0.0
    );
    scunit_printf("), ");
    scunit_printfc(
        (summary.failedTests > 0) ? SCUNIT_COLOR_DARK_RED : SCUNIT_COLOR_DARK_DEFAULT,
        SCUNIT_COLOR_DARK_DEFAULT,
        "%" PRId64 " ",
        summary.failedTests
    );
    scunit_printf("Failed (");
    scunit_printfc(
        (summary.failedTests > 0) ? SCUNIT_COLOR_DARK_RED : SCUNIT_COLOR_DARK_DEFAULT,
        SCUNIT_COLOR_DARK_DEFAULT,
        "%.2F%%",
        (totalTests > 0) ? (((double) summary.failedTests) / totalTests) * 100.0 : 0.0
    );
    scunit_printf(
        "), %" PRId64 " Total\nWall: %.3F %s, CPU: %.3F %s\n",
        totalTests,
        wallTimeMeasurement.time,
        wallTimeMeasurement.timeUnitString,
        cpuTimeMeasurement.time,
        cpuTimeMeasurement.timeUnitString
    );
    return (summary.failedTests > 0) ? EXIT_FAILURE : EXIT_SUCCESS;
}

/** @brief Deallocates all registered suites. */
static inline void scunit_freeSuites() {
    for (int64_t i = 0; i < registeredSuites; i++) {
        scunit_suite_free(&suites[i]);
    }
    SCUNIT_FREE(suites);
    suites = nullptr;
    capacity = 0;
    registeredSuites = 0;
}

int scunit_main(int argc, char** argv) {
    scunit_parseArguments(argc, argv);
    int exitCode = scunit_runSuites();
    scunit_freeSuites();
    return exitCode;
}