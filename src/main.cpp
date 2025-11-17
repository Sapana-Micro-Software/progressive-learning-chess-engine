#include "../include/neural_network.h"
#include "../include/curriculum_learning.h"
#include "../include/chess_representation.h"
#include "../include/training_engine.h"
#include "../include/inference_engine.h"
#include "../include/pavlovian_learning.h"
#include "../include/multi_agent_game.h"
#include <iostream>
#include <cstdio>

void print_usage(const char* program_name) {
    printf("Usage: %s [command] [options]\n", program_name);
    printf("\nCommands:\n");
    printf("  train          - Train the chess engine\n");
    printf("  infer          - Run inference on a position\n");
    printf("  puzzle         - Generate and solve puzzles\n");
    printf("  interactive    - Interactive chess game\n");
    printf("  test           - Run tests\n");
    printf("\nOptions:\n");
    printf("  --model <path>     - Model file path\n");
    printf("  --fen <fen_string> - FEN position string\n");
    printf("  --level <n>        - Difficulty level (0-9)\n");
    printf("  --epochs <n>       - Number of training epochs\n");
    printf("  --lr <rate>        - Learning rate\n");
    printf("  --optimizer <type> - Optimizer (sgd, adam, adagrad, rmsprop)\n");
}

int cmd_train(int argc, char* argv[]) {
    printf("Starting training...\n");
    
    // Create neural network
    NeuralNetwork* nn = nn_create_hybrid(768, 512, 4096);  // 8x8x12 input, 64x64 output
    
    // Training configuration
    TrainingConfig config;
    config.optimizer_type = OPTIMIZER_ADAM;
    config.learning_rate = 0.001;
    config.momentum = 0.9;
    config.weight_decay = 0.0001;
    config.batch_size = 32;
    config.max_epochs = 100;
    config.early_stopping_threshold = 0.001;
    config.use_curriculum = true;
    config.use_pavlovian = true;
    config.use_spaced_repetition = true;
    config.mastery_threshold = 0.85;
    config.patience = 10;
    
    // Create training engine
    TrainingEngine* engine = training_engine_create(nn, &config);
    
    printf("Training with curriculum learning...\n");
    printf("Level progression: Preschool -> Kindergarten -> ... -> Infinite\n");
    
    // Training loop
    for (size_t epoch = 0; epoch < config.max_epochs; epoch++) {
        training_engine_train_with_curriculum(engine);
        
        TrainingStats* stats = training_engine_get_stats(engine);
        
        if (epoch % 10 == 0) {
            printf("Epoch %zu: Loss=%.6f, Accuracy=%.2f%%, Level=%zu\n",
                   stats->epoch, stats->current_loss, 
                   stats->accuracy * 100.0, stats->current_level);
        }
        
        // Check for early stopping
        if (stats->current_loss < config.early_stopping_threshold) {
            printf("Early stopping at epoch %zu\n", epoch);
            break;
        }
    }
    
    // Save model
    training_engine_save_checkpoint(engine, "checkpoint.bin");
    printf("Model saved to checkpoint.bin\n");
    
    training_engine_destroy(engine);
    return 0;
}

int cmd_infer(int argc, char* argv[]) {
    const char* fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    const char* model_path = "checkpoint.bin";
    
    // Parse arguments
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "--fen") == 0 && i + 1 < argc) {
            fen = argv[++i];
        } else if (strcmp(argv[i], "--model") == 0 && i + 1 < argc) {
            model_path = argv[++i];
        }
    }
    
    printf("Loading model from %s...\n", model_path);
    
    // Create network and load model
    NeuralNetwork* nn = nn_create_hybrid(768, 512, 4096);
    InferenceEngine* engine = inference_engine_create(nn);
    inference_engine_load_model(engine, model_path);
    
    printf("Loading position from FEN: %s\n", fen);
    ChessPosition* pos = chess_position_from_fen(fen);
    
    // Evaluate position
    double eval = inference_engine_evaluate_position(engine, pos);
    printf("Position evaluation: %.4f\n", eval);
    
    // Predict move
    MoveEvaluation* move_eval = inference_engine_predict_move(engine, pos);
    if (move_eval) {
        printf("Predicted move: %d->%d (confidence: %.2f)\n",
               move_eval->move.from, move_eval->move.to, move_eval->confidence);
        delete move_eval;
    }
    
    chess_position_destroy(pos);
    inference_engine_destroy(engine);
    return 0;
}

int cmd_puzzle(int argc, char* argv[]) {
    printf("Puzzle generator mode\n");
    
    // Create curriculum
    Curriculum* curriculum = curriculum_create(10);
    PuzzleGenerator* generator = puzzle_generator_create(curriculum);
    
    DifficultyLevelEnum level = LEVEL_PRESCHOOL;
    
    // Parse arguments
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "--level") == 0 && i + 1 < argc) {
            level = (DifficultyLevelEnum)atoi(argv[++i]);
            if (level > LEVEL_INFINITE) level = LEVEL_INFINITE;
        }
    }
    
    printf("Generating puzzle at level %d...\n", level);
    
    TrainingExample* puzzle = puzzle_generator_create_puzzle(generator, level);
    if (puzzle) {
        printf("Puzzle generated: difficulty=%.2f, input_size=%zu, target_size=%zu\n",
               puzzle->difficulty, puzzle->input_size, puzzle->target_size);
        
        // Clean up
        delete[] puzzle->input;
        delete[] puzzle->target;
        delete puzzle;
    }
    
    puzzle_generator_destroy(generator);
    curriculum_destroy(curriculum);
    return 0;
}

int cmd_interactive(int argc, char* argv[]) {
    printf("Interactive chess mode\n");
    printf("Commands: move <from> <to>, eval, quit\n");
    
    ChessPosition* pos = chess_position_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    
    // Load model if available
    NeuralNetwork* nn = nn_create_hybrid(768, 512, 4096);
    InferenceEngine* engine = inference_engine_create(nn);
    
    const char* model_path = "checkpoint.bin";
    FILE* f = fopen(model_path, "r");
    if (f) {
        fclose(f);
        inference_engine_load_model(engine, model_path);
        printf("Model loaded from %s\n", model_path);
    } else {
        printf("No model found, using untrained network\n");
    }
    
    char line[256];
    while (true) {
        printf("chess> ");
        if (!fgets(line, sizeof(line), stdin)) break;
        
        if (strncmp(line, "quit", 4) == 0) break;
        if (strncmp(line, "eval", 4) == 0) {
            double eval = inference_engine_evaluate_position(engine, pos);
            printf("Position evaluation: %.4f\n", eval);
        } else if (strncmp(line, "move", 4) == 0) {
            // Parse move (simplified)
            printf("Move command (not fully implemented)\n");
        } else {
            printf("Unknown command\n");
        }
    }
    
    chess_position_destroy(pos);
    inference_engine_destroy(engine);
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }
    
    const char* command = argv[1];
    
    if (strcmp(command, "train") == 0) {
        return cmd_train(argc, argv);
    } else if (strcmp(command, "infer") == 0) {
        return cmd_infer(argc, argv);
    } else if (strcmp(command, "puzzle") == 0) {
        return cmd_puzzle(argc, argv);
    } else if (strcmp(command, "interactive") == 0) {
        return cmd_interactive(argc, argv);
    } else if (strcmp(command, "test") == 0) {
        printf("Running tests...\n");
        // Test code would go here
        return 0;
    } else {
        printf("Unknown command: %s\n", command);
        print_usage(argv[0]);
        return 1;
    }
}
