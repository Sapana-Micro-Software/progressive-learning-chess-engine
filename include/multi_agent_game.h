/*
 * Copyright (C) 2025, Shyamal Suhana Chandra
 * All rights reserved.
 */
#ifndef MULTI_AGENT_GAME_H
#define MULTI_AGENT_GAME_H

#include <stddef.h>
#include <stdbool.h>
#include "chess_representation.h"  // For ChessMove

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations for structs defined in .cpp files
typedef struct MultiAgentGame MultiAgentGame;

// Game types
typedef enum {
    GAME_CHESS,
    GAME_FOOTBALL,
    GAME_BASKETBALL,
    GAME_BASEBALL,
    GAME_HOCKEY,
    GAME_SOCCER,
    GAME_TENNIS,
    GAME_GENERIC
} GameType;

// Agent types
typedef enum {
    AGENT_PLAYER,
    AGENT_TEAM,
    AGENT_COACH,
    AGENT_REFEREE
} AgentType;

// Agent structure (defined before GameState which uses it)
typedef struct {
    size_t agent_id;
    AgentType type;
    double* policy;  // Action probabilities
    double* value;   // State value estimate
    size_t policy_size;
    bool is_learning;
    double learning_rate;
} Agent;

// Action representation
typedef struct {
    size_t agent_id;
    double* action_vector;
    size_t action_size;
    double timestamp;
    double confidence;
} GameAction;

// State representation
typedef struct {
    double* state_vector;
    size_t state_size;
    GameType game_type;
    size_t num_agents;
    Agent** agents;
    double timestamp;
    bool is_terminal;
    double reward;  // For RL
} GameState;

// Multi-agent game framework
MultiAgentGame* multi_agent_game_create(GameType game_type, size_t num_agents);
void multi_agent_game_destroy(MultiAgentGame* game);
void multi_agent_game_reset(MultiAgentGame* game);
GameState* multi_agent_game_get_state(MultiAgentGame* game);
void multi_agent_game_apply_action(MultiAgentGame* game, const GameAction* action);
bool multi_agent_game_is_terminal(MultiAgentGame* game);
double multi_agent_game_get_reward(MultiAgentGame* game, size_t agent_id);

// Agent API
Agent* agent_create(size_t agent_id, AgentType type, size_t action_space_size);
void agent_destroy(Agent* agent);
void agent_update_policy(Agent* agent, const GameState* state, const GameAction* action, double reward);
GameAction* agent_select_action(Agent* agent, const GameState* state);
void agent_update_value(Agent* agent, const GameState* state, double value);

// Chess as multi-agent (white and black are separate agents)
MultiAgentGame* chess_as_multi_agent_create();
void chess_multi_agent_reset(MultiAgentGame* game);
GameAction* chess_action_from_move(const ChessMove* move);
ChessMove* chess_move_from_action(const GameAction* action);

// Sports game implementations
MultiAgentGame* football_game_create(size_t num_players_per_team);
MultiAgentGame* basketball_game_create(size_t num_players_per_team);
MultiAgentGame* baseball_game_create();
MultiAgentGame* hockey_game_create(size_t num_players_per_team);
MultiAgentGame* soccer_game_create(size_t num_players_per_team);
MultiAgentGame* tennis_game_create(bool doubles);

// State vector generation for different games
void game_state_to_vector(const GameState* state, double* vector, size_t* vector_size);
GameState* game_state_from_vector(const double* vector, size_t vector_size, GameType game_type);

#ifdef __cplusplus
}
#endif

#endif // MULTI_AGENT_GAME_H
