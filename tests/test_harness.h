/*
 * Copyright (C) 2025, Shyamal Suhana Chandra
 * All rights reserved.
 */
#ifndef TEST_HARNESS_H
#define TEST_HARNESS_H

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

// Test function type (must be defined before use)
typedef char* (*TestFunction)(void);

// Test result structure
typedef struct {
    const char* test_name;
    bool passed;
    const char* error_message;
    double execution_time;
} TestResult;

// Test entry structure
typedef struct {
    const char* test_name;
    TestFunction test_func;
} TestEntry;

// Test suite structure
typedef struct {
    const char* suite_name;
    TestEntry* tests;
    TestResult* results;
    size_t num_tests;
    size_t passed_count;
    size_t failed_count;
    double total_time;
} TestSuite;

// Test macros
#define ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            return strdup(message); \
        } \
    } while(0)

#define ASSERT_EQ(actual, expected, message) \
    do { \
        if ((actual) != (expected)) { \
            char* msg = (char*)malloc(256); \
            snprintf(msg, 256, "%s: expected %ld, got %ld", message, (long)(expected), (long)(actual)); \
            return msg; \
        } \
    } while(0)

#define ASSERT_FLOAT_EQ(actual, expected, epsilon, message) \
    do { \
        double diff = (actual) - (expected); \
        if (diff < 0) diff = -diff; \
        if (diff > (epsilon)) { \
            char* msg = (char*)malloc(256); \
            snprintf(msg, 256, "%s: expected %.6f, got %.6f", message, (expected), (actual)); \
            return msg; \
        } \
    } while(0)

#define ASSERT_NE(actual, expected, message) \
    do { \
        if ((actual) == (expected)) { \
            return strdup(message); \
        } \
    } while(0)

#define ASSERT_NOT_NULL(ptr, message) \
    do { \
        if ((ptr) == NULL) { \
            return strdup(message); \
        } \
    } while(0)

#define ASSERT_NULL(ptr, message) \
    do { \
        if ((ptr) != NULL) { \
            return strdup(message); \
        } \
    } while(0)

// Test harness functions
TestSuite* test_suite_create(const char* name);
void test_suite_destroy(TestSuite* suite);
void test_suite_add_test(TestSuite* suite, const char* test_name, TestFunction test_func);
void test_suite_run(TestSuite* suite);
void test_suite_print_results(TestSuite* suite);

// Test runner
void run_all_tests(void);

#ifdef __cplusplus
}
#endif

#endif // TEST_HARNESS_H
