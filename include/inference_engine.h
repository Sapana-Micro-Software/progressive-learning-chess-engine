/*
 * Copyright (C) 2025, Shyamal Suhana Chandra
 * All rights reserved.
 */
#ifndef INFERENCE_ENGINE_H
#define INFERENCE_ENGINE_H

#include <stddef.h>
#include <stdbool.h>
#include "neural_network.h"
#include "chess_representation.h"
#include "multi_agent_game.h"

#ifdef __cplusplus
extern "C" {
#endif

// Move evaluation
typedef struct {
    ChessMove move;
    double score;
    double probability;
    double confidence;
    bool is_legal;
} MoveEvaluation;

// Inference Engine
typedef struct {
    NeuralNetwork* network;
    bool is_loaded;
    double temperature;  // For sampling
    size_t max_depth;    // For search
    bool use_mcts;       // Monte Carlo Tree Search
} InferenceEngine;

// Inference Engine API
InferenceEngine* inference_engine_create(NeuralNetwork* nn);
void inference_engine_destroy(InferenceEngine* engine);
void inference_engine_load_model(InferenceEngine* engine, const char* model_path);
void inference_engine_save_model(InferenceEngine* engine, const char* model_path);

// Position evaluation
double inference_engine_evaluate_position(InferenceEngine* engine, const ChessPosition* pos);
void inference_engine_evaluate_position_vector(InferenceEngine* engine, 
                                              const double* position_vector,
                                              size_t vector_size,
                                              double* output,
                                              size_t output_size);

// Move prediction
MoveEvaluation* inference_engine_predict_move(InferenceEngine* engine, const ChessPosition* pos);
void inference_engine_predict_moves(InferenceEngine* engine, 
                                   const ChessPosition* pos,
                                   MoveEvaluation* evaluations,
                                   size_t* num_moves);
ChessMove* inference_engine_select_best_move(InferenceEngine* engine, const ChessPosition* pos);

// Multi-agent inference
GameAction* inference_engine_select_action(InferenceEngine* engine, 
                                          const GameState* state,
                                          size_t agent_id);

// Search algorithms
ChessMove* inference_engine_search_move(InferenceEngine* engine, 
                                       const ChessPosition* pos,
                                       size_t depth);
ChessMove* inference_engine_mcts_search(InferenceEngine* engine,
                                       const ChessPosition* pos,
                                       size_t simulations);

// Batch inference
void inference_engine_batch_predict(InferenceEngine* engine,
                                   const double* inputs,
                                   size_t num_inputs,
                                   size_t input_size,
                                   double* outputs,
                                   size_t output_size);

// Confidence and uncertainty
double inference_engine_get_confidence(InferenceEngine* engine, 
                                      const ChessPosition* pos,
                                      const ChessMove* move);
bool inference_engine_detect_uncertainty(InferenceEngine* engine,
                                        const ChessPosition* pos,
                                        double threshold);

#ifdef __cplusplus
}
#endif

#endif // INFERENCE_ENGINE_H
