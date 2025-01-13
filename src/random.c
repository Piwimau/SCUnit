#include <time.h>
#include <SCUnit/memory.h>
#include <SCUnit/random.h>

struct SCUnitRandom {

    /** @brief Seed used to initialize the state of this `SCUnitRandom`. */
    uint64_t seed;

    /**
     * @brief State of this `SCUnitRandom`.
     *
     * @note This is an implementation of the `xoshiro256**` PRNG by David Blackman and Sebastiano
     * Vigna, which has a state space of (4 * 64 = 256 bits). See [here](https://prng.di.unimi.it/)
     * for more information.
     */
    uint64_t state[4];

};

SCUnitRandom* scunit_random_new(SCUnitError* error) {
    return scunit_random_withSeed((uint64_t) time(nullptr), error);
}

SCUnitRandom* scunit_random_withSeed(uint64_t seed, SCUnitError* error) {
    SCUnitRandom* random = SCUNIT_MALLOC(sizeof(SCUnitRandom));
    if (random == nullptr) {
        *error = SCUNIT_ERROR_OUT_OF_MEMORY;
        return nullptr;
    }
    scunit_random_setSeed(random, seed);
    *error = SCUNIT_ERROR_NONE;
    return random;
}

uint64_t scunit_random_getSeed(const SCUnitRandom* random) {
    return random->seed;
}

void scunit_random_setSeed(SCUnitRandom* random, uint64_t seed) {
    random->seed = seed;
    for (int32_t i = 0; i < 4; i++) {
        uint64_t result = (seed += 0x9E3779B97F4A7C15);
        result = (result ^ (result >> 30)) * 0xBF58476D1CE4E5B9;
        result = (result ^ (result >> 27)) * 0x94D049BB133111EB;
        random->state[i] = result ^ (result >> 31);
    }
}

/**
 * @brief Rotates a given value to the left by a specified number of bits.
 *
 * @param[in] value Value to rotate to the left.
 * @param[in] bits  Number of bits by which the value should be rotated to the left.
 * @return The given value rotated to the left by the specified number of bits.
 */
static inline uint64_t rotateLeft(uint64_t value, int32_t bits) {
    return (value << bits) | (value >> (64 - bits));
}

/**
 * @brief Advances the state of a given `SCUnitRandom`.
 *
 * @return A pseudorandom `double` between `0.0` (inclusive) and `1.0` (exclusive).
 */
static inline double next(SCUnitRandom* random) {
    uint64_t result = rotateLeft(random->state[1] * 5, 7) * 9;
    uint64_t temp = random->state[1] << 17;
    random->state[2] ^= random->state[0];
    random->state[3] ^= random->state[1];
    random->state[1] ^= random->state[2];
    random->state[0] ^= random->state[3];
    random->state[2] ^= temp;
    random->state[3] = rotateLeft(random->state[3], 45);
    return (result >> 11) * (1.0 / (1ULL << 53));
}

uint32_t scunit_random_uint32(SCUnitRandom* random, uint32_t min, uint32_t max) {
    return min + (uint32_t) (next(random) * (max - min + 1));
}

int32_t scunit_random_int32(SCUnitRandom* random, int32_t min, int32_t max) {
    return min + (int32_t) (next(random) * (max - min + 1));
}

uint64_t scunit_random_uint64(SCUnitRandom* random, uint64_t min, uint64_t max) {
    return min + (uint64_t) (next(random) * (max - min + 1));
}

int64_t scunit_random_int64(SCUnitRandom* random, int64_t min, int64_t max) {
    return min + (int64_t) (next(random) * (max - min + 1));
}

float scunit_random_float(SCUnitRandom* random, float min, float max) {
    return min + (float) (next(random) * (max - min));
}

double scunit_random_double(SCUnitRandom* random, double min, double max) {
    return min + (next(random) * (max - min));
}

void scunit_random_free(SCUnitRandom* random) {
    SCUNIT_FREE(random);
}