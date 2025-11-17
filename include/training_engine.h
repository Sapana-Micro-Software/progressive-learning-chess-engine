#ifndef TRAINING_ENGINE_H
#define TRAINING_ENGINE_H

#include <stddef.h>
#include <stdbool.h>
#include "neural_network.h"
#include "curriculum_learning.h"
#include "pavlovian_learning.h"

#ifdef __cplusplus
extern "C" {
#endif

// Training configuration
typedef struct {
    OptimizerType optimizer_type;
    double learning_rate;
    double momentum;
    double weight_decay;
    size_t batch_size;
    size_t max_epochs;
    double early_stopping_threshold;
    bool use_curriculum;
    bool use_pavlovian;
    bool use_spaced_repetition;
    double mastery_threshold;
    size_t patience;  // Early stopping patience
} TrainingConfig;

// Training statistics
typedef struct {
    double current_loss;
    double average_loss;
    double accuracy;
    size_t epoch;
    size_t examples_seen;
    DifficultyLevelEnum current_level;
    double training_time;
    double validation_accuracy;
} TrainingStats;

// Training Engine
typedef struct {
    NeuralNetwork* network;
    Curriculum* curriculum;
    PavlovianLearner* pavlovian_learner;
    SpacedRepetition* spaced_repetition;
    Optimizer* optimizer;
    TrainingConfig config;
    TrainingStats stats;
    bool is_training;
} TrainingEngine;

// Training Engine API
TrainingEngine* training_engine_create(NeuralNetwork* nn, TrainingConfig* config);
void training_engine_destroy(TrainingEngine* engine);

// Training methods
void training_engine_train_epoch(TrainingEngine* engine);
void training_engine_train_full(TrainingEngine* engine);
void training_engine_train_with_curriculum(TrainingEngine* engine);
void training_engine_train_with_pavlovian(TrainingEngine* engine, 
                                          const ConditionedStimulus* cs,
                                          const UnconditionedStimulus* us);
void training_engine_train_with_spaced_repetition(TrainingEngine* engine);

// Evaluation
double training_engine_evaluate(TrainingEngine* engine, 
                               const double* inputs, 
                               const double* targets, 
                               size_t num_examples);
TrainingStats* training_engine_get_stats(TrainingEngine* engine);

// Checkpointing
void training_engine_save_checkpoint(TrainingEngine* engine, const char* filepath);
TrainingEngine* training_engine_load_checkpoint(const char* filepath);

// Progressive difficulty training
void training_engine_train_progressive(TrainingEngine* engine, 
                                      double start_difficulty,
                                      double end_difficulty,
                                      size_t steps);

// Anti-hallucination measures
void training_engine_validate_predictions(TrainingEngine* engine,
                                         const double* inputs,
                                         const double* targets,
                                         size_t num_examples,
                                         bool* is_hallucination);
void training_engine_apply_regularization(TrainingEngine* engine, double lambda);

#ifdef __cplusplus
}
#endif

#endif // TRAINING_ENGINE_H
