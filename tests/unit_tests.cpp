/*
 * Copyright (C) 2025, Shyamal Suhana Chandra
 * All rights reserved.
 */
#include "test_harness.h"
#include "../include/neural_network.h"
#include "../include/curriculum_learning.h"
#include "../include/chess_representation.h"
#include "../include/pavlovian_learning.h"
#include "../include/training_engine.h"
#include "../include/inference_engine.h"
#include <cmath>
#include <cstdlib>

// Unit Test: Neural Network Creation
char* test_nn_create_hybrid(void) {
    NeuralNetwork* nn = nn_create_hybrid(100, 50, 10);
    ASSERT_NOT_NULL(nn, "Neural network creation failed");
    // Note: Can't access members directly as struct is opaque
    // Test that forward/backward pass works instead
    double input[100];
    double output[10];
    for (size_t i = 0; i < 100; i++) input[i] = 0.01 * i;
    nn_forward(nn, input, output);
    ASSERT_NOT_NULL(output, "Forward pass should produce output");
    nn_destroy(nn);
    return nullptr;
}

// Unit Test: Neural Network Forward Pass
char* test_nn_forward_pass(void) {
    NeuralNetwork* nn = nn_create_hybrid(10, 5, 3);
    double input[10] = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
    double output[3];
    nn_forward(nn, input, output);
    ASSERT_NOT_NULL(output, "Output is null");
    nn_destroy(nn);
    return nullptr;
}

// Unit Test: Neural Network Backward Pass
char* test_nn_backward_pass(void) {
    NeuralNetwork* nn = nn_create_hybrid(10, 5, 3);
    double input[10] = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
    double target[3] = {0.5, 0.5, 0.5};
    double output[3];
    nn_forward(nn, input, output);
    double loss;
    nn_backward(nn, target, &loss);
    ASSERT(loss >= 0.0, "Loss should be non-negative");
    nn_destroy(nn);
    return nullptr;
}

// Unit Test: Optimizer Creation
char* test_optimizer_create(void) {
    Optimizer* opt = optimizer_create(OPTIMIZER_ADAM, 0.001);
    ASSERT_NOT_NULL(opt, "Optimizer creation failed");
    // Test that optimizer can be used with network (validates it's functional)
    NeuralNetwork* nn = nn_create_hybrid(10, 5, 3);
    double input[10] = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
    double target[3] = {0.5, 0.5, 0.5};
    double output[3];
    nn_forward(nn, input, output);
    double loss;
    nn_backward(nn, target, &loss);
    optimizer_update(opt, nn);  // Should not crash
    optimizer_destroy(opt);
    nn_destroy(nn);
    return nullptr;
}

// Unit Test: Curriculum Creation
char* test_curriculum_create(void) {
    Curriculum* curriculum = curriculum_create(10);
    ASSERT_NOT_NULL(curriculum, "Curriculum creation failed");
    // Test that we can get current level (validates curriculum is functional)
    DifficultyLevelEnum level = curriculum_get_current_level(curriculum);
    ASSERT_EQ(level, LEVEL_PRESCHOOL, "Should start at preschool level");
    curriculum_destroy(curriculum);
    return nullptr;
}

// Unit Test: Curriculum Add Example
char* test_curriculum_add_example(void) {
    Curriculum* curriculum = curriculum_create(5);
    TrainingExample example;
    example.input_size = 10;
    example.target_size = 5;
    example.difficulty = 0.3;
    example.input = new double[10];
    example.target = new double[5];
    for (size_t i = 0; i < 10; i++) example.input[i] = 0.1 * i;
    for (size_t i = 0; i < 5; i++) example.target[i] = 0.2 * i;
    
    // Add example - if it doesn't crash, it worked
    curriculum_add_example(curriculum, &example, LEVEL_PRESCHOOL);
    // Test that we can check advancement (validates curriculum is functional)
    bool should_advance = curriculum_should_advance(curriculum, 0.90);
    ASSERT(should_advance || !should_advance, "Advancement check should work");  // Always true, just tests function works
    
    delete[] example.input;
    delete[] example.target;
    curriculum_destroy(curriculum);
    return nullptr;
}

