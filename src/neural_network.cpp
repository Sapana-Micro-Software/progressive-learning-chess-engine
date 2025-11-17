#include "../include/neural_network.h"
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <random>

// Random number generator
static std::mt19937 rng(std::random_device{}());
static std::uniform_real_distribution<double> dist(-0.1, 0.1);

// Activation functions
static double sigmoid(double x) {                                    // Compute sigmoid activation function for neural network forward pass
    return 1.0 / (1.0 + exp(-x));                                    // Return normalized value between zero and one using exponential
}

static double sigmoid_derivative(double x) {                           // Calculate derivative of sigmoid function for backpropagation gradient
    double s = sigmoid(x);                                             // Get sigmoid value to compute derivative efficiently
    return s * (1.0 - s);                                             // Return derivative using sigmoid property s'(x) = s(x)(1-s(x))
}

static double tanh_activation(double x) {                              // Compute hyperbolic tangent activation function for neural layers
    return tanh(x);                                                    // Return value between negative one and positive one
}

static double tanh_derivative(double x) {                              // Calculate derivative of tanh function for gradient computation
    double t = tanh(x);                                                // Get tanh value to compute derivative using identity
    return 1.0 - t * t;                                               // Return derivative using tanh property t'(x) = 1 - t²(x)
}

static double relu(double x) {                                        // Compute rectified linear unit activation for deep networks
    return x > 0 ? x : 0.0;                                            // Return input if positive otherwise return zero threshold
}

static double relu_derivative(double x) {                             // Calculate derivative of ReLU function for backpropagation
    return x > 0 ? 1.0 : 0.0;                                         // Return one if positive otherwise return zero for gradient
}

static void softmax(double* x, size_t n) {                           // Normalize vector to probability distribution using softmax function
    double max_val = x[0];                                            // Find maximum value to prevent numerical overflow in exponent
    for (size_t i = 1; i < n; i++) {                                  // Iterate through all elements to find global maximum value
        if (x[i] > max_val) max_val = x[i];                           // Update maximum if current element is larger than previous
    }
    
    double sum = 0.0;                                                 // Initialize sum accumulator for normalization denominator
    for (size_t i = 0; i < n; i++) {                                  // Compute exponential of each element shifted by maximum value
        x[i] = exp(x[i] - max_val);                                    // Subtract max for numerical stability then apply exponential
        sum += x[i];                                                   // Accumulate sum of exponentials for probability normalization
    }
    
    for (size_t i = 0; i < n; i++) {                                  // Normalize each element by dividing by sum of all exponentials
        x[i] /= sum;                                                   // Divide each exponential by total sum to get probabilities
    }
}

// Bayesian Layer Implementation
struct BayesianLayer {
    size_t num_nodes;
    size_t num_parents;
    double* weights;      // Conditional probability tables
    double* biases;
    double* activations;
    double* input_cache;
    ActivationType activation;
};

BayesianLayer* bayesian_layer_create(size_t num_nodes, size_t num_parents) {  // Allocate and initialize Bayesian network layer with nodes and parents
    BayesianLayer* layer = new BayesianLayer;                          // Allocate memory for new Bayesian layer structure
    layer->num_nodes = num_nodes;                                      // Set number of output nodes in this Bayesian layer
    layer->num_parents = num_parents;                                // Set number of input parent nodes for conditional probabilities
    layer->weights = new double[num_nodes * num_parents];             // Allocate weight matrix for conditional probability tables
    layer->biases = new double[num_nodes];                             // Allocate bias vector for each output node activation
    layer->activations = new double[num_nodes];                        // Allocate activation cache for backward pass computation
    layer->input_cache = new double[num_parents];                     // Allocate input cache to store values for gradient computation
    layer->activation = ACTIVATION_SIGMOID;                           // Set default activation function to sigmoid for probabilities
    
    for (size_t i = 0; i < num_nodes * num_parents; i++) {             // Initialize all weight values with small random numbers
        layer->weights[i] = dist(rng);                                 // Sample from uniform distribution for weight initialization
    }
    for (size_t i = 0; i < num_nodes; i++) {                          // Initialize all bias values with small random numbers
        layer->biases[i] = dist(rng);                                  // Sample from uniform distribution for bias initialization
    }
    
    return layer;                                                      // Return pointer to initialized Bayesian layer structure
}

void bayesian_layer_destroy(BayesianLayer* layer) {
    if (layer) {
        delete[] layer->weights;
        delete[] layer->biases;
        delete[] layer->activations;
        delete[] layer->input_cache;
        delete layer;
    }
}

