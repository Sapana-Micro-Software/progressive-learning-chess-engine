/*
 * Copyright (C) 2025, Shyamal Suhana Chandra
 * All rights reserved.
 */

#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations
typedef struct NeuralNetwork NeuralNetwork;
typedef struct BayesianLayer BayesianLayer;
typedef struct LSTMLayer LSTMLayer;
typedef struct Optimizer Optimizer;

// Activation functions
typedef enum {
    ACTIVATION_SIGMOID,
    ACTIVATION_TANH,
    ACTIVATION_RELU,
    ACTIVATION_SOFTMAX,
    ACTIVATION_LINEAR
} ActivationType;

// Optimizer types
typedef enum {
    OPTIMIZER_SGD,
    OPTIMIZER_ADAM,
    OPTIMIZER_ADAGRAD,
    OPTIMIZER_RMSPROP
} OptimizerType;

// Neural Network API
NeuralNetwork* nn_create_hybrid(size_t input_size, size_t hidden_size, size_t output_size);
void nn_destroy(NeuralNetwork* nn);

// Bayesian Network Layer
BayesianLayer* bayesian_layer_create(size_t num_nodes, size_t num_parents);
void bayesian_layer_destroy(BayesianLayer* layer);
void bayesian_layer_forward(BayesianLayer* layer, const double* input, double* output);
void bayesian_layer_backward(BayesianLayer* layer, const double* gradient, double* input_gradient);

// LSTM Layer
LSTMLayer* lstm_layer_create(size_t input_size, size_t hidden_size);
void lstm_layer_destroy(LSTMLayer* layer);
void lstm_layer_forward(LSTMLayer* layer, const double* input, double* output, double* hidden_state);
void lstm_layer_backward(LSTMLayer* layer, const double* gradient, double* input_gradient);

// Forward/Backward pass
void nn_forward(NeuralNetwork* nn, const double* input, double* output);
void nn_backward(NeuralNetwork* nn, const double* target, double* loss);

// Optimizer
Optimizer* optimizer_create(OptimizerType type, double learning_rate);
void optimizer_destroy(Optimizer* opt);
void optimizer_update(Optimizer* opt, NeuralNetwork* nn);

// Training
void nn_train_batch(NeuralNetwork* nn, Optimizer* opt, 
                    const double* inputs, const double* targets, 
                    size_t batch_size, size_t epochs);

#ifdef __cplusplus
}
#endif

#endif // NEURAL_NETWORK_H