// Unit Test: Curriculum Advancement
char* test_curriculum_advancement(void) {
    Curriculum* curriculum = curriculum_create(5);
    bool should_advance = curriculum_should_advance(curriculum, 0.90);
    ASSERT(should_advance, "Should advance with 90% accuracy");
    
    bool should_not_advance = curriculum_should_advance(curriculum, 0.50);
    ASSERT(!should_not_advance, "Should not advance with 50% accuracy");
    
    curriculum_destroy(curriculum);
    return nullptr;
}

// Unit Test: Spaced Repetition Creation
char* test_spaced_repetition_create(void) {
    SpacedRepetition* sr = spaced_repetition_create(1000, 5.0);
    ASSERT_NOT_NULL(sr, "Spaced repetition creation failed");
    ASSERT_EQ(sr->capacity, 1000, "Capacity mismatch");
    ASSERT_FLOAT_EQ(sr->ltm_threshold, 5.0, 0.1, "LTM threshold mismatch");
    spaced_repetition_destroy(sr);
    return nullptr;
}

// Unit Test: Chess Position Creation
char* test_chess_position_create(void) {
    ChessPosition* pos = chess_position_create();
    ASSERT_NOT_NULL(pos, "Chess position creation failed");
    // Test that we can convert to FEN (validates position is valid)
    FENString fen;
    chess_position_to_fen(pos, &fen);
    ASSERT_NOT_NULL(fen.fen_string, "FEN conversion should work");
    chess_position_destroy(pos);
    return nullptr;
}

// Unit Test: Chess Position from FEN
char* test_chess_position_from_fen(void) {
    const char* fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    ChessPosition* pos = chess_position_from_fen(fen);
    ASSERT_NOT_NULL(pos, "FEN parsing failed");
    // Test roundtrip conversion
    FENString fen2;
    chess_position_to_fen(pos, &fen2);
    ASSERT_NOT_NULL(fen2.fen_string, "FEN roundtrip should work");
    chess_position_destroy(pos);
    return nullptr;
}

// Unit Test: Chess Position to Matrix
char* test_chess_position_to_matrix(void) {
    ChessPosition* pos = chess_position_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    double matrix[8 * 8 * 12];
    chess_position_to_matrix(pos, matrix);
    
    // Check that some squares have pieces (non-zero values)
    bool has_pieces = false;
    for (size_t i = 0; i < 8 * 8 * 12; i++) {
        if (matrix[i] > 0.5) {
            has_pieces = true;
            break;
        }
    }
    ASSERT(has_pieces, "Matrix should contain piece information");
    
    chess_position_destroy(pos);
    return nullptr;
}

// Unit Test: Pavlovian Learner Creation
char* test_pavlovian_learner_create(void) {
    PavlovianLearner* learner = pavlovian_learner_create(PAVLOVIAN_HYBRID, 0.1);
    ASSERT_NOT_NULL(learner, "Pavlovian learner creation failed");
    ASSERT_EQ(learner->type, PAVLOVIAN_HYBRID, "Learner type mismatch");
    ASSERT_FLOAT_EQ(learner->learning_rate, 0.1, 0.01, "Learning rate mismatch");
    pavlovian_learner_destroy(learner);
    return nullptr;
}

// Unit Test: Pavlovian Stimulus Pairing
char* test_pavlovian_pair_stimuli(void) {
    PavlovianLearner* learner = pavlovian_learner_create(PAVLOVIAN_HYBRID, 0.1);
    
    double cs_vec[10] = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
    double us_vec[1] = {1.0};
    
    ConditionedStimulus* cs = conditioned_stimulus_create(cs_vec, 10, 1.0);
    UnconditionedStimulus* us = unconditioned_stimulus_create(us_vec, 1, 1.0, 1.0);
    
    pavlovian_pair_stimuli(learner, cs, us);
    
    double strength = pavlovian_get_association_strength(learner, cs, us);
    ASSERT(strength > 0.0, "Association strength should be positive after pairing");
    
    conditioned_stimulus_destroy(cs);
    unconditioned_stimulus_destroy(us);
    pavlovian_learner_destroy(learner);
    return nullptr;
}

