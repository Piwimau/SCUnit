#include <getopt.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SCUnit/scunit.h>

/** @brief Represents a long command line option. */
typedef struct option SCUnitLongOption;

/** @brief Version information of SCUnit. */
static constexpr SCUnitVersion VERSION = { .major = 0, .minor = 2, .patch = 1 };

/** @brief Growth factor used for resizing the array of suites. */
static constexpr int64_t GROWTH_FACTOR = 2;

/**
 * @brief Supported short command line options.
 *
 * @note The leading '-' causes `getopt_long()` to handle nonoption arguments as if they were the
 * argument of an option, allowing us to directly recognize and report unexpected arguments.
 *
 * The first ':' causes `getopt_long()` to return ':' instead of '?' to indicate a missing option
 * argument, which is used to differentiate this case from an unknown option.
 */
static const char* const SHORT_OPTIONS = "-:hv";

/** @brief Supported long command line options. */
static const SCUnitLongOption LONG_OPTIONS[] = {
    { "help", no_argument, nullptr, 'h' },
    { "version", no_argument, nullptr, 'v' },
    { "colored-output", required_argument, nullptr, 0 },
    { nullptr, no_argument, nullptr, 0 }
};

/**
 * @brief Suites registered to be run automatically by SCUnit.
 *
 * @note This is a dynamically resized array with storage for `capacity` elements and
 * `registeredSuites` registered suites, except if `capacity` is zero, in which case it is initially
 * a `nullptr`.
 */
static SCUnitSuite** suites;

/** @brief Capacity for registering suites. */
static int64_t capacity;

/** @brief Number of registered suites. */
static int64_t registeredSuites;

SCUnitVersion scunit_getVersion() {
    return VERSION;
}

void scunit_registerSuite(SCUnitSuite* suite, SCUnitError* error) {
    if (registeredSuites >= capacity) {
        int64_t newCapacity = (capacity == 0) ? 1 : capacity * GROWTH_FACTOR;
        SCUnitSuite** newSuites = SCUNIT_REALLOC(suites, newCapacity * sizeof(SCUnitSuite*));
        if (newSuites == nullptr) {
            *error = SCUNIT_ERROR_OUT_OF_MEMORY;
            return;
        }
        suites = newSuites;
        capacity = newCapacity;
    }
    suites[registeredSuites++] = suite;
    *error = SCUNIT_ERROR_NONE;
}

/**
 * @brief Parses the command line arguments passed to the test executable.
 *
 * @attention If any unexpected error occurs, an error message is printed to `stderr` and the
 * program exits using `EXIT_FAILURE`.
 *
 * Note that the program immediately exits with `EXIT_SUCCESS` if the help (`-h` or `--help`) or
 * version (`-v` or `--version`) option is present in `argv`.
 *
 * @param[in] argc Number of command line arguments passed to the test executable.
 * @param[in] argv Command line arguments passed to the test executable.
 */
static void scunit_parseArguments(int argc, char** argv) {
    // Disable error messages of `getopt_long()`.
    opterr = 0;
    int option;
    int optionIndex;
    while ((option = getopt_long(argc, argv, SHORT_OPTIONS, LONG_OPTIONS, &optionIndex)) != -1) {
        switch (option) {
            case 'h':
                scunit_printf(
                    "Usage: %s [OPTION]...\n"
                    "\n"
                    "Options:\n"
                    "  -h, --help                           Display this help and exit.\n"
                    "  -v, --version                        Display version information and exit.\n"
                    "  --colored-output={disabled|enabled}  Enable or disable colored output "
                    "(default = enabled).\n",
                    argv[0]
                );
                exit(EXIT_SUCCESS);
            case 'v':
                scunit_printf(
                    "SCUnit %" PRId32 ".%" PRId32 ".%" PRId32 "\n",
                    VERSION.major,
                    VERSION.minor,
                    VERSION.patch
                );
                exit(EXIT_SUCCESS);
            case 0:
                const char* optionName = LONG_OPTIONS[optionIndex].name;
                if (strcmp(optarg, "disabled") == 0) {
                    SCUnitError error;
                    scunit_setColoredOutput(SCUNIT_COLORED_OUTPUT_DISABLED, &error);
                }
                else if (strcmp(optarg, "enabled") == 0) {
                    SCUnitError error;
                    scunit_setColoredOutput(SCUNIT_COLORED_OUTPUT_ENABLED, &error);
                }
                else {
                    scunit_fprintf(
                        stderr,
                        "Invalid argument '%s' for option '--%s'.\n"
                        "Try option '-h' or '--help' for more information.\n",
                        optarg,
                        optionName
                    );
                    exit(EXIT_FAILURE);
                }
                break;
            case 1:
                scunit_fprintf(
                    stderr,
                    "Unexpected argument '%s'.\n"
                    "Try option '-h' or '--help' for more information.\n",
                    optarg
                );
                exit(EXIT_FAILURE);
            case ':':
                scunit_fprintf(
                    stderr,
                    "Missing argument for option '%s'.\n"
                    "Try option '-h' or '--help' for more information.\n",
                    argv[optind - 1]
                );
                exit(EXIT_FAILURE);
            case '?':
                scunit_fprintf(stderr, "Unknown option '");
                if (optopt == 0) {
                    scunit_fprintf(stderr, "%s", argv[optind - 1]);
                }
                else {
                    scunit_fprintf(stderr, "-%c", optopt);
                }
                scunit_fprintf(
                    stderr,
                    "'.\n"
                    "Try option '-h' or '--help' for more information.\n"
                );
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
    SCUnitError error;
    SCUnitTimer* timer = scunit_timer_new(&error);
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
    scunit_timer_start(timer, &error);
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
        SCUnitSummary suiteSummary;
        // Run suites in the order they are originally defined in, which is apparently reversed in
        // relation to how they are registered using `[[gnu::constructor]]`. Note that SCUnit does
        // not guarantee the order in which tests or suites are run anywhere, so we are free to
        // implement this however we want.
        scunit_suite_run(suites[registeredSuites - 1 - i], &suiteSummary, &error);
        if (error != SCUNIT_ERROR_NONE) {
            scunit_fprintfc(
                stderr,
                SCUNIT_COLOR_DARK_RED,
                SCUNIT_COLOR_DARK_DEFAULT,
                "An unexpected error occurred while running the suite %s (code %d).\n",
                scunit_suite_getName(suites[registeredSuites - 1 - i]),
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
    scunit_timer_stop(timer, &error);
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
    SCUnitMeasurement wallTimeMeasurement = scunit_timer_getWallTime(timer, &error);
    SCUnitMeasurement cpuTimeMeasurement = scunit_timer_getCPUTime(timer, &error);
    scunit_timer_free(timer);
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
        scunit_suite_free(suites[i]);
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