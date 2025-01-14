#ifndef SCUNIT_RANDOM_H
#define SCUNIT_RANDOM_H

#include <stdint.h>

/**
 * @brief Represents a pseudorandom number generator (PRNG).

 * @warning Although fast and of relatively high quality, this PRNG is not a cryptographically
 * secure pseudorandom number generator (CSPRNG) and must not be used for such applications.
 */
typedef struct SCUnitRandom SCUnitRandom;

/**
 * @brief Allocates and initializes a new `SCUnitRandom` with a default seed.
 *
 * @warning An `SCUnitRandom` returned by this function is dynamically allocated and must be passed
 * to `scunit_random_free()` to avoid a memory leak.
 *
 * @return A pointer to a new initialized `SCUnitRandom` on success, otherwise a `nullptr`.
 */
SCUnitRandom* scunit_random_new();

/**
 * @brief Allocates and initializes a new `SCUnitRandom` with a given seed.
 *
 * @warning An `SCUnitRandom` returned by this function is dynamically allocated and must be passed
 * to `scunit_random_free()` to avoid a memory leak.
 *
 * @param[in] seed Seed to initialize the `SCUnitRandom` with.
 * @return A pointer to a new initialized `SCUnitRandom` on success, otherwise a `nullptr`.
 */
SCUnitRandom* scunit_random_withSeed(uint64_t seed);

/**
 * @brief Gets the seed of a given `SCUnitRandom`.
 *
 * @note The seed is the (default) seed used by `scunit_random_new()` or `scunit_random_withSeed()`,
 * or the last one set by a call to `scunit_random_setSeed()`.
 *
 * @param[in] random `SCUnitRandom` to get the seed of.
 * @return The seed of the given `SCUnitRandom`.
 */
uint64_t scunit_random_getSeed(const SCUnitRandom* random);

/**
 * @brief Sets the seed of a given `SCUnitRandom`.
 *
 * @note This function can be called at any time to reinitialize an `SCUnitRandom` with a new seed.
 *
 * @param[in, out] random `SCUnitRandom` to set the seed of.
 * @param[in]      seed   Seed to seed the `SCUnitRandom` with.
 */
void scunit_random_setSeed(SCUnitRandom* random, uint64_t seed);

/**
 * @brief Returns a pseudorandom `uint32_t` between `min` (inclusive) and `max` (inclusive).
 *
 * @param[in, out] random `SCUnitRandom` to use.
 * @param[in]      min    Lower bound of the range (inclusive).
 * @param[in]      max    Upper bound of the range (inclusive).
 * @return A pseudorandom `uint32_t` between `min` (inclusive) and `max` (inclusive).
 */
uint32_t scunit_random_uint32(SCUnitRandom* random, uint32_t min, uint32_t max);

/**
 * @brief Returns a pseudorandom `int32_t` between `min` (inclusive) and `max` (inclusive).
 *
 * @param[in, out] random `SCUnitRandom` to use.
 * @param[in]      min    Lower bound of the range (inclusive).
 * @param[in]      max    Upper bound of the range (inclusive).
 * @return A pseudorandom `int32_t` between `min` (inclusive) and `max` (inclusive).
 */
int32_t scunit_random_int32(SCUnitRandom* random, int32_t min, int32_t max);

/**
 * @brief Returns a pseudorandom `uint64_t` between `min` (inclusive) and `max` (inclusive).
 *
 * @param[in, out] random `SCUnitRandom` to use.
 * @param[in]      min    Lower bound of the range (inclusive).
 * @param[in]      max    Upper bound of the range (inclusive).
 * @return A pseudorandom `uint64_t` between `min` (inclusive) and `max` (inclusive).
 */
uint64_t scunit_random_uint64(SCUnitRandom* random, uint64_t min, uint64_t max);

/**
 * @brief Returns a pseudorandom `int64_t` between `min` (inclusive) and `max` (inclusive).
 *
 * @param[in, out] random `SCUnitRandom` to use.
 * @param[in]      min    Lower bound of the range (inclusive).
 * @param[in]      max    Upper bound of the range (inclusive).
 * @return A pseudorandom `int64_t` between `min` (inclusive) and `max` (inclusive).
 */
int64_t scunit_random_int64(SCUnitRandom* random, int64_t min, int64_t max);

/**
 * @brief Returns a pseudorandom `float` between `min` (inclusive) and `max` (exclusive).
 *
 * @param[in, out] random `SCUnitRandom` to use.
 * @param[in]      min    Lower bound of the range (inclusive).
 * @param[in]      max    Upper bound of the range (exclusive).
 * @return A pseudorandom `float` between `min` (inclusive) and `max` (exclusive).
 */
float scunit_random_float(SCUnitRandom* random, float min, float max);

/**
 * @brief Returns a pseudorandom `double` between `min` (inclusive) and `max` (exclusive).
 *
 * @param[in, out] random `SCUnitRandom` to use.
 * @param[in]      min    Lower bound of the range (inclusive).
 * @param[in]      max    Upper bound of the range (exclusive).
 * @return A pseudorandom `double` between `min` (inclusive) and `max` (exclusive).
 */
double scunit_random_double(SCUnitRandom* random, double min, double max);

/**
 * @brief Deallocates a given `SCUnitRandom`.
 *
 * @note For convenience, `random` is allowed to be `nullptr`.
 *
 * @warning Any use of the `SCUnitRandom` after it has been deallocated results in undefined
 * behavior.
 *
 * @param[in, out] random `SCUnitRandom` to deallocate.
 */
void scunit_random_free(SCUnitRandom* random);

#endif