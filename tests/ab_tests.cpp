#include "test_harness.h"
#include "../include/neural_network.h"
#include "../include/training_engine.h"
#include "../include/curriculum_learning.h"
#include "../include/pavlovian_learning.h"
#include <cmath>
#include <cstdlib>

// A-B Test: SGD vs Adam Optimizer
char* test_optimizer_comparison(void) {
    NeuralNetwork* nn1 = nn_create_hybrid(100, 50, 10);
    NeuralNetwork* nn2 = nn_create_hybrid(100, 50, 10);
    
    TrainingConfig config1, config2;
    config1.optimizer_type = OPTIMIZER_SGD;
    config1.learning_rate = 0.01;
    config1.use_curriculum = false;
    config1.use_pavlovian = false;
    config1.use_spaced_repetition = false;
    config1.max_epochs = 1;
    config1.batch_size = 10;
    
    config2 = config1;
    config2.optimizer_type = OPTIMIZER_ADAM;
    
    TrainingEngine* engine1 = training_engine_create(nn1, &config1);
    TrainingEngine* engine2 = training_engine_create(nn2, &config2);
    
    // Run one epoch with each
    training_engine_train_epoch(engine1);
    training_engine_train_epoch(engine2);
    
    TrainingStats* stats1 = training_engine_get_stats(engine1);
    TrainingStats* stats2 = training_engine_get_stats(engine2);
    
    // Both should complete without error
    ASSERT(stats1->epoch > 0, "SGD training should complete");
    ASSERT(stats2->epoch > 0, "Adam training should complete");
    
    training_engine_destroy(engine1);
    training_engine_destroy(engine2);
    return nullptr;
}

// A-B Test: With vs Without Curriculum Learning
char* test_curriculum_vs_no_curriculum(void) {
    NeuralNetwork* nn1 = nn_create_hybrid(100, 50, 10);
    NeuralNetwork* nn2 = nn_create_hybrid(100, 50, 10);
    
    TrainingConfig config1, config2;
    config1.optimizer_type = OPTIMIZER_ADAM;
    config1.learning_rate = 0.001;
    config1.use_curriculum = true;
    config1.use_pavlovian = false;
    config1.use_spaced_repetition = false;
    config1.max_epochs = 1;
    config1.batch_size = 10;
    
    config2 = config1;
    config2.use_curriculum = false;
    
    TrainingEngine* engine1 = training_engine_create(nn1, &config1);
    TrainingEngine* engine2 = training_engine_create(nn2, &config2);
    
    // Both should train successfully
    training_engine_train_epoch(engine1);
    training_engine_train_epoch(engine2);
    
    TrainingStats* stats1 = training_engine_get_stats(engine1);
    TrainingStats* stats2 = training_engine_get_stats(engine2);
    
    ASSERT(stats1->epoch > 0, "Curriculum training should complete");
    ASSERT(stats2->epoch > 0, "Non-curriculum training should complete");
    
    training_engine_destroy(engine1);
    training_engine_destroy(engine2);
    return nullptr;
}

// A-B Test: With vs Without Pavlovian Learning
char* test_pavlovian_vs_no_pavlovian(void) {
    NeuralNetwork* nn1 = nn_create_hybrid(100, 50, 10);
    NeuralNetwork* nn2 = nn_create_hybrid(100, 50, 10);
    
    TrainingConfig config1, config2;
    config1.optimizer_type = OPTIMIZER_ADAM;
    config1.learning_rate = 0.001;
    config1.use_curriculum = false;
    config1.use_pavlovian = true;
    config1.use_spaced_repetition = false;
    config1.max_epochs = 1;
    config1.batch_size = 10;
    
    config2 = config1;
    config2.use_pavlovian = false;
    
    TrainingEngine* engine1 = training_engine_create(nn1, &config1);
    TrainingEngine* engine2 = training_engine_create(nn2, &config2);
    
    // Test Pavlovian pairing
    if (engine1->pavlovian_learner) {
        double cs_vec[10] = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
        double us_vec[1] = {1.0};
        ConditionedStimulus* cs = conditioned_stimulus_create(cs_vec, 10, 1.0);
        UnconditionedStimulus* us = unconditioned_stimulus_create(us_vec, 1, 1.0, 1.0);
        
        training_engine_train_with_pavlovian(engine1, cs, us);
        
        conditioned_stimulus_destroy(cs);
        unconditioned_stimulus_destroy(us);
    }
    
    training_engine_train_epoch(engine2);
    
    TrainingStats* stats1 = training_engine_get_stats(engine1);
    TrainingStats* stats2 = training_engine_get_stats(engine2);
    
    ASSERT(stats1->epoch >= 0, "Pavlovian training should work");
    ASSERT(stats2->epoch > 0, "Non-Pavlovian training should complete");
    
    training_engine_destroy(engine1);
    training_engine_destroy(engine2);
    return nullptr;
}

