/**
 * @file scunit.h
 *
 * @brief Main header of SCUnit to be included in tests.
 *
 * @note If you want to take advantage of SCUnit's automatic test registration and execution
 * mechanism, this header also declares two important functions required for that. See below for
 * more details.
 *
 * @author Philipp Raschka
 */

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
SCUnitVersion scunit_version();

/**
 * @brief Registers an `SCUnitSuite` to be run automatically by SCUnit.
 *
 * @note Modifying the suite (i. e. registering setup, teardown or test functions) after it has been
 * registered is fine.
 *
 * @warning SCUnit takes ownership of the given `SCUnitSuite` and is responsible for deallocating
 * it at the end of `scunit_main()`. You must not deallocate it manually yourself. Do not forget to
 * call `scunit_main()`, otherwise memory leaks will be the consequence.
 *
 * @param[in] suite `SCUnitSuite` to register.
 * @return `SCUNIT_ERROR_ARGUMENT_NULL` if `suite` is `nullptr`, `SCUNIT_ERROR_OUT_OF_MEMORY` if an
 * out-of-memory condition occurred and `SCUNIT_ERROR_NONE` otherwise.
 */
SCUnitError scunit_registerSuite(SCUnitSuite* suite);

/**
 * @brief Main function of SCUnit.
 *
 * @details This function does three things. First, it parses any command line arguments passed to
 * the test executable and sets up SCUnit accordingly. Run the test executable with the `-h` or
 * `--help` option to get a list of all supported options.
 *
 * It then executes all suites (and their tests) registered either automatically with the
 * `SCUNIT_SUITE()` and `SCUNIT_PARTIAL_SUITE()` macros or by a call to `scunit_registerSuite()`.
 *
 * Finally, it deallocates all registered suites.
 *
 * @note It does not make sense to call this function twice, as SCUnit deallocates all suites after
 * running them.
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
 * @return `EXIT_SUCCESS` if all tests passed, otherwise `EXIT_FAILURE`.
 */
int scunit_main(int argc, char** argv);

#endif