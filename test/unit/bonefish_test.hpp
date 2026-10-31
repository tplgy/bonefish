#ifndef BONEFISH_TEST_HPP
#define BONEFISH_TEST_HPP

#include <cstdio>
#include <string>

static int g_test_failures = 0;
static int g_test_checks = 0;

#define CHECK(cond) \
    do { \
        ++g_test_checks; \
        if (!(cond)) { \
            ++g_test_failures; \
            std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        } \
    } while (0)

#define CHECK_EQ(a, b) \
    do { \
        ++g_test_checks; \
        if (!((a) == (b))) { \
            ++g_test_failures; \
            std::fprintf(stderr, "FAIL %s:%d: %s == %s\n", __FILE__, __LINE__, #a, #b); \
        } \
    } while (0)

static inline int test_summary(const char* suite)
{
    std::fprintf(stderr, "%s: %d checks, %d failures\n", suite, g_test_checks, g_test_failures);
    return g_test_failures == 0 ? 0 : 1;
}

#endif // BONEFISH_TEST_HPP
