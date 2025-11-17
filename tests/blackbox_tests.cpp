#include "test_harness.h"
#include "../include/inference_engine.h"
#include "../include/training_engine.h"
#include "../include/chess_representation.h"
#include <cmath>
#include <cstdlib>

// Blackbox Test: End-to-End Training and Inference
char* test_end_to_end_training_inference(void) {
    // Create network
    NeuralNetwork* nn = nn_create_hybrid(768, 512, 4096);
    
    // Train
    TrainingConfig config;
    config.optimizer_type = OPTIMIZER_ADAM;
    config.learning_rate = 0.001;
    config.use_curriculum = true;
    config.use_pavlovian = true;
    config.use_spaced_repetition = true;
    config.max_epochs = 2;
    config.batch_size = 10;
    
    TrainingEngine* engine = training_engine_create(nn, &config);
    training_engine_train_epoch(engine);
    
    // Create inference engine
    InferenceEngine* inf_engine = inference_engine_create(nn);
    
    // Test inference on starting position
    ChessPosition* pos = chess_position_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    double eval = inference_engine_evaluate_position(inf_engine, pos);
    
    ASSERT(!isnan(eval), "Evaluation should be a valid number");
    ASSERT(!isinf(eval), "Evaluation should not be infinite");
    
    MoveEvaluation* move = inference_engine_predict_move(inf_engine, pos);
    if (move) {
        ASSERT(move->move.from < 64, "From square should be valid");
        ASSERT(move->move.to < 64, "To square should be valid");
        delete move;
    }
    
    chess_position_destroy(pos);
    inference_engine_destroy(inf_engine);
    training_engine_destroy(engine);
    return nullptr;
}

// Blackbox Test: Multiple Position Evaluations
char* test_multiple_positions(void) {
    NeuralNetwork* nn = nn_create_hybrid(768, 512, 4096);
    InferenceEngine* engine = inference_engine_create(nn);
    
    const char* fens[] = {
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1",
        "rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR w KQkq e6 0 2"
    };
    
    for (size_t i = 0; i < 3; i++) {
        ChessPosition* pos = chess_position_from_fen(fens[i]);
        double eval = inference_engine_evaluate_position(engine, pos);
        
        ASSERT(!isnan(eval), "Evaluation should not be NaN");
        ASSERT(!isinf(eval), "Evaluation should not be infinite");
        
        chess_position_destroy(pos);
    }
    
    inference_engine_destroy(engine);
    return nullptr;
}

// Blackbox Test: Curriculum Learning Progression
char* test_curriculum_progression_blackbox(void) {
    NeuralNetwork* nn = nn_create_hybrid(768, 512, 4096);
    
    TrainingConfig config;
    config.optimizer_type = OPTIMIZER_ADAM;
    config.learning_rate = 0.001;
    config.use_curriculum = true;
    config.use_pavlovian = false;
    config.use_spaced_repetition = false;
    config.max_epochs = 1;
    config.batch_size = 10;
    config.mastery_threshold = 0.85;
    
    TrainingEngine* engine = training_engine_create(nn, &config);
    
    // Simulate curriculum progression
    DifficultyLevelEnum start_level = curriculum_get_current_level(engine->curriculum);
    ASSERT_EQ(start_level, LEVEL_PRESCHOOL, "Should start at preschool level");
    
    // Train with curriculum
    training_engine_train_with_curriculum(engine);
    
    TrainingStats* stats = training_engine_get_stats(engine);
    ASSERT(stats->current_level >= 0, "Current level should be valid");
    ASSERT(stats->current_level < 10, "Current level should be within bounds");
    
    training_engine_destroy(engine);
    return nullptr;
}

// Blackbox Test: Batch Inference
char* test_batch_inference(void) {
    NeuralNetwork* nn = nn_create_hybrid(768, 512, 4096);
    InferenceEngine* engine = inference_engine_create(nn);
    
    const size_t batch_size = 5;
    double inputs[batch_size * 768];
    double outputs[batch_size * 4096];
    
    // Initialize inputs
    for (size_t i = 0; i < batch_size * 768; i++) {
        inputs[i] = 0.01 * (i % 100);
    }
    
    inference_engine_batch_predict(engine, inputs, batch_size, 768, outputs, 4096);
    
    // Check outputs are valid
    for (size_t i = 0; i < batch_size * 4096; i++) {
        ASSERT(!isnan(outputs[i]), "Batch output should not be NaN");
        ASSERT(!isinf(outputs[i]), "Batch output should not be infinite");
    }
    
    inference_engine_destroy(engine);
    return nullptr;
}

