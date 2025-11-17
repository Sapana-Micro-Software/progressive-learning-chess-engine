#include "test_harness.h"
#include <stdlib.h>
#include <stdarg.h>
#include <cstring>
#include <ctime>
#include <cstdio>

TestSuite* test_suite_create(const char* name) {
    TestSuite* suite = new TestSuite;
    suite->suite_name = name;
    suite->tests = nullptr;
    suite->results = nullptr;
    suite->num_tests = 0;
    suite->passed_count = 0;
    suite->failed_count = 0;
    suite->total_time = 0.0;
    return suite;
}

void test_suite_destroy(TestSuite* suite) {
    if (suite) {
        if (suite->results) {
            for (size_t i = 0; i < suite->num_tests; i++) {
                if (suite->results[i].error_message) {
                    free((void*)suite->results[i].error_message);
                }
            }
            delete[] suite->results;
        }
        if (suite->tests) {
            delete[] suite->tests;
        }
        delete suite;
    }
}

void test_suite_add_test(TestSuite* suite, const char* test_name, TestFunction test_func) {
    // Resize tests array
    TestEntry* new_tests = new TestEntry[suite->num_tests + 1];
    if (suite->tests) {
        memcpy(new_tests, suite->tests, suite->num_tests * sizeof(TestEntry));
        delete[] suite->tests;
    }
    suite->tests = new_tests;
    
    // Add new test entry
    TestEntry* entry = &suite->tests[suite->num_tests];
    entry->test_name = test_name;
    entry->test_func = test_func;
    
    suite->num_tests++;
}

void test_suite_run(TestSuite* suite) {
    printf("Running test suite: %s\n", suite->suite_name);
    printf("========================================\n");
    
    // Allocate results array
    suite->results = new TestResult[suite->num_tests];
    
    clock_t start_total = clock();
    
    for (size_t i = 0; i < suite->num_tests; i++) {
        TestEntry* entry = &suite->tests[i];
        TestResult* result = &suite->results[i];
        
        result->test_name = entry->test_name;
        result->passed = false;
        result->error_message = nullptr;
        result->execution_time = 0.0;
        
        clock_t start = clock();
        char* error = entry->test_func ? entry->test_func() : nullptr;
        clock_t end = clock();
        
        result->execution_time = ((double)(end - start)) / CLOCKS_PER_SEC;
        result->error_message = error;
        result->passed = (error == nullptr);
        
        if (result->passed) {
            suite->passed_count++;
            printf("✓ %s (%.4f s)\n", result->test_name, result->execution_time);
        } else {
            suite->failed_count++;
            printf("✗ %s (%.4f s)\n", result->test_name, result->execution_time);
            if (error) {
                printf("  Error: %s\n", error);
            }
        }
    }
    
    clock_t end_total = clock();
    suite->total_time = ((double)(end_total - start_total)) / CLOCKS_PER_SEC;
    
    printf("========================================\n");
    printf("Passed: %zu, Failed: %zu, Total: %zu (%.4f s)\n\n",
           suite->passed_count, suite->failed_count, suite->num_tests, suite->total_time);
}

void test_suite_print_results(TestSuite* suite) {
    test_suite_run(suite);
}