void bayesian_layer_forward(BayesianLayer* layer, const double* input, double* output) {  // Forward pass through Bayesian layer computing conditional probabilities
    memcpy(layer->input_cache, input, layer->num_parents * sizeof(double));  // Cache input values for backward pass gradient computation
    
    for (size_t i = 0; i < layer->num_nodes; i++) {                    // Iterate through each output node to compute activation
        double sum = layer->biases[i];                                 // Initialize sum with bias term for this output node
        for (size_t j = 0; j < layer->num_parents; j++) {             // Sum weighted contributions from all parent input nodes
            sum += layer->weights[i * layer->num_parents + j] * input[j];  // Add product of weight and input to accumulated sum
        }
        
        switch (layer->activation) {                                   // Apply activation function based on layer configuration
            case ACTIVATION_SIGMOID:                                    // Use sigmoid for probability-like outputs between zero and one
                output[i] = sigmoid(sum);                              // Apply sigmoid function to weighted sum for normalization
                break;
            case ACTIVATION_TANH:                                       // Use tanh for outputs between negative one and positive one
                output[i] = tanh_activation(sum);                       // Apply hyperbolic tangent to weighted sum
                break;
            case ACTIVATION_RELU:                                       // Use ReLU for non-negative outputs with zero threshold
                output[i] = relu(sum);                                 // Apply rectified linear unit to weighted sum
                break;
            default:                                                    // Use linear activation if no specific function specified
                output[i] = sum;                                       // Pass through weighted sum without transformation
        }
        layer->activations[i] = output[i];                             // Cache activation value for backward pass derivative
    }
}

void bayesian_layer_backward(BayesianLayer* layer, const double* gradient, double* input_gradient) {  // Backward pass computing gradients for input layer from output gradients
    memset(input_gradient, 0, layer->num_parents * sizeof(double));  // Initialize input gradient array to zero before accumulation
    
    for (size_t i = 0; i < layer->num_nodes; i++) {                    // Iterate through each output node to backpropagate gradients
        double grad = gradient[i];                                     // Get gradient from next layer for this output node
        
        switch (layer->activation) {                                   // Multiply gradient by activation function derivative
            case ACTIVATION_SIGMOID:                                    // Apply sigmoid derivative to gradient for correct chain rule
                grad *= sigmoid_derivative(layer->activations[i]);     // Multiply by sigmoid derivative using cached activation value
                break;
            case ACTIVATION_TANH:                                       // Apply tanh derivative to gradient for chain rule computation
                grad *= tanh_derivative(layer->activations[i]);        // Multiply by tanh derivative using cached activation value
                break;
            case ACTIVATION_RELU:                                       // Apply ReLU derivative to gradient for chain rule computation
                grad *= relu_derivative(layer->activations[i]);       // Multiply by ReLU derivative using cached activation value
                break;
            default:                                                    // No derivative multiplication needed for linear activation
                break;
        }
        
        for (size_t j = 0; j < layer->num_parents; j++) {             // Propagate gradient back to each input parent node
            input_gradient[j] += layer->weights[i * layer->num_parents + j] * grad;  // Accumulate weighted gradient contribution
        }
    }
}

// LSTM Layer Implementation
struct LSTMLayer {
    size_t input_size;
    size_t hidden_size;
    
    // Weight matrices
    double* Wf;  // Forget gate
    double* Wi;  // Input gate
    double* Wo;  // Output gate
    double* Wc;  // Cell candidate
    double* Uf, *Ui, *Uo, *Uc;  // Hidden state weights
    double* bf, *bi, *bo, *bc;  // Biases
    
    // States
    double* hidden_state;
    double* cell_state;
    double* previous_hidden;
    double* previous_cell;
    
    // Caches for backward pass
    double* input_cache;
    double* forget_gate;
    double* input_gate;
    double* output_gate;
    double* cell_candidate;
    double* cell_state_cache;
};