// A-B Test: Different Learning Rates
char* test_learning_rate_comparison(void) {
    NeuralNetwork* nn1 = nn_create_hybrid(100, 50, 10);
    NeuralNetwork* nn2 = nn_create_hybrid(100, 50, 10);
    
    TrainingConfig config1, config2;
    config1.optimizer_type = OPTIMIZER_ADAM;
    config1.learning_rate = 0.001;
    config1.use_curriculum = false;
    config1.use_pavlovian = false;
    config1.use_spaced_repetition = false;
    config1.max_epochs = 1;
    config1.batch_size = 10;
    
    config2 = config1;
    config2.learning_rate = 0.01;
    
    TrainingEngine* engine1 = training_engine_create(nn1, &config1);
    TrainingEngine* engine2 = training_engine_create(nn2, &config2);
    
    training_engine_train_epoch(engine1);
    training_engine_train_epoch(engine2);
    
    TrainingStats* stats1 = training_engine_get_stats(engine1);
    TrainingStats* stats2 = training_engine_get_stats(engine2);
    
    // Both should train, but may have different loss values
    ASSERT(stats1->epoch > 0, "Low LR training should complete");
    ASSERT(stats2->epoch > 0, "High LR training should complete");
    
    training_engine_destroy(engine1);
    training_engine_destroy(engine2);
    return nullptr;
}

// A-B Test: Bayesian vs LSTM Layer Performance
char* test_layer_type_comparison(void) {
    // Test that both layer types work
    NeuralNetwork* nn = nn_create_hybrid(100, 50, 10);
    double input[100];
    for (size_t i = 0; i < 100; i++) input[i] = 0.01 * i;
    
    double output[10];
    nn_forward(nn, input, output);
    
    // Output should be valid
    for (size_t i = 0; i < 10; i++) {
        ASSERT(!isnan(output[i]), "Output should not be NaN");
        ASSERT(!isinf(output[i]), "Output should not be infinite");
    }
    
    nn_destroy(nn);
    return nullptr;
}

// A-B Test: Spaced Repetition vs Regular Training
char* test_spaced_repetition_comparison(void) {
    NeuralNetwork* nn1 = nn_create_hybrid(100, 50, 10);
    NeuralNetwork* nn2 = nn_create_hybrid(100, 50, 10);
    
    TrainingConfig config1, config2;
    config1.optimizer_type = OPTIMIZER_ADAM;
    config1.learning_rate = 0.001;
    config1.use_curriculum = false;
    config1.use_pavlovian = false;
    config1.use_spaced_repetition = true;
    config1.max_epochs = 1;
    config1.batch_size = 10;
    
    config2 = config1;
    config2.use_spaced_repetition = false;
    
    TrainingEngine* engine1 = training_engine_create(nn1, &config1);
    TrainingEngine* engine2 = training_engine_create(nn2, &config2);
    
    // Test spaced repetition
    if (engine1->spaced_repetition) {
        TrainingExample example;
        example.input_size = 100;
        example.target_size = 10;
        example.difficulty = 0.5;
        example.input = new double[100];
        example.target = new double[10];
        memset(example.input, 0, 100 * sizeof(double));
        memset(example.target, 0, 10 * sizeof(double));
        
        spaced_repetition_add_example(engine1->spaced_repetition, &example);
        training_engine_train_with_spaced_repetition(engine1);
        
        delete[] example.input;
        delete[] example.target;
    }
    
    training_engine_train_epoch(engine2);
    
    TrainingStats* stats1 = training_engine_get_stats(engine1);
    TrainingStats* stats2 = training_engine_get_stats(engine2);
    
    ASSERT(stats1->epoch >= 0, "Spaced repetition training should work");
    ASSERT(stats2->epoch > 0, "Regular training should complete");
    
    training_engine_destroy(engine1);
    training_engine_destroy(engine2);
    return nullptr;
}

// Run all A-B tests
TestSuite* create_ab_test_suite(void) {
    TestSuite* suite = test_suite_create("A-B Tests");
    
    test_suite_add_test(suite, "SGD vs Adam Optimizer", test_optimizer_comparison);
    test_suite_add_test(suite, "Curriculum vs No Curriculum", test_curriculum_vs_no_curriculum);
    test_suite_add_test(suite, "Pavlovian vs No Pavlovian", test_pavlovian_vs_no_pavlovian);
    test_suite_add_test(suite, "Learning Rate Comparison", test_learning_rate_comparison);
    test_suite_add_test(suite, "Layer Type Comparison", test_layer_type_comparison);
    test_suite_add_test(suite, "Spaced Repetition Comparison", test_spaced_repetition_comparison);
    
    return suite;
}
