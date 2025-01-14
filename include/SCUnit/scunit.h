#ifndef SCUNIT_H
#define SCUNIT_H

#include <stdint.h>
#include <SCUnit/assert.h>
#include <SCUnit/context.h>
#include <SCUnit/error.h>
#include <SCUnit/memory.h>
#include <SCUnit/print.h>
#include <SCUnit/random.h>
#include <SCUnit/suite.h>
#include <SCUnit/timer.h>

/** @brief Represents the version information of SCUnit. */
typedef struct SCUnitVersion {

    /** @brief Major version number of SCUnit. */
    int32_t major;

    /** @brief Minor version number of SCUnit. */
    int32_t minor;

    /** @brief Patch version number of SCUnit. */
    int32_t patch;

} SCUnitVersion;

/** @brief Represents an enumeration of the different states the colored output may be in. */
typedef enum SCUnitColoredOutput {

    /** @brief Indicates that colored output is never used. */
    SCUNIT_COLORED_OUTPUT_NEVER,

    /** @brief Indicates that colored output is always used. */
    SCUNIT_COLORED_OUTPUT_ALWAYS

} SCUnitColoredOutput;

/**
 * @brief Represents an enumeration of the different orders in which suites and tests can be
 * executed.
 */
typedef enum SCUnitOrder {

    /** @brief Indicates that suites and tests are executed in a sequential order. */
    SCUNIT_ORDER_SEQUENTIAL,

    /** @brief Indicates that suites and tests are executed in a random order. */
    SCUNIT_ORDER_RANDOM

} SCUnitOrder;

/**
 * @brief Gets the version information of SCUnit.
 *
 * @return An `SCUnitVersion` containing the major, minor and patch version number of SCUnit.
 */
SCUnitVersion scunit_getVersion();

/**
 * @brief Gets the current state of the colored output.
 *
 * @note Colored output is always used by default (set to `SCUNIT_COLORED_OUTPUT_ALWAYS`).
 *
 * @return The current state of the colored output.
 */
SCUnitColoredOutput scunit_getColoredOutput();

/**
 * @brief Sets the state of the colored output.
 *
 * @note Colored output is always used by default (set to `SCUNIT_COLORED_OUTPUT_ALWAYS`).
 *
 * @param[in] coloredOutput `SCUnitColoredOutput` state to set.
 * @return `SCUNIT_ERROR_ARGUMENT_OUT_OF_RANGE` if `coloredOutput` is not a valid
 * `SCUnitColoredOutput`, otherwise `SCUNIT_ERROR_NONE`.
 */
SCUnitError scunit_setColoredOutput(SCUnitColoredOutput coloredOutput);

/**
 * @brief Gets the current order in which suites and tests are executed.
 *
 * @note Suites and tests are executed in a sequential order by default (set to
 * `SCUNIT_ORDER_SEQUENTIAL`).
 *
 * @return The current order in which suites and tests are executed.
 */
SCUnitOrder scunit_getOrder();

/**
 * @brief Sets the order in which suites and tests are executed.
 *
 * @param[in] order `SCUnitOrder` to set.
 * @return `SCUNIT_ERROR_ARGUMENT_OUT_OF_RANGE` if `order` is not a valid `SCUnitOrder`,
 * otherwise `SCUNIT_ERROR_NONE`.
 */
SCUnitError scunit_setOrder(SCUnitOrder order);

/**
 * @brief Registers an `SCUnitSuite` to be executed automatically by SCUnit.
 *
 * @note Modifying the suite (i. e. registering setup, teardown or test functions) after it has been
 * registered is allowed.
 *
 * @warning SCUnit takes ownership of the given `SCUnitSuite` and is responsible for the
 * deallocation. You must not deallocate it manually yourself.
 *
 * @param[in] suite `SCUnitSuite` to register.
 * @return `SCUNIT_ERROR_OUT_OF_MEMORY` if an out-of-memory condition occurred,
 * otherwise `SCUNIT_ERROR_NONE`.
 */
SCUnitError scunit_registerSuite(SCUnitSuite* suite);

/**
 * @brief Parses the command line arguments passed to the test executable.
 *
 * @note This function produces diagnostic output on `stdout` and `stderr`, such as the usage or
 * error messages in case an unexpected error occurs while parsing the command line arguments.
 *
 * It respects the current colored output state set by calling `scunit_setColoredOutput()`.
 * If currently set to `SCUNIT_COLORED_OUTPUT_NEVER`, the default color is used instead.
 *
 * Note that the program immediately exits with `EXIT_SUCCESS` if the help (`-h` or `--help`) or
 * version (`-v` or `--version`) option is present in `argv` or with `EXIT_FAILURE` if any
 * unexpected error occurs while parsing the command line arguments.
 *
 * @param[in] argc Number of command line arguments passed to the test executable.
 * @param[in] argv Command line arguments passed to the test executable.
 */
void scunit_parseArguments(int argc, char** argv);

/**
 * @brief Executes all registered suites (and their tests).
 *
 * @note This function produces diagnostic output on `stdout` and `stderr`, such as the names of
 * suites and tests, results, time measurements, detailed error messages whenever an assertion fails
 * and a summary at the end.
 *
 * It respects the current colored output state set by calling `scunit_setColoredOutput()`.
 * If currently set to `SCUNIT_COLORED_OUTPUT_NEVER`, the default color is used instead.
 *
 * Note that the program immediately exits with `EXIT_FAILURE` if any unexpected error occurs while
 * executing the registered suites.
 *
 * @return `EXIT_FAILURE` if at least one test failed, otherwise `EXIT_SUCCESS`.
 */
int scunit_executeSuites();

#endif