LSTMLayer* lstm_layer_create(size_t input_size, size_t hidden_size) {  // Create LSTM layer with specified input and hidden state dimensions
    LSTMLayer* layer = new LSTMLayer;                                  // Allocate memory for new LSTM layer structure
    layer->input_size = input_size;                                    // Set input vector dimension for this LSTM layer
    layer->hidden_size = hidden_size;                                  // Set hidden state dimension for this LSTM layer
    
    size_t total_weights = hidden_size * (input_size + hidden_size);   // Calculate total number of weight parameters needed
    
    layer->Wf = new double[hidden_size * input_size];                  // Allocate weight matrix for forget gate input transformation
    layer->Wi = new double[hidden_size * input_size];                 // Allocate weight matrix for input gate input transformation
    layer->Wo = new double[hidden_size * input_size];                 // Allocate weight matrix for output gate input transformation
    layer->Wc = new double[hidden_size * input_size];                 // Allocate weight matrix for cell candidate input transformation
    layer->Uf = new double[hidden_size * hidden_size];                // Allocate weight matrix for forget gate hidden transformation
    layer->Ui = new double[hidden_size * hidden_size];                // Allocate weight matrix for input gate hidden transformation
    layer->Uo = new double[hidden_size * hidden_size];                // Allocate weight matrix for output gate hidden transformation
    layer->Uc = new double[hidden_size * hidden_size];                // Allocate weight matrix for cell candidate hidden transformation
    
    layer->bf = new double[hidden_size];                               // Allocate bias vector for forget gate computation
    layer->bi = new double[hidden_size];                               // Allocate bias vector for input gate computation
    layer->bo = new double[hidden_size];                               // Allocate bias vector for output gate computation
    layer->bc = new double[hidden_size];                               // Allocate bias vector for cell candidate computation
    
    layer->hidden_state = new double[hidden_size];                     // Allocate current hidden state vector storage
    layer->cell_state = new double[hidden_size];                       // Allocate current cell state vector storage
    layer->previous_hidden = new double[hidden_size];                  // Allocate previous hidden state for temporal memory
    layer->previous_cell = new double[hidden_size];                    // Allocate previous cell state for temporal memory
    
    layer->input_cache = new double[input_size];                       // Allocate cache for input values in backward pass
    layer->forget_gate = new double[hidden_size];                      // Allocate cache for forget gate activations
    layer->input_gate = new double[hidden_size];                       // Allocate cache for input gate activations
    layer->output_gate = new double[hidden_size];                     // Allocate cache for output gate activations
    layer->cell_candidate = new double[hidden_size];                   // Allocate cache for cell candidate values
    layer->cell_state_cache = new double[hidden_size];                // Allocate cache for cell state in backward pass
    
    double scale = sqrt(2.0 / (input_size + hidden_size));            // Calculate Xavier initialization scale factor
    std::uniform_real_distribution<double> init_dist(-scale, scale);   // Create uniform distribution for weight initialization
    
    auto init_weights = [&](double* w, size_t n) {                     // Lambda function to initialize weight array with random values
        for (size_t i = 0; i < n; i++) {                               // Iterate through each weight in array
            w[i] = init_dist(rng);                                     // Sample random value from uniform distribution
        }
    };
    
    init_weights(layer->Wf, hidden_size * input_size);                 // Initialize forget gate input weights with random values
    init_weights(layer->Wi, hidden_size * input_size);                 // Initialize input gate input weights with random values
    init_weights(layer->Wo, hidden_size * input_size);                // Initialize output gate input weights with random values
    init_weights(layer->Wc, hidden_size * input_size);                 // Initialize cell candidate input weights with random values
    init_weights(layer->Uf, hidden_size * hidden_size);                // Initialize forget gate hidden weights with random values
    init_weights(layer->Ui, hidden_size * hidden_size);                // Initialize input gate hidden weights with random values
    init_weights(layer->Uo, hidden_size * hidden_size);                // Initialize output gate hidden weights with random values
    init_weights(layer->Uc, hidden_size * hidden_size);                // Initialize cell candidate hidden weights with random values
    
    memset(layer->bf, 0, hidden_size * sizeof(double));                // Initialize forget gate biases to zero
    memset(layer->bi, 0, hidden_size * sizeof(double));                // Initialize input gate biases to zero
    memset(layer->bo, 0, hidden_size * sizeof(double));                // Initialize output gate biases to zero
    memset(layer->bc, 0, hidden_size * sizeof(double));                // Initialize cell candidate biases to zero
    
    memset(layer->hidden_state, 0, hidden_size * sizeof(double));      // Initialize hidden state vector to zero
    memset(layer->cell_state, 0, hidden_size * sizeof(double));        // Initialize cell state vector to zero
    memset(layer->previous_hidden, 0, hidden_size * sizeof(double));   // Initialize previous hidden state to zero
    memset(layer->previous_cell, 0, hidden_size * sizeof(double));     // Initialize previous cell state to zero
    
    return layer;                                                       // Return pointer to initialized LSTM layer
}

