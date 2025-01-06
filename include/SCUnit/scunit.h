#ifndef SCUNIT_H
#define SCUNIT_H

#include <stdint.h>
#include <SCUnit/assert.h>
#include <SCUnit/context.h>
#include <SCUnit/error.h>
#include <SCUnit/memory.h>
#include <SCUnit/print.h>
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

/**
 * @brief Retrieves the version information of SCUnit.
 *
 * @return An `SCUnitVersion` containing the major, minor and patch version number of SCUnit.
 */
SCUnitVersion scunit_getVersion();

/**
 * @brief Registers an `SCUnitSuite` to be run automatically by SCUnit.
 *
 * @note Modifying the suite (i. e. registering setup, teardown or test functions) after it has been
 * registered is allowed.
 *
 * @warning SCUnit takes ownership of the given `SCUnitSuite` and is responsible for deallocating
 * it at the end of `scunit_main()`. You must not deallocate it manually yourself. Do not forget to
 * call `scunit_main()`, otherwise memory leaks will be the consequence.
 *
 * @param[in]  suite `SCUnitSuite` to register.
 * @param[out] error `SCUNIT_ERROR_OUT_OF_MEMORY` if an out-of-memory condition occurred,
 *                   otherwise `SCUNIT_ERROR_NONE`.
 */
void scunit_registerSuite(SCUnitSuite* suite, SCUnitError* error);

/**
 * @brief Main function of SCUnit.
 *
 * @details This function does three things:
 *
 * * First, it parses any command line arguments passed to the test executable and sets up SCUnit
 *   accordingly. Run the test executable with the `-h` or `--help` option to get a list of all
 *   supported options.
 *
 * * It then executes all suites (and their tests) registered either automatically with the
 *   `SCUNIT_SUITE()` and `SCUNIT_PARTIAL_SUITE()` macros or manually by a call to
 *   `scunit_registerSuite()`.
 *
 * * Finally, it deallocates all registered suites.
 *
 * @note It does not make sense to call this function twice, as SCUnit deallocates all suites after
 * running them.
 *
 * @attention If any unexpected error occurs, an error message is printed to `stderr` and the
 * program exits using `EXIT_FAILURE`.
 *
 * Note that the program immediately exits with `EXIT_SUCCESS` if the help (`-h` or `--help`) or
 * version (`-v` or `--version`) option is present in `argv`.
 *
 * @param[in] argc Number of command line arguments passed to the test executable.
 * @param[in] argv Command line arguments passed to the test executable.
 * @return `EXIT_SUCCESS` if all tests passed, otherwise `EXIT_FAILURE`.
 */
int scunit_main(int argc, char** argv);

#endif