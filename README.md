# Curriculum Chess Learning System

A comprehensive C/C++/Objective-C implementation of a curriculum learning system for chess and multi-agent games, featuring:

- **Hybrid Neural Architecture**: Bayesian networks + LSTM layers
- **Curriculum Learning**: Progressive difficulty from preschool to infinite chess
- **Pavlovian Learning**: Classical conditioning and reward-based learning
- **Spaced Repetition**: Quizlet-like system with long-term memory transition
- **Multi-Agent Framework**: Extensible to chess, sports, and other games
- **Anti-Hallucination Measures**: Validation and regularization
- **Multiple Optimizers**: SGD, Adam, Adagrad, RMSprop

## Features

### Neural Network Architecture
- **Bayesian Layers**: Probabilistic reasoning with conditional probability tables
- **LSTM Layers**: Sequential pattern recognition for move sequences
- **Hybrid Design**: Combines both architectures for robust learning

### Curriculum Learning System
- **10 Difficulty Levels**: Preschool → Kindergarten → Elementary → ... → Infinite
- **Progressive Difficulty**: Automatically increases complexity
- **Mastery Threshold**: 85% accuracy required to advance
- **Puzzle Generator**: Creates level-appropriate training examples

### Pavlovian Learning
- **Classical Conditioning**: CS-US association learning (Rescorla-Wagner model)
- **Reward-Based Learning**: Positive/negative reinforcement
- **Instrumental Conditioning**: Action-reward associations
- **Extinction**: Forgetting through non-reinforcement

### Spaced Repetition
- **SM-2 Algorithm**: Exponential spacing intervals
- **Long-Term Memory**: Transitions after 5 consecutive correct answers
- **Adaptive Review**: Adjusts intervals based on performance

### Chess Representations
- **FEN Strings**: Standard chess notation
- **Board Matrices**: 8x8x12 tensor (6 piece types × 2 colors)
- **Move Sequences**: Sequential move encoding
- **Infinite Chess**: Support for variant rules

### Multi-Agent Framework
- **Chess as Multi-Agent**: White and Black as separate agents
- **Sports Support**: Football, Basketball, Baseball, Hockey, Soccer, Tennis
- **Policy Learning**: Agent-specific action policies
- **Value Functions**: State evaluation per agent

## Building

### Command-Line Interface
```bash
make cli
./curriculum_chess train --epochs 100 --lr 0.001
./curriculum_chess infer --fen "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
./curriculum_chess puzzle --level 3
./curriculum_chess interactive
```

### GUI Application (macOS)
```bash
make gui
open CurriculumChess.app
```

## Usage

### Training
```cpp
// Create neural network
NeuralNetwork* nn = nn_create_hybrid(768, 512, 4096);

// Configure training
TrainingConfig config;
config.use_curriculum = true;
config.use_pavlovian = true;
config.use_spaced_repetition = true;
config.optimizer_type = OPTIMIZER_ADAM;
config.learning_rate = 0.001;

// Train
TrainingEngine* engine = training_engine_create(nn, &config);
training_engine_train_with_curriculum(engine);
```

### Inference
```cpp
// Load model
InferenceEngine* engine = inference_engine_create(nn);
inference_engine_load_model(engine, "model.bin");

// Evaluate position
ChessPosition* pos = chess_position_from_fen("...");
double eval = inference_engine_evaluate_position(engine, pos);

// Predict move
MoveEvaluation* move = inference_engine_predict_move(engine, pos);
```

### Curriculum Learning
```cpp
// Create curriculum
Curriculum* curriculum = curriculum_create(10);

// Add examples at different levels
TrainingExample* example = create_example(...);
curriculum_add_example(curriculum, example, LEVEL_PRESCHOOL);

// Check advancement
if (curriculum_should_advance(curriculum, accuracy)) {
    curriculum_advance_level(curriculum);
}
```

### Pavlovian Learning
```cpp
// Create learner
PavlovianLearner* learner = pavlovian_learner_create(PAVLOVIAN_HYBRID, 0.1);

// Pair position (CS) with outcome (US)
ConditionedStimulus* cs = chess_position_to_cs(position);
UnconditionedStimulus* us = chess_outcome_to_us(1.0);  // Win
pavlovian_pair_stimuli(learner, cs, us);

// Get expected reward
double expected = pavlovian_get_expected_reward(learner, cs);
```

## Architecture

```
include/
├── neural_network.h          # Hybrid Bayesian+LSTM network
├── curriculum_learning.h      # Progressive difficulty system
├── chess_representation.h      # FEN, matrices, move sequences
├── multi_agent_game.h         # Multi-agent framework
├── pavlovian_learning.h       # Classical conditioning
├── training_engine.h          # Training orchestration
└── inference_engine.h         # Model inference

src/
├── neural_network.cpp
├── curriculum_learning.cpp
├── chess_representation.cpp
├── multi_agent_game.cpp
├── pavlovian_learning.cpp
├── training_engine.cpp
├── inference_engine.cpp
└── main.cpp                  # CLI entry point

objc/
├── CurriculumChessApp.h/m     # macOS GUI application
├── TrainingViewController.h/m # Training interface
├── ChessBoardViewController.h/m # Chess board UI
└── main.m                     # GUI entry point
```

## Extending to Other Sports

The framework is designed to be extensible:

```cpp
// Create game
MultiAgentGame* game = basketball_game_create(5);  // 5 players per team

// Get state
GameState* state = multi_agent_game_get_state(game);

// Select action
GameAction* action = agent_select_action(agent, state);

// Apply action
multi_agent_game_apply_action(game, action);
```

## Research Background

- **Curriculum Learning**: Bengio et al. (2009) - "Curriculum Learning"
- **Spaced Repetition**: SM-2 algorithm (SuperMemo)
- **Pavlovian Learning**: Rescorla-Wagner model (1972)
- **Stacked RBMs**: Hinton et al. (2006) - "A Fast Learning Algorithm"
- **LSTM**: Hochreiter & Schmidhuber (1997)

## License

This implementation is provided for educational and research purposes.

## Notes

- The implementation includes placeholders for complex algorithms (full MCTS, complete move generation, etc.)
- Some functions are simplified for clarity and can be extended
- The GUI requires macOS with Cocoa framework
- Model serialization is simplified and should be extended for production use