void lstm_layer_destroy(LSTMLayer* layer) {
    if (layer) {
        delete[] layer->Wf;
        delete[] layer->Wi;
        delete[] layer->Wo;
        delete[] layer->Wc;
        delete[] layer->Uf;
        delete[] layer->Ui;
        delete[] layer->Uo;
        delete[] layer->Uc;
        delete[] layer->bf;
        delete[] layer->bi;
        delete[] layer->bo;
        delete[] layer->bc;
        delete[] layer->hidden_state;
        delete[] layer->cell_state;
        delete[] layer->previous_hidden;
        delete[] layer->previous_cell;
        delete[] layer->input_cache;
        delete[] layer->forget_gate;
        delete[] layer->input_gate;
        delete[] layer->output_gate;
        delete[] layer->cell_candidate;
        delete[] layer->cell_state_cache;
        delete layer;
    }
}

void lstm_layer_forward(LSTMLayer* layer, const double* input, double* output, double* hidden_state) {  // Forward pass through LSTM layer computing gates and updating states
    memcpy(layer->input_cache, input, layer->input_size * sizeof(double));  // Cache input values for backward pass gradient computation
    
    memcpy(layer->previous_hidden, hidden_state, layer->hidden_size * sizeof(double));  // Save previous hidden state before update
    memcpy(layer->previous_cell, layer->cell_state, layer->hidden_size * sizeof(double));  // Save previous cell state before update
    
    for (size_t i = 0; i < layer->hidden_size; i++) {                  // Iterate through each hidden state dimension
        double f_sum = layer->bf[i];                                   // Initialize forget gate sum with bias term
        for (size_t j = 0; j < layer->input_size; j++) {               // Add weighted input contributions to forget gate
            f_sum += layer->Wf[i * layer->input_size + j] * input[j];  // Multiply input weight by input value and accumulate
        }
        for (size_t j = 0; j < layer->hidden_size; j++) {             // Add weighted hidden state contributions to forget gate
            f_sum += layer->Uf[i * layer->hidden_size + j] * hidden_state[j];  // Multiply hidden weight by state value and accumulate
        }
        layer->forget_gate[i] = sigmoid(f_sum);                        // Apply sigmoid to get forget gate activation between zero and one
        
        double i_sum = layer->bi[i];                                   // Initialize input gate sum with bias term
        for (size_t j = 0; j < layer->input_size; j++) {               // Add weighted input contributions to input gate
            i_sum += layer->Wi[i * layer->input_size + j] * input[j];  // Multiply input weight by input value and accumulate
        }
        for (size_t j = 0; j < layer->hidden_size; j++) {             // Add weighted hidden state contributions to input gate
            i_sum += layer->Ui[i * layer->hidden_size + j] * hidden_state[j];  // Multiply hidden weight by state value and accumulate
        }
        layer->input_gate[i] = sigmoid(i_sum);                        // Apply sigmoid to get input gate activation between zero and one
        
        double o_sum = layer->bo[i];                                   // Initialize output gate sum with bias term
        for (size_t j = 0; j < layer->input_size; j++) {               // Add weighted input contributions to output gate
            o_sum += layer->Wo[i * layer->input_size + j] * input[j];  // Multiply input weight by input value and accumulate
        }
        for (size_t j = 0; j < layer->hidden_size; j++) {            // Add weighted hidden state contributions to output gate
            o_sum += layer->Uo[i * layer->hidden_size + j] * hidden_state[j];  // Multiply hidden weight by state value and accumulate
        }
        layer->output_gate[i] = sigmoid(o_sum);                        // Apply sigmoid to get output gate activation between zero and one
        
        double c_sum = layer->bc[i];                                   // Initialize cell candidate sum with bias term
        for (size_t j = 0; j < layer->input_size; j++) {              // Add weighted input contributions to cell candidate
            c_sum += layer->Wc[i * layer->input_size + j] * input[j];  // Multiply input weight by input value and accumulate
        }
        for (size_t j = 0; j < layer->hidden_size; j++) {            // Add weighted hidden state contributions to cell candidate
            c_sum += layer->Uc[i * layer->hidden_size + j] * hidden_state[j];  // Multiply hidden weight by state value and accumulate
        }
        layer->cell_candidate[i] = tanh_activation(c_sum);            // Apply tanh to get cell candidate value between negative one and one
        
        layer->cell_state[i] = layer->forget_gate[i] * layer->previous_cell[i] +   // Update cell state using forget gate and previous cell
                              layer->input_gate[i] * layer->cell_candidate[i];      // Add input gate and cell candidate contribution
        layer->cell_state_cache[i] = layer->cell_state[i];             // Cache cell state for backward pass computation
        
        hidden_state[i] = layer->output_gate[i] * tanh_activation(layer->cell_state[i]);  // Update hidden state using output gate and cell state
        output[i] = hidden_state[i];                                   // Copy hidden state to output vector
    }
    
    memcpy(layer->hidden_state, hidden_state, layer->hidden_size * sizeof(double));  // Save final hidden state for next forward pass
}

