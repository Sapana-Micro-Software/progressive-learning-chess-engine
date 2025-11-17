# Test Suite Documentation

This directory contains comprehensive tests for the Curriculum Chess Learning System.

## Test Types

### 1. Unit Tests (`unit_tests.cpp`)
Tests individual components and functions in isolation:
- Neural network creation and operations
- Curriculum learning system
- Spaced repetition system
- Chess position representation
- Pavlovian learning
- Training and inference engines

**Run:** Tests verify that each component works correctly on its own.

### 2. Regression Tests (`regression_tests.cpp`)
Ensures existing functionality doesn't break after changes:
- Network consistency across multiple runs
- Curriculum level progression
- Spaced repetition interval calculations
- FEN string roundtrip conversion
- Training statistics accuracy
- Memory leak detection
- Weight initialization consistency

**Run:** Prevents regressions when modifying code.

### 3. A-B Tests (`ab_tests.cpp`)
Compares different implementations and configurations:
- SGD vs Adam optimizer performance
- Curriculum vs non-curriculum learning
- Pavlovian vs non-Pavlovian learning
- Different learning rates
- Layer type comparisons
- Spaced repetition effectiveness

**Run:** Helps choose optimal configurations.

### 4. Blackbox Tests (`blackbox_tests.cpp`)
Tests the system from external interface without knowing internals:
- End-to-end training and inference pipeline
- Multiple position evaluations
- Curriculum progression
- Batch inference
- Move generation and evaluation
- Full feature training
- Evaluation consistency

**Run:** Validates system behavior from user perspective.

### 5. UX Tests (`ux_tests.cpp`)
Tests user experience and interface aspects:
- Training progress visibility
- Real-time statistics updates
- Level progression feedback
- Error handling clarity
- Inference response time
- Configuration validation
- Progress indicators
- State persistence

**Run:** Ensures good user experience.

## Running Tests

### Build and Run All Tests
```bash
make test
```

### Run Specific Test Suite
```bash
./test_runner  # Runs all test suites
```

### Test Output Format
```
Running test suite: Unit Tests
========================================
✓ Neural Network Creation (0.0012 s)
✓ Neural Network Forward Pass (0.0008 s)
✗ Some Test (0.0005 s)
  Error: Expected value mismatch
========================================
Passed: 14, Failed: 1, Total: 15 (0.0234 s)
```

## Test Assertions

The test harness provides several assertion macros:

- `ASSERT(condition, message)` - Assert condition is true
- `ASSERT_EQ(actual, expected, message)` - Assert equality
- `ASSERT_FLOAT_EQ(actual, expected, epsilon, message)` - Assert float equality with tolerance
- `ASSERT_NE(actual, expected, message)` - Assert not equal
- `ASSERT_NOT_NULL(ptr, message)` - Assert pointer is not null
- `ASSERT_NULL(ptr, message)` - Assert pointer is null

## Adding New Tests

1. Create test function returning `char*` (nullptr on success, error message on failure):
```cpp
char* test_my_feature(void) {
    // Test code
    ASSERT(condition, "Error message");
    return nullptr;  // Success
}
```

2. Add to appropriate test suite:
```cpp
test_suite_add_test(suite, "My Feature Test", test_my_feature);
```

## Test Coverage

- **Neural Networks**: Creation, forward/backward pass, optimization
- **Curriculum Learning**: Level progression, example management
- **Spaced Repetition**: Interval calculation, LTM transition
- **Chess Representation**: FEN parsing, matrix conversion
- **Pavlovian Learning**: Stimulus pairing, association strength
- **Training Engine**: Configuration, statistics, checkpointing
- **Inference Engine**: Position evaluation, move prediction

## Continuous Integration

Tests are designed to run in CI/CD pipelines:
- Exit code 0 on all tests passing
- Exit code 1 on any test failure
- Detailed output for debugging
- Fast execution (< 1 second for most tests)

## Performance Benchmarks

Tests also serve as performance benchmarks:
- Inference response time (< 1s for UX)
- Training epoch time
- Memory usage patterns
- Batch processing speed
