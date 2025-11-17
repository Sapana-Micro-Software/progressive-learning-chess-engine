/*
 * Copyright (C) 2025, Shyamal Suhana Chandra
 * All rights reserved.
 */
#include "test_harness.h"
#include <stdio.h>
#include <stdlib.h>

// Forward declarations
TestSuite* create_unit_test_suite(void);
TestSuite* create_regression_test_suite(void);
TestSuite* create_ab_test_suite(void);
TestSuite* create_blackbox_test_suite(void);
TestSuite* create_ux_test_suite(void);

void run_all_tests(void) {
    printf("========================================\n");
    printf("Curriculum Chess Learning System Tests\n");
    printf("========================================\n\n");
    
    size_t total_passed = 0;
    size_t total_failed = 0;
    double total_time = 0.0;
    
    // Run unit tests
    TestSuite* unit_suite = create_unit_test_suite();
    test_suite_run(unit_suite);
    total_passed += unit_suite->passed_count;
    total_failed += unit_suite->failed_count;
    total_time += unit_suite->total_time;
    test_suite_destroy(unit_suite);
    
    // Run regression tests
    TestSuite* regression_suite = create_regression_test_suite();
    test_suite_run(regression_suite);
    total_passed += regression_suite->passed_count;
    total_failed += regression_suite->failed_count;
    total_time += regression_suite->total_time;
    test_suite_destroy(regression_suite);
    
    // Run A-B tests
    TestSuite* ab_suite = create_ab_test_suite();
    test_suite_run(ab_suite);
    total_passed += ab_suite->passed_count;
    total_failed += ab_suite->failed_count;
    total_time += ab_suite->total_time;
    test_suite_destroy(ab_suite);
    
    // Run blackbox tests
    TestSuite* blackbox_suite = create_blackbox_test_suite();
    test_suite_run(blackbox_suite);
    total_passed += blackbox_suite->passed_count;
    total_failed += blackbox_suite->failed_count;
    total_time += blackbox_suite->total_time;
    test_suite_destroy(blackbox_suite);
    
    // Run UX tests
    TestSuite* ux_suite = create_ux_test_suite();
    test_suite_run(ux_suite);
    total_passed += ux_suite->passed_count;
    total_failed += ux_suite->failed_count;
    total_time += ux_suite->total_time;
    test_suite_destroy(ux_suite);
    
    // Print summary
    printf("========================================\n");
    printf("TEST SUMMARY\n");
    printf("========================================\n");
    printf("Total Passed: %zu\n", total_passed);
    printf("Total Failed: %zu\n", total_failed);
    printf("Total Tests: %zu\n", total_passed + total_failed);
    printf("Total Time: %.4f seconds\n", total_time);
    printf("========================================\n");
    
    if (total_failed == 0) {
        printf("✓ All tests passed!\n");
        exit(0);
    } else {
        printf("✗ Some tests failed\n");
        exit(1);
    }
}

int main(int argc, char* argv[]) {
    run_all_tests();
    return 0;
}