void lstm_layer_backward(LSTMLayer* layer, const double* gradient, double* input_gradient) {
    // Simplified backward pass (full implementation would be more complex)
    // This is a placeholder - full LSTM backprop requires careful handling of gates
    memset(input_gradient, 0, layer->input_size * sizeof(double));
    
    // Basic gradient propagation (simplified)
    for (size_t i = 0; i < layer->hidden_size; i++) {
        double grad = gradient[i] * layer->output_gate[i];
        for (size_t j = 0; j < layer->input_size; j++) {
            input_gradient[j] += layer->Wc[i * layer->input_size + j] * grad;
        }
    }
}

// Neural Network (Hybrid: Bayesian + LSTM)
struct NeuralNetwork {
    size_t input_size;
    size_t hidden_size;
    size_t output_size;
    
    BayesianLayer** bayesian_layers;  // Array of pointers to Bayesian layers
    LSTMLayer** lstm_layers;         // Array of pointers to LSTM layers
    size_t num_bayesian_layers;
    size_t num_lstm_layers;
    
    double* output;
    double* hidden_buffer;
};

NeuralNetwork* nn_create_hybrid(size_t input_size, size_t hidden_size, size_t output_size) {  // Create hybrid neural network combining Bayesian and LSTM layers
    NeuralNetwork* nn = new NeuralNetwork;                            // Allocate memory for new neural network structure
    nn->input_size = input_size;                                      // Set input vector dimension for network
    nn->hidden_size = hidden_size;                                    // Set hidden layer dimension for network
    nn->output_size = output_size;                                    // Set output vector dimension for network
    
    nn->num_bayesian_layers = 1;                                      // Set number of Bayesian layers in hybrid network
    nn->num_lstm_layers = 1;                                         // Set number of LSTM layers in hybrid network
    
    nn->bayesian_layers = new BayesianLayer*[nn->num_bayesian_layers];  // Allocate array of pointers to Bayesian layers
    nn->lstm_layers = new LSTMLayer*[nn->num_lstm_layers];           // Allocate array of pointers to LSTM layers
    
    nn->bayesian_layers[0] = bayesian_layer_create(hidden_size, input_size);  // Create first Bayesian layer transforming input to hidden
    nn->lstm_layers[0] = lstm_layer_create(hidden_size, hidden_size);  // Create first LSTM layer processing hidden state sequences
    
    nn->output = new double[output_size];                             // Allocate output buffer for network predictions
    nn->hidden_buffer = new double[hidden_size];                      // Allocate hidden state buffer for layer communication
    
    return nn;                                                         // Return pointer to initialized hybrid neural network
}

void nn_destroy(NeuralNetwork* nn) {
    if (nn) {
        for (size_t i = 0; i < nn->num_bayesian_layers; i++) {
            bayesian_layer_destroy(nn->bayesian_layers[i]);
        }
        for (size_t i = 0; i < nn->num_lstm_layers; i++) {
            lstm_layer_destroy(nn->lstm_layers[i]);
        }
        delete[] nn->bayesian_layers;
        delete[] nn->lstm_layers;
        delete[] nn->output;
        delete[] nn->hidden_buffer;
        delete nn;
    }
}

void nn_forward(NeuralNetwork* nn, const double* input, double* output) {  // Forward pass through hybrid network computing output from input
    double* current = const_cast<double*>(input);                     // Get pointer to input for first layer processing
    double* temp_buffer = new double[nn->hidden_size];               // Allocate temporary buffer for intermediate layer outputs
    
    bayesian_layer_forward(nn->bayesian_layers[0], current, temp_buffer);  // Pass input through Bayesian layer to get hidden representation
    current = temp_buffer;                                            // Update current pointer to Bayesian layer output
    
    memset(nn->hidden_buffer, 0, nn->hidden_size * sizeof(double));  // Initialize hidden state buffer to zero for LSTM processing
    lstm_layer_forward(nn->lstm_layers[0], current, nn->hidden_buffer, nn->hidden_buffer);  // Pass through LSTM layer updating hidden state
    
    memcpy(output, nn->hidden_buffer, std::min(nn->hidden_size, nn->output_size) * sizeof(double));  // Copy hidden state to output buffer
    
    delete[] temp_buffer;                                             // Free temporary buffer memory
}

