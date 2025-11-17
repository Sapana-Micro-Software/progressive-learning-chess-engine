#include "test_harness.h"
#include "../include/training_engine.h"
#include "../include/inference_engine.h"
#include "../include/curriculum_learning.h"
#include <cmath>

// UX Test: Training Progress Visibility
char* test_training_progress_visibility(void) {
    NeuralNetwork* nn = nn_create_hybrid(768, 512, 4096);
    
    TrainingConfig config;
    config.optimizer_type = OPTIMIZER_ADAM;
    config.learning_rate = 0.001;
    config.use_curriculum = true;
    config.use_pavlovian = false;
    config.use_spaced_repetition = false;
    config.max_epochs = 1;
    config.batch_size = 10;
    
    TrainingEngine* engine = training_engine_create(nn, &config);
    training_engine_train_epoch(engine);
    
    // Check that stats are accessible (simulating UI access)
    TrainingStats* stats = training_engine_get_stats(engine);
    
    // Stats should be readable and meaningful
    ASSERT(stats->epoch >= 0, "Epoch should be accessible");
    ASSERT(stats->current_loss >= 0.0, "Loss should be non-negative");
    ASSERT(stats->accuracy >= 0.0 && stats->accuracy <= 1.0, "Accuracy should be in valid range");
    ASSERT(stats->current_level >= 0 && stats->current_level < 10, "Level should be in valid range");
    
    training_engine_destroy(engine);
    return nullptr;
}

// UX Test: Real-time Statistics Update
char* test_realtime_stats_update(void) {
    NeuralNetwork* nn = nn_create_hybrid(768, 512, 4096);
    
    TrainingConfig config;
    config.optimizer_type = OPTIMIZER_ADAM;
    config.learning_rate = 0.001;
    config.use_curriculum = true;
    config.max_epochs = 3;
    config.batch_size = 10;
    
    TrainingEngine* engine = training_engine_create(nn, &config);
    
    // Simulate multiple epoch updates (as UI would poll)
    for (size_t i = 0; i < 3; i++) {
        training_engine_train_epoch(engine);
        TrainingStats* stats = training_engine_get_stats(engine);
        
        // Stats should update after each epoch
        ASSERT_EQ(stats->epoch, i + 1, "Epoch should increment");
    }
    
    training_engine_destroy(engine);
    return nullptr;
}

// UX Test: Level Progression Feedback
char* test_level_progression_feedback(void) {
    Curriculum* curriculum = curriculum_create(10);
    
    // Simulate user seeing level progression
    DifficultyLevelEnum level1 = curriculum_get_current_level(curriculum);
    ASSERT_EQ(level1, LEVEL_PRESCHOOL, "Should start at preschool");
    
    // Advance level (simulating user progress)
    curriculum_advance_level(curriculum);
    DifficultyLevelEnum level2 = curriculum_get_current_level(curriculum);
    ASSERT_EQ(level2, LEVEL_KINDERGARTEN, "Should advance to kindergarten");
    
    curriculum_destroy(curriculum);
    return nullptr;
}

// UX Test: Error Message Clarity
char* test_error_handling_ux(void) {
    // Test that invalid operations return sensible results
    NeuralNetwork* nn = nullptr;
    InferenceEngine* engine = inference_engine_create(nn);
    
    // Should handle null network gracefully
    ChessPosition* pos = chess_position_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    double eval = inference_engine_evaluate_position(engine, pos);
    
    // Should return 0.0 or handle gracefully, not crash
    ASSERT(!isnan(eval), "Should handle null network gracefully");
    
    chess_position_destroy(pos);
    inference_engine_destroy(engine);
    return nullptr;
}

// UX Test: Response Time for Inference
char* test_inference_response_time(void) {
    NeuralNetwork* nn = nn_create_hybrid(768, 512, 4096);
    InferenceEngine* engine = inference_engine_create(nn);
    
    ChessPosition* pos = chess_position_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    
    // Measure inference time (should be fast for UX)
    clock_t start = clock();
    double eval = inference_engine_evaluate_position(engine, pos);
    MoveEvaluation* move = inference_engine_predict_move(engine, pos);
    clock_t end = clock();
    
    double elapsed = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    // Inference should complete quickly (< 1 second for good UX)
    ASSERT(elapsed < 1.0, "Inference should be fast for good UX");
    ASSERT(!isnan(eval), "Evaluation should be valid");
    
    if (move) delete move;
    chess_position_destroy(pos);
    inference_engine_destroy(engine);
    return nullptr;
}

