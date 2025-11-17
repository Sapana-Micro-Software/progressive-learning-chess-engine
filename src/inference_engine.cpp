#include "../include/inference_engine.h"
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <algorithm>

InferenceEngine* inference_engine_create(NeuralNetwork* nn) {           // Create inference engine with neural network for chess evaluation
    InferenceEngine* engine = new InferenceEngine;                     // Allocate memory for new inference engine structure
    engine->network = nn;                                             // Store pointer to neural network for position evaluation
    engine->is_loaded = (nn != nullptr);                               // Set loaded flag based on whether network is provided
    engine->temperature = 1.0;                                        // Set temperature to one for deterministic move selection
    engine->max_depth = 3;                                            // Set maximum search depth to three for minimax algorithm
    engine->use_mcts = false;                                         // Disable Monte Carlo tree search by default
    return engine;                                                     // Return pointer to initialized inference engine
}

void inference_engine_destroy(InferenceEngine* engine) {
    if (engine) {
        delete engine;
    }
}

void inference_engine_load_model(InferenceEngine* engine, const char* model_path) {
    // Model loading (simplified - would deserialize network weights)
    engine->is_loaded = true;
}

void inference_engine_save_model(InferenceEngine* engine, const char* model_path) {
    // Model saving (simplified - would serialize network weights)
}

double inference_engine_evaluate_position(InferenceEngine* engine, const ChessPosition* pos) {  // Evaluate chess position using neural network
    if (!engine->is_loaded) return 0.0;                              // Return zero if network is not loaded or available
    
    double input[64 * 12];                                            // Allocate input buffer for eight by eight by twelve tensor
    chess_position_to_matrix((ChessPosition*)pos, input);             // Convert chess position to matrix representation for network
    
    double output[1];                                                // Allocate output buffer for single evaluation score
    nn_forward(engine->network, input, output);                       // Forward pass through network to compute position evaluation
    
    return output[0];                                                 // Return position evaluation score from network output
}

void inference_engine_evaluate_position_vector(InferenceEngine* engine, 
                                              const double* position_vector,
                                              size_t vector_size,
                                              double* output,
                                              size_t output_size) {
    if (!engine->is_loaded) return;
    
    nn_forward(engine->network, position_vector, output);
}

MoveEvaluation* inference_engine_predict_move(InferenceEngine* engine, const ChessPosition* pos) {  // Predict best move from position using neural network
    if (!engine->is_loaded) return nullptr;                           // Return null if network is not loaded or available
    
    double input[64 * 12];                                            // Allocate input buffer for position matrix representation
    chess_position_to_matrix((ChessPosition*)pos, input);              // Convert chess position to matrix for network input
    
    double output[64 * 64];                                           // Allocate output buffer for move probability distribution
    nn_forward(engine->network, input, output);                        // Forward pass computing move probabilities for all moves
    
    double max_prob = 0.0;                                            // Initialize maximum probability to zero for search
    size_t best_from = 0, best_to = 0;                                // Initialize best move squares to zero
    
    for (size_t from = 0; from < 64; from++) {                        // Iterate through all possible from squares
        for (size_t to = 0; to < 64; to++) {                          // Iterate through all possible to squares
            size_t idx = from * 64 + to;                              // Calculate index in output array for this move
            if (output[idx] > max_prob) {                             // Check if this move has higher probability than current best
                max_prob = output[idx];                                // Update maximum probability to this move value
                best_from = from;                                     // Update best from square to current from square
                best_to = to;                                         // Update best to square to current to square
            }
        }
    }
    
    MoveEvaluation* eval = new MoveEvaluation;                        // Allocate memory for move evaluation structure
    eval->move.from = (Square)best_from;                              // Set move from square to best from square found
    eval->move.to = (Square)best_to;                                 // Set move to square to best to square found
    eval->score = max_prob;                                           // Set evaluation score to maximum probability value
    eval->probability = max_prob;                                      // Set move probability to maximum probability value
    eval->confidence = max_prob;                                      // Set confidence to maximum probability value
    eval->is_legal = chess_position_is_legal_move((ChessPosition*)pos, &eval->move);  // Check if predicted move is legal in position
    
    return eval;                                                      // Return pointer to move evaluation structure
}

