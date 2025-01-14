#include <getopt.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SCUnit/scunit.h>

/** @brief Represents a structure containing the configuration settings of SCUnit. */
typedef struct SCUnitConfig {

    /** @brief Current state of the colored output. */
    SCUnitColoredOutput coloredOutput;

    /** @brief Current order in which suites and tests are executed. */
    SCUnitOrder order;

} SCUnitConfig;

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
    { "color", required_argument, nullptr, 0 },
    { "order", required_argument, nullptr, 0 },
    { "seed", required_argument, nullptr, 0 },
    { nullptr, no_argument, nullptr, 0 }
};

/** @brief SCUnit configuration settings. */
static SCUnitConfig config = {
    .coloredOutput = SCUNIT_COLORED_OUTPUT_ALWAYS,
    .order = SCUNIT_ORDER_SEQUENTIAL
};

/**
 * @brief Suites registered to be executed automatically by SCUnit.
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

/** @brief Single pseudorandom number generator (PRNG) used by SCUnit. */
SCUnitRandom* random;

/**
 * @brief Initializes SCUnit.
 *
 * @attention If any unexpected error occurs, an error message is printed to `stderr` and the
 * program exits using `EXIT_FAILURE`.
 */
[[gnu::constructor(101)]]
static void init() {
    random = scunit_random_new();
    if (random == nullptr) {
        scunit_fprintfc(
            stderr,
            SCUNIT_COLOR_DARK_RED,
            SCUNIT_COLOR_DARK_DEFAULT,
            "An unexpected error occurred while initializing SCUnit (code %d).\n",
            SCUNIT_ERROR_OUT_OF_MEMORY
        );
        exit(EXIT_FAILURE);
    }
}

SCUnitVersion scunit_getVersion() {
    return VERSION;
}

SCUnitColoredOutput scunit_getColoredOutput() {
    return config.coloredOutput;
}

SCUnitError scunit_setColoredOutput(SCUnitColoredOutput coloredOutput) {
    if ((coloredOutput < SCUNIT_COLORED_OUTPUT_NEVER)
            || (coloredOutput > SCUNIT_COLORED_OUTPUT_ALWAYS)) {
        return SCUNIT_ERROR_ARGUMENT_OUT_OF_RANGE;
    }
    config.coloredOutput = coloredOutput;
    return SCUNIT_ERROR_NONE;
}

SCUnitOrder scunit_getOrder() {
    return config.order;
}

SCUnitError scunit_setOrder(SCUnitOrder order) {
    if ((order < SCUNIT_ORDER_SEQUENTIAL) || (order > SCUNIT_ORDER_RANDOM)) {
        return SCUNIT_ERROR_ARGUMENT_OUT_OF_RANGE;
    }
    config.order = order;
    return SCUNIT_ERROR_NONE;
}