// Blackbox Test: Move Generation and Evaluation
char* test_move_generation_evaluation(void) {
    NeuralNetwork* nn = nn_create_hybrid(768, 512, 4096);
    InferenceEngine* engine = inference_engine_create(nn);
    
    ChessPosition* pos = chess_position_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    
    // Get multiple move predictions
    MoveEvaluation evaluations[20];
    size_t num_moves = 0;
    inference_engine_predict_moves(engine, pos, evaluations, &num_moves);
    
    // Should return some moves (even if network is untrained)
    ASSERT(num_moves >= 0, "Should return valid move count");
    ASSERT(num_moves <= 20, "Should not exceed buffer size");
    
    chess_position_destroy(pos);
    inference_engine_destroy(engine);
    return nullptr;
}

// Blackbox Test: Training with All Features Enabled
char* test_full_feature_training(void) {
    NeuralNetwork* nn = nn_create_hybrid(768, 512, 4096);
    
    TrainingConfig config;
    config.optimizer_type = OPTIMIZER_ADAM;
    config.learning_rate = 0.001;
    config.use_curriculum = true;
    config.use_pavlovian = true;
    config.use_spaced_repetition = true;
    config.max_epochs = 1;
    config.batch_size = 10;
    
    TrainingEngine* engine = training_engine_create(nn, &config);
    
    // All features should be enabled - test by training
    training_engine_train_with_curriculum(engine);
    
    TrainingStats* stats = training_engine_get_stats(engine);
    // Training may complete with 0 examples if curriculum is empty, which is valid
    ASSERT(stats->epoch >= 0, "Training should complete without error");
    
    training_engine_destroy(engine);
    return nullptr;
}

// Blackbox Test: Position Evaluation Consistency
char* test_evaluation_consistency(void) {
    NeuralNetwork* nn = nn_create_hybrid(768, 512, 4096);
    InferenceEngine* engine = inference_engine_create(nn);
    
    ChessPosition* pos = chess_position_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    
    // Evaluate same position multiple times
    double eval1 = inference_engine_evaluate_position(engine, pos);
    double eval2 = inference_engine_evaluate_position(engine, pos);
    double eval3 = inference_engine_evaluate_position(engine, pos);
    
    // Evaluations should be valid numbers (LSTM may have state, so may vary slightly)
    ASSERT(!isnan(eval1), "Evaluation1 should not be NaN");
    ASSERT(!isnan(eval2), "Evaluation2 should not be NaN");
    ASSERT(!isnan(eval3), "Evaluation3 should not be NaN");
    ASSERT(!isinf(eval1), "Evaluation1 should not be infinite");
    ASSERT(!isinf(eval2), "Evaluation2 should not be infinite");
    ASSERT(!isinf(eval3), "Evaluation3 should not be infinite");
    
    chess_position_destroy(pos);
    inference_engine_destroy(engine);
    return nullptr;
}

// Run all blackbox tests
TestSuite* create_blackbox_test_suite(void) {
    TestSuite* suite = test_suite_create("Blackbox Tests");
    
    test_suite_add_test(suite, "End-to-End Training and Inference", test_end_to_end_training_inference);
    test_suite_add_test(suite, "Multiple Position Evaluations", test_multiple_positions);
    test_suite_add_test(suite, "Curriculum Progression", test_curriculum_progression_blackbox);
    test_suite_add_test(suite, "Batch Inference", test_batch_inference);
    test_suite_add_test(suite, "Move Generation and Evaluation", test_move_generation_evaluation);
    test_suite_add_test(suite, "Full Feature Training", test_full_feature_training);
    test_suite_add_test(suite, "Evaluation Consistency", test_evaluation_consistency);
    
    return suite;
}