void inference_engine_predict_moves(InferenceEngine* engine, 
                                   const ChessPosition* pos,
                                   MoveEvaluation* evaluations,
                                   size_t* num_moves) {
    if (!engine->is_loaded) {
        *num_moves = 0;
        return;
    }
    
    double input[64 * 12];
    chess_position_to_matrix((ChessPosition*)pos, input);
    
    double output[64 * 64];
    nn_forward(engine->network, input, output);
    
    // Get top moves
    *num_moves = 0;
    const size_t max_moves = 20;
    
    for (size_t from = 0; from < 64 && *num_moves < max_moves; from++) {
        for (size_t to = 0; to < 64 && *num_moves < max_moves; to++) {
            size_t idx = from * 64 + to;
            if (output[idx] > 0.01) {  // Threshold
                MoveEvaluation* eval = &evaluations[*num_moves];
                eval->move.from = (Square)from;
                eval->move.to = (Square)to;
                eval->score = output[idx];
                eval->probability = output[idx];
                eval->confidence = output[idx];
                eval->is_legal = chess_position_is_legal_move((ChessPosition*)pos, &eval->move);
                (*num_moves)++;
            }
        }
    }
}

ChessMove* inference_engine_select_best_move(InferenceEngine* engine, const ChessPosition* pos) {
    MoveEvaluation* eval = inference_engine_predict_move(engine, pos);
    if (!eval) return nullptr;
    
    ChessMove* move = new ChessMove;
    *move = eval->move;
    delete eval;
    
    return move;
}

GameAction* inference_engine_select_action(InferenceEngine* engine, 
                                          const GameState* state,
                                          size_t agent_id) {
    if (!engine->is_loaded) return nullptr;
    
    double output[1000];
    nn_forward(engine->network, state->state_vector, output);
    
    GameAction* action = new GameAction;
    action->agent_id = agent_id;
    action->action_size = 100;  // Default
    action->action_vector = new double[action->action_size];
    memcpy(action->action_vector, output, 
           std::min(action->action_size, (size_t)1000) * sizeof(double));
    action->timestamp = state->timestamp;
    action->confidence = 1.0;
    
    return action;
}

ChessMove* inference_engine_search_move(InferenceEngine* engine, 
                                       const ChessPosition* pos,
                                       size_t depth) {
    // Minimax search (simplified)
    if (depth == 0) {
        return inference_engine_select_best_move(engine, pos);
    }
    
    // Generate moves and search (use white as default since we can't access white_to_move)
    ChessMove moves[256];
    size_t num_moves = 0;
    chess_position_generate_moves((ChessPosition*)pos, COLOR_WHITE, moves, &num_moves);
    
    double best_score = -1e10;
    ChessMove* best_move = nullptr;
    
    for (size_t i = 0; i < num_moves; i++) {
        chess_position_make_move((ChessPosition*)pos, &moves[i]);
        double score = inference_engine_evaluate_position(engine, pos);
        chess_position_unmake_move((ChessPosition*)pos);
        
        if (score > best_score) {
            best_score = score;
            best_move = &moves[i];
        }
    }
    
    if (best_move) {
        ChessMove* result = new ChessMove;
        *result = *best_move;
        return result;
    }
    
    return inference_engine_select_best_move(engine, pos);
}

ChessMove* inference_engine_mcts_search(InferenceEngine* engine,
                                       const ChessPosition* pos,
                                       size_t simulations) {
    // Monte Carlo Tree Search (simplified placeholder)
    // Full MCTS implementation would be more complex
    return inference_engine_select_best_move(engine, pos);
}

void inference_engine_batch_predict(InferenceEngine* engine,
                                   const double* inputs,
                                   size_t num_inputs,
                                   size_t input_size,
                                   double* outputs,
                                   size_t output_size) {
    if (!engine->is_loaded) return;
    
    for (size_t i = 0; i < num_inputs; i++) {
        const double* input = inputs + i * input_size;
        double* output = outputs + i * output_size;
        nn_forward(engine->network, input, output);
    }
}

double inference_engine_get_confidence(InferenceEngine* engine, 
                                      const ChessPosition* pos,
                                      const ChessMove* move) {
    if (!engine->is_loaded) return 0.0;
    
    double input[64 * 12];
    chess_position_to_matrix((ChessPosition*)pos, input);
    
    double output[64 * 64];
    nn_forward(engine->network, input, output);
    
    size_t idx = move->from * 64 + move->to;
    return output[idx];
}

bool inference_engine_detect_uncertainty(InferenceEngine* engine,
                                        const ChessPosition* pos,
                                        double threshold) {
    if (!engine->is_loaded) return true;
    
    double input[64 * 12];
    chess_position_to_matrix((ChessPosition*)pos, input);
    
    double output[64 * 64];
    nn_forward(engine->network, input, output);
    
    // Check variance/entropy of output distribution
    double sum = 0.0, sum_sq = 0.0;
    for (size_t i = 0; i < 64 * 64; i++) {
        sum += output[i];
        sum_sq += output[i] * output[i];
    }
    
    double mean = sum / (64 * 64);
    double variance = (sum_sq / (64 * 64)) - (mean * mean);
    
    return variance < threshold;  // Low variance = high uncertainty
}