SCUnitError scunit_registerSuite(SCUnitSuite* suite) {
    if (registeredSuites >= capacity) {
        int64_t newCapacity = (capacity == 0) ? 1 : capacity * GROWTH_FACTOR;
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

void scunit_parseArguments(int argc, char** argv) {
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
                    "  -h, --help                   Display this help and exit.\n"
                    "  -v, --version                Display version information and exit.\n"
                    "  --color={never|always}       Colorize the output (default = always).\n"
                    "  --order={sequential|random}  Execute suites and tests in a different order "
                    "(default = sequential).\n"
                    "  --seed=<seed>                Use a specific seed to reproduce a run.\n"
                    "                               Parsed as a uint64_t in octal, hexadecimal or "
                    "decimal notation.\n"
                    "                               Only has an effect if '--order=random' is "
                    "specified.\n",
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
                if (strcmp(optionName, "color") == 0) {
                    if (strcmp(optarg, "never") == 0) {
                        config.coloredOutput = SCUNIT_COLORED_OUTPUT_NEVER;
                    }
                    else if (strcmp(optarg, "always") == 0) {
                        config.coloredOutput = SCUNIT_COLORED_OUTPUT_ALWAYS;
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
                }
                else if (strcmp(optionName, "order") == 0) {
                    if (strcmp(optarg, "sequential") == 0) {
                        config.order = SCUNIT_ORDER_SEQUENTIAL;
                    }
                    else if (strcmp(optarg, "random") == 0) {
                        config.order = SCUNIT_ORDER_RANDOM;
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
                }
                else if (strcmp(optionName, "seed") == 0) {
                    char* end = nullptr;
                    errno = 0;
                    uint64_t seed = strtoull(optarg, &end, 0);
                    if ((*optarg == '\0') || (*end != '\0') || (errno == ERANGE)) {
                        scunit_fprintf(
                            stderr,
                            "Invalid argument '%s' for option '--%s'.\n"
                            "Try option '-h' or '--help' for more information.\n",
                            optarg,
                            optionName
                        );
                        exit(EXIT_FAILURE);
                    }
                    scunit_random_setSeed(random, seed);
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

int scunit_executeSuites() {
    int exitCode = EXIT_SUCCESS;
    // Suites can be executed in a sequential or random order. This means that we may need to
    // shuffle the indices of the suites. If no suites are registered, `suiteIndices` is a `nullptr`
    // since allocating an array of size zero results in implementation-defined behavior (which we
    // try to avoid).
    int64_t* suiteIndices = nullptr;
    if (registeredSuites > 0) {
        suiteIndices = SCUNIT_MALLOC(registeredSuites * sizeof(int64_t));
        if (suiteIndices == nullptr) {
            scunit_fprintfc(
                stderr,
                SCUNIT_COLOR_DARK_RED,
                SCUNIT_COLOR_DARK_DEFAULT,
                "An unexpected error occurred while preparing the execution of the suites "
                "(code %d).\n",
                SCUNIT_ERROR_OUT_OF_MEMORY
            );
            exitCode = EXIT_FAILURE;
            goto suiteIndicesAllocationFailed;
        }
    }
    // We initialize the indices of the suites in the order they were registered using
    // `[[gnu::constructor]]`, which is apparently in reversed order.
    for (int64_t i = 0; i < registeredSuites; i++) {
        suiteIndices[i] = registeredSuites - 1 - i;
    }
    if (config.order == SCUNIT_ORDER_RANDOM) {
        for (int64_t i = registeredSuites - 1; i > 0; i--) {
            int64_t j = scunit_random_int64(random, 0, i);
            int64_t temp = suiteIndices[i];
            suiteIndices[i] = suiteIndices[j];
            suiteIndices[j] = temp;
        }
    }
    int64_t failedSuites = 0;
    SCUnitSummary summary = { };
    SCUnitTimer* timer = scunit_timer_new();
    if (timer == nullptr) {
        scunit_fprintfc(
            stderr,
            SCUNIT_COLOR_DARK_RED,
            SCUNIT_COLOR_DARK_DEFAULT,
            "An unexpected error occurred while preparing the execution of the suites (code %d).\n",
            SCUNIT_ERROR_OUT_OF_MEMORY
        );
        exitCode = EXIT_FAILURE;
        goto timerAllocationFailed;
    }
    SCUnitError error = scunit_timer_start(timer);
    if (error != SCUNIT_ERROR_NONE) {
        scunit_fprintfc(
            stderr,
            SCUNIT_COLOR_DARK_RED,
            SCUNIT_COLOR_DARK_DEFAULT,
            "An unexpected error occurred while executing the suites (code %d).\n",
            error
        );
        exitCode = EXIT_FAILURE;
        goto failed;
    }
    for (int64_t i = 0; i < registeredSuites; i++) {
        const SCUnitSuite* suite = suites[suiteIndices[i]];
        SCUnitSummary suiteSummary;
        error = scunit_suite_execute(suite, &suiteSummary);
        if (error != SCUNIT_ERROR_NONE) {
            scunit_fprintfc(
                stderr,
                SCUNIT_COLOR_DARK_RED,
                SCUNIT_COLOR_DARK_DEFAULT,
                "An unexpected error occurred while executing the suite %s (code %d).\n",
                scunit_suite_getName(suite),
                error
            );
            exitCode = EXIT_FAILURE;
            goto failed;
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
            "An unexpected error occurred while executing the suites (code %d).\n",
            error
        );
        exitCode = EXIT_FAILURE;
        goto failed;
    }
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
    SCUnitMeasurement wallTimeMeasurement = scunit_timer_getWallTime(timer, &error);
    SCUnitMeasurement cpuTimeMeasurement = scunit_timer_getCPUTime(timer, &error);
    scunit_printf(
        "), %" PRId64 " Total\nWall: %.3F %s, CPU: %.3F %s\n",
        totalTests,
        wallTimeMeasurement.time,
        wallTimeMeasurement.timeUnitString,
        cpuTimeMeasurement.time,
        cpuTimeMeasurement.timeUnitString
    );
    if (config.order == SCUNIT_ORDER_RANDOM) {
        scunit_printf(
            "\nNote: Suites and tests were executed in a random order.\n"
            "Specify '--seed=%" PRIu64 "' to reproduce this run.\n",
            scunit_random_getSeed(random)
        );
    }
failed:
    scunit_timer_free(timer);
timerAllocationFailed:
    SCUNIT_FREE(suiteIndices);
suiteIndicesAllocationFailed:
    if (exitCode != EXIT_SUCCESS) {
        exit(exitCode);
    }
    return (summary.failedTests > 0) ? EXIT_FAILURE : EXIT_SUCCESS;
}

/** @brief Deinitializes SCUnit. */
[[gnu::destructor(101)]]
static void deinit() {
    for (int64_t i = 0; i < registeredSuites; i++) {
        scunit_suite_free(suites[i]);
    }
    SCUNIT_FREE(suites);
    scunit_random_free(random);
}