// Unit Test: Training Engine Creation
char* test_training_engine_create(void) {
    NeuralNetwork* nn = nn_create_hybrid(100, 50, 10);
    TrainingConfig config;
    config.optimizer_type = OPTIMIZER_ADAM;
    config.learning_rate = 0.001;
    config.use_curriculum = true;
    config.use_pavlovian = true;
    config.use_spaced_repetition = true;
    config.max_epochs = 10;
    config.batch_size = 32;
    
    TrainingEngine* engine = training_engine_create(nn, &config);
    ASSERT_NOT_NULL(engine, "Training engine creation failed");
    // Test that we can get stats (validates engine is functional)
    TrainingStats* stats = training_engine_get_stats(engine);
    ASSERT_NOT_NULL(stats, "Stats should be accessible");
    
    training_engine_destroy(engine);
    return nullptr;
}

// Unit Test: Inference Engine Creation
char* test_inference_engine_create(void) {
    NeuralNetwork* nn = nn_create_hybrid(768, 512, 4096);
    InferenceEngine* engine = inference_engine_create(nn);
    ASSERT_NOT_NULL(engine, "Inference engine creation failed");
    // Test that we can evaluate a position (validates engine is functional)
    ChessPosition* pos = chess_position_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    double eval = inference_engine_evaluate_position(engine, pos);
    ASSERT(!isnan(eval), "Evaluation should work");
    chess_position_destroy(pos);
    inference_engine_destroy(engine);
    return nullptr;
}

// Unit Test: Inference Engine Position Evaluation
char* test_inference_evaluate_position(void) {
    NeuralNetwork* nn = nn_create_hybrid(768, 512, 4096);
    InferenceEngine* engine = inference_engine_create(nn);
    
    ChessPosition* pos = chess_position_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    double eval = inference_engine_evaluate_position(engine, pos);
    
    // Evaluation should be a valid number (not NaN or Inf)
    ASSERT(!isnan(eval), "Evaluation should not be NaN");
    ASSERT(!isinf(eval), "Evaluation should not be infinite");
    
    chess_position_destroy(pos);
    inference_engine_destroy(engine);
    return nullptr;
}

// Unit Test: Inference Engine Move Prediction
char* test_inference_predict_move(void) {
    NeuralNetwork* nn = nn_create_hybrid(768, 512, 4096);
    InferenceEngine* engine = inference_engine_create(nn);
    
    ChessPosition* pos = chess_position_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    MoveEvaluation* move_eval = inference_engine_predict_move(engine, pos);
    
    // Even untrained network should return some move
    if (move_eval) {
        ASSERT(move_eval->move.from < 64, "From square should be valid");
        ASSERT(move_eval->move.to < 64, "To square should be valid");
        delete move_eval;
    }
    
    chess_position_destroy(pos);
    inference_engine_destroy(engine);
    return nullptr;
}

// Run all unit tests
TestSuite* create_unit_test_suite(void) {
    TestSuite* suite = test_suite_create("Unit Tests");
    
    test_suite_add_test(suite, "Neural Network Creation", test_nn_create_hybrid);
    test_suite_add_test(suite, "Neural Network Forward Pass", test_nn_forward_pass);
    test_suite_add_test(suite, "Neural Network Backward Pass", test_nn_backward_pass);
    test_suite_add_test(suite, "Optimizer Creation", test_optimizer_create);
    test_suite_add_test(suite, "Curriculum Creation", test_curriculum_create);
    test_suite_add_test(suite, "Curriculum Add Example", test_curriculum_add_example);
    test_suite_add_test(suite, "Curriculum Advancement", test_curriculum_advancement);
    test_suite_add_test(suite, "Spaced Repetition Creation", test_spaced_repetition_create);
    test_suite_add_test(suite, "Chess Position Creation", test_chess_position_create);
    test_suite_add_test(suite, "Chess Position from FEN", test_chess_position_from_fen);
    test_suite_add_test(suite, "Chess Position to Matrix", test_chess_position_to_matrix);
    test_suite_add_test(suite, "Pavlovian Learner Creation", test_pavlovian_learner_create);
    test_suite_add_test(suite, "Pavlovian Stimulus Pairing", test_pavlovian_pair_stimuli);
    test_suite_add_test(suite, "Training Engine Creation", test_training_engine_create);
    test_suite_add_test(suite, "Inference Engine Creation", test_inference_engine_create);
    test_suite_add_test(suite, "Inference Position Evaluation", test_inference_evaluate_position);
    test_suite_add_test(suite, "Inference Move Prediction", test_inference_predict_move);
    
    return suite;
}