void nn_backward(NeuralNetwork* nn, const double* target, double* loss) {  // Backward pass computing loss and gradients for weight updates
    *loss = 0.0;                                                      // Initialize loss accumulator to zero
    for (size_t i = 0; i < nn->output_size; i++) {                   // Iterate through each output dimension
        double diff = nn->output[i] - target[i];                      // Compute difference between prediction and target
        *loss += diff * diff;                                         // Accumulate squared difference for mean squared error
    }
    *loss /= nn->output_size;                                         // Divide by output size to get mean squared error
    
    double* output_gradient = new double[nn->output_size];           // Allocate gradient buffer for output layer
    for (size_t i = 0; i < nn->output_size; i++) {                   // Compute gradient for each output dimension
        output_gradient[i] = 2.0 * (nn->output[i] - target[i]) / nn->output_size;  // MSE gradient is two times difference divided by size
    }
    
    delete[] output_gradient;                                         // Free gradient buffer memory
}

// Optimizer Implementation
struct Optimizer {
    OptimizerType type;
    double learning_rate;
    double momentum;
    double beta1, beta2;  // For Adam
    double epsilon;
    double* momentum_buffer;
    double* velocity_buffer;  // For Adam/Adagrad
    size_t buffer_size;
    size_t step;
};

Optimizer* optimizer_create(OptimizerType type, double learning_rate) {  // Create optimizer with specified type and learning rate
    Optimizer* opt = new Optimizer;                                   // Allocate memory for new optimizer structure
    opt->type = type;                                                 // Set optimizer algorithm type SGD Adam Adagrad or RMSprop
    opt->learning_rate = learning_rate;                               // Set learning rate for weight update step size
    opt->momentum = 0.9;                                             // Set momentum coefficient for SGD momentum updates
    opt->beta1 = 0.9;                                                 // Set first moment decay rate for Adam optimizer
    opt->beta2 = 0.999;                                               // Set second moment decay rate for Adam optimizer
    opt->epsilon = 1e-8;                                              // Set small epsilon value to prevent division by zero
    opt->momentum_buffer = nullptr;                                   // Initialize momentum buffer pointer to null
    opt->velocity_buffer = nullptr;                                   // Initialize velocity buffer pointer to null for Adam
    opt->buffer_size = 0;                                            // Initialize buffer size to zero not yet allocated
    opt->step = 0;                                                   // Initialize step counter to zero for learning rate decay
    return opt;                                                       // Return pointer to initialized optimizer structure
}

void optimizer_destroy(Optimizer* opt) {
    if (opt) {
        delete[] opt->momentum_buffer;
        delete[] opt->velocity_buffer;
        delete opt;
    }
}

void optimizer_update(Optimizer* opt, NeuralNetwork* nn) {
    // Optimizer update logic (simplified placeholder)
    // Full implementation would update all weights based on gradients
    opt->step++;
}

void nn_train_batch(NeuralNetwork* nn, Optimizer* opt,                  // Train neural network on batch of examples for multiple epochs
                    const double* inputs, const double* targets, 
                    size_t batch_size, size_t epochs) {
    for (size_t epoch = 0; epoch < epochs; epoch++) {                  // Iterate through specified number of training epochs
        double total_loss = 0.0;                                       // Initialize loss accumulator for epoch averaging
        for (size_t i = 0; i < batch_size; i++) {                      // Iterate through each example in batch
            const double* input = inputs + i * nn->input_size;         // Get pointer to input vector for current example
            const double* target = targets + i * nn->output_size;     // Get pointer to target vector for current example
            
            nn_forward(nn, input, nn->output);                         // Forward pass computing network output from input
            double loss;                                               // Variable to store computed loss value
            nn_backward(nn, target, &loss);                            // Backward pass computing gradients and loss
            total_loss += loss;                                        // Accumulate loss for epoch average computation
            
            optimizer_update(opt, nn);                                 // Update network weights using optimizer algorithm
        }
    }
}