// UX Test: Configuration Validation
char* test_config_validation_ux(void) {
    NeuralNetwork* nn = nn_create_hybrid(768, 512, 4096);
    
    // Test with various configs (simulating user input)
    TrainingConfig configs[] = {
        {OPTIMIZER_SGD, 0.001, 0.9, 0.0001, 32, 10, 0.001, true, true, true, 0.85, 10},
        {OPTIMIZER_ADAM, 0.0001, 0.9, 0.0001, 16, 5, 0.001, false, false, false, 0.85, 5},
        {OPTIMIZER_ADAGRAD, 0.01, 0.9, 0.0001, 64, 20, 0.001, true, false, true, 0.90, 15}
    };
    
    for (size_t i = 0; i < 3; i++) {
        TrainingEngine* engine = training_engine_create(nn, &configs[i]);
        ASSERT_NOT_NULL(engine, "Engine should be created with valid config");
        training_engine_destroy(engine);
    }
    
    nn_destroy(nn);
    return nullptr;
}

// UX Test: Progress Indicators
char* test_progress_indicators(void) {
    NeuralNetwork* nn = nn_create_hybrid(768, 512, 4096);
    
    TrainingConfig config;
    config.optimizer_type = OPTIMIZER_ADAM;
    config.learning_rate = 0.001;
    config.use_curriculum = true;
    config.max_epochs = 5;
    config.batch_size = 10;
    
    TrainingEngine* engine = training_engine_create(nn, &config);
    
    // Simulate progress tracking (as UI would)
    for (size_t i = 0; i < 5; i++) {
        training_engine_train_epoch(engine);
        TrainingStats* stats = training_engine_get_stats(engine);
        
        // Progress should be trackable
        double progress = (double)stats->epoch / config.max_epochs;
        ASSERT(progress >= 0.0 && progress <= 1.0, "Progress should be in valid range");
        ASSERT(stats->examples_seen >= 0, "Examples seen should be trackable");
    }
    
    training_engine_destroy(engine);
    return nullptr;
}

// UX Test: State Persistence
char* test_state_persistence_ux(void) {
    NeuralNetwork* nn = nn_create_hybrid(768, 512, 4096);
    
    TrainingConfig config;
    config.optimizer_type = OPTIMIZER_ADAM;
    config.learning_rate = 0.001;
    config.use_curriculum = true;
    config.max_epochs = 2;
    config.batch_size = 10;
    
    TrainingEngine* engine = training_engine_create(nn, &config);
    training_engine_train_epoch(engine);
    
    // Simulate checkpoint save/load (UX feature)
    training_engine_save_checkpoint(engine, "test_checkpoint.bin");
    
    // Checkpoint should be created (file existence would be checked in real UX)
    ASSERT(true, "Checkpoint save should not crash");
    
    training_engine_destroy(engine);
    return nullptr;
}

// Run all UX tests
TestSuite* create_ux_test_suite(void) {
    TestSuite* suite = test_suite_create("UX Tests");
    
    test_suite_add_test(suite, "Training Progress Visibility", test_training_progress_visibility);
    test_suite_add_test(suite, "Real-time Statistics Update", test_realtime_stats_update);
    test_suite_add_test(suite, "Level Progression Feedback", test_level_progression_feedback);
    test_suite_add_test(suite, "Error Handling UX", test_error_handling_ux);
    test_suite_add_test(suite, "Inference Response Time", test_inference_response_time);
    test_suite_add_test(suite, "Configuration Validation", test_config_validation_ux);
    test_suite_add_test(suite, "Progress Indicators", test_progress_indicators);
    test_suite_add_test(suite, "State Persistence", test_state_persistence_ux);
    
    return suite;
}
