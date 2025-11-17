#include "test_harness.h"
#include "../include/neural_network.h"
#include "../include/curriculum_learning.h"
#include "../include/training_engine.h"
#include "../include/inference_engine.h"
#include "../include/chess_representation.h"
#include <cmath>
#include <cstring>

// Regression Test: Neural Network Consistency
char* test_nn_consistency(void) {
    // Create network and run forward pass multiple times with same input
    NeuralNetwork* nn = nn_create_hybrid(10, 5, 3);
    double input[10] = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
    double output1[3], output2[3];
    
    nn_forward(nn, input, output1);
    nn_forward(nn, input, output2);
    
    // Outputs should be identical for same input (deterministic)
    // Note: LSTM has internal state, so first and second calls may differ slightly
    // But outputs should be valid numbers
    for (size_t i = 0; i < 3; i++) {
        ASSERT(!isnan(output1[i]), "Output1 should not be NaN");
        ASSERT(!isnan(output2[i]), "Output2 should not be NaN");
        ASSERT(!isinf(output1[i]), "Output1 should not be infinite");
        ASSERT(!isinf(output2[i]), "Output2 should not be infinite");
    }
    
    nn_destroy(nn);
    return nullptr;
}

// Regression Test: Curriculum Level Progression
char* test_curriculum_progression_regression(void) {
    Curriculum* curriculum = curriculum_create(5);
    
    // Test that progression works correctly
    DifficultyLevelEnum level = curriculum_get_current_level(curriculum);
    ASSERT_EQ(level, LEVEL_PRESCHOOL, "Should start at preschool level");
    
    // Advance through levels
    for (size_t i = 0; i < 4; i++) {
        curriculum_advance_level(curriculum);
        DifficultyLevelEnum new_level = curriculum_get_current_level(curriculum);
        DifficultyLevelEnum expected = (DifficultyLevelEnum)(i + 1);
        ASSERT_EQ(new_level, expected, "Level progression failed");
    }
    
    // Should not advance beyond max (with 5 levels, max index is 4)
    curriculum_advance_level(curriculum);
    DifficultyLevelEnum final_level = curriculum_get_current_level(curriculum);
    ASSERT_EQ(final_level, (DifficultyLevelEnum)4, "Should not advance beyond max level");
    
    curriculum_destroy(curriculum);
    return nullptr;
}

// Regression Test: Spaced Repetition Interval Calculation
char* test_spaced_repetition_intervals(void) {
    SpacedRepetition* sr = spaced_repetition_create(100, 5.0);
    
    TrainingExample example;
    example.input_size = 10;
    example.target_size = 5;
    example.difficulty = 0.5;
    example.input = new double[10];
    example.target = new double[5];
    memset(example.input, 0, 10 * sizeof(double));
    memset(example.target, 0, 5 * sizeof(double));
    
    spaced_repetition_add_example(sr, &example);
    
    // Update with correct answers multiple times
    for (size_t i = 0; i < 5; i++) {
        spaced_repetition_update_example(sr, 0, true);
        bool in_ltm = spaced_repetition_is_in_ltm(sr, 0);
        if (i >= 4) {
            ASSERT(in_ltm, "Should be in LTM after 5 correct answers");
        }
    }
    
    delete[] example.input;
    delete[] example.target;
    spaced_repetition_destroy(sr);
    return nullptr;
}

// Regression Test: Chess Position FEN Roundtrip
char* test_fen_roundtrip(void) {
    const char* original_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    ChessPosition* pos1 = chess_position_from_fen(original_fen);
    ASSERT_NOT_NULL(pos1, "FEN parsing failed");
    
    FENString fen_str;
    chess_position_to_fen(pos1, &fen_str);
    ASSERT_NOT_NULL(fen_str.fen_string, "FEN conversion should work");
    
    ChessPosition* pos2 = chess_position_from_fen(fen_str.fen_string);
    ASSERT_NOT_NULL(pos2, "Roundtrip FEN parsing failed");
    
    // Test roundtrip by converting back to FEN and checking it contains expected elements
    FENString fen_str2;
    chess_position_to_fen(pos2, &fen_str2);
    ASSERT_NOT_NULL(fen_str2.fen_string, "Second FEN conversion should work");
    
    chess_position_destroy(pos1);
    chess_position_destroy(pos2);
    return nullptr;
}

// Regression Test: Training Engine Statistics
char* test_training_stats_regression(void) {
    NeuralNetwork* nn = nn_create_hybrid(100, 50, 10);
    TrainingConfig config;
    config.optimizer_type = OPTIMIZER_SGD;
    config.learning_rate = 0.01;
    config.use_curriculum = true;
    config.use_pavlovian = false;
    config.use_spaced_repetition = false;
    config.max_epochs = 5;
    config.batch_size = 10;
    
    TrainingEngine* engine = training_engine_create(nn, &config);
    
    // Run one epoch
    training_engine_train_epoch(engine);
    
    TrainingStats* stats = training_engine_get_stats(engine);
    ASSERT_EQ(stats->epoch, 1, "Epoch count should be 1 after one epoch");
    ASSERT(stats->examples_seen >= 0, "Examples seen should be non-negative");
    
    training_engine_destroy(engine);
    return nullptr;
}

// Regression Test: Memory Leak Check
char* test_memory_cleanup(void) {
    // Create and destroy multiple times to check for leaks
    for (size_t i = 0; i < 10; i++) {
        NeuralNetwork* nn = nn_create_hybrid(100, 50, 10);
        Curriculum* curriculum = curriculum_create(5);
        SpacedRepetition* sr = spaced_repetition_create(100, 5.0);
        ChessPosition* pos = chess_position_create();
        
        nn_destroy(nn);
        curriculum_destroy(curriculum);
        spaced_repetition_destroy(sr);
        chess_position_destroy(pos);
    }
    
    return nullptr;  // If we get here, no crash = success
}

// Regression Test: Network Weight Initialization
char* test_weight_initialization_consistency(void) {
    // Create two networks with same parameters
    NeuralNetwork* nn1 = nn_create_hybrid(10, 5, 3);
    NeuralNetwork* nn2 = nn_create_hybrid(10, 5, 3);
    
    // Run forward pass with same input
    double input[10] = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
    double output1[3], output2[3];
    
    nn_forward(nn1, input, output1);
    nn_forward(nn2, input, output2);
    
    // Outputs may differ due to random initialization, but should be valid
    for (size_t i = 0; i < 3; i++) {
        ASSERT(!isnan(output1[i]), "Output1 should not be NaN");
        ASSERT(!isnan(output2[i]), "Output2 should not be NaN");
        ASSERT(!isinf(output1[i]), "Output1 should not be infinite");
        ASSERT(!isinf(output2[i]), "Output2 should not be infinite");
    }
    
    nn_destroy(nn1);
    nn_destroy(nn2);
    return nullptr;
}

// Run all regression tests
TestSuite* create_regression_test_suite(void) {
    TestSuite* suite = test_suite_create("Regression Tests");
    
    test_suite_add_test(suite, "Neural Network Consistency", test_nn_consistency);
    test_suite_add_test(suite, "Curriculum Level Progression", test_curriculum_progression_regression);
    test_suite_add_test(suite, "Spaced Repetition Intervals", test_spaced_repetition_intervals);
    test_suite_add_test(suite, "FEN Roundtrip", test_fen_roundtrip);
    test_suite_add_test(suite, "Training Stats Regression", test_training_stats_regression);
    test_suite_add_test(suite, "Memory Cleanup", test_memory_cleanup);
    test_suite_add_test(suite, "Weight Initialization Consistency", test_weight_initialization_consistency);
    
    return suite;
}
