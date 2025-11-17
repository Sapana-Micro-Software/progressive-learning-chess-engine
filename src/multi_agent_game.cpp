#include "../include/multi_agent_game.h"
#include "../include/chess_representation.h"
#include <cstring>
#include <cstdlib>

// Game State and Agent are defined in header, no need to redefine here

// Multi-Agent Game Implementation
struct MultiAgentGame {
    GameType game_type;
    size_t num_agents;
    Agent** agents;
    GameState* current_state;
    size_t current_agent_turn;
    bool is_terminal;
};

Agent* agent_create(size_t agent_id, AgentType type, size_t action_space_size) {
    Agent* agent = new Agent;
    agent->agent_id = agent_id;
    agent->type = type;
    agent->policy_size = action_space_size;
    agent->policy = new double[action_space_size];
    agent->value = new double[1];
    agent->is_learning = true;
    agent->learning_rate = 0.01;
    
    // Initialize policy uniformly
    double uniform_prob = 1.0 / action_space_size;
    for (size_t i = 0; i < action_space_size; i++) {
        agent->policy[i] = uniform_prob;
    }
    agent->value[0] = 0.0;
    
    return agent;
}

void agent_destroy(Agent* agent) {
    if (agent) {
        delete[] agent->policy;
        delete[] agent->value;
        delete agent;
    }
}

void agent_update_policy(Agent* agent, const GameState* state, const GameAction* action, double reward) {
    // Policy gradient update (simplified)
    if (agent->is_learning) {
        // Update policy based on reward
        // This is a placeholder - full implementation would use policy gradient methods
        size_t action_idx = (size_t)(action->action_vector[0] * agent->policy_size);
        if (action_idx >= agent->policy_size) action_idx = agent->policy_size - 1;
        
        // Soft update
        double update = agent->learning_rate * reward;
        agent->policy[action_idx] += update;
        
        // Renormalize
        double sum = 0.0;
        for (size_t i = 0; i < agent->policy_size; i++) {
            if (agent->policy[i] < 0.0) agent->policy[i] = 0.0;
            sum += agent->policy[i];
        }
        if (sum > 0.0) {
            for (size_t i = 0; i < agent->policy_size; i++) {
                agent->policy[i] /= sum;
            }
        }
    }
}

GameAction* agent_select_action(Agent* agent, const GameState* state) {
    GameAction* action = new GameAction;
    action->agent_id = agent->agent_id;
    action->action_size = agent->policy_size;
    action->action_vector = new double[action->action_size];
    action->timestamp = state->timestamp;
    action->confidence = 0.0;
    
    // Sample from policy
    double r = ((double)rand() / RAND_MAX);
    double cumsum = 0.0;
    size_t selected = 0;
    
    for (size_t i = 0; i < agent->policy_size; i++) {
        cumsum += agent->policy[i];
        if (r <= cumsum) {
            selected = i;
            break;
        }
    }
    
    // One-hot encoding
    for (size_t i = 0; i < action->action_size; i++) {
        action->action_vector[i] = (i == selected) ? 1.0 : 0.0;
    }
    action->confidence = agent->policy[selected];
    
    return action;
}

void agent_update_value(Agent* agent, const GameState* state, double value) {
    // Value function update (TD learning)
    if (agent->is_learning) {
        double error = value - agent->value[0];
        agent->value[0] += agent->learning_rate * error;
    }
}

MultiAgentGame* multi_agent_game_create(GameType game_type, size_t num_agents) {
    MultiAgentGame* game = new MultiAgentGame;
    game->game_type = game_type;
    game->num_agents = num_agents;
    game->agents = new Agent*[num_agents];
    game->current_agent_turn = 0;
    game->is_terminal = false;
    
    // Create agents
    size_t action_space_size = 100;  // Default, game-specific
    switch (game_type) {
        case GAME_CHESS:
            action_space_size = 64 * 64;  // From square to square
            break;
        case GAME_FOOTBALL:
        case GAME_BASKETBALL:
        case GAME_HOCKEY:
        case GAME_SOCCER:
            action_space_size = 100;  // Various actions
            break;
        default:
            action_space_size = 50;
            break;
    }
    
    for (size_t i = 0; i < num_agents; i++) {
        game->agents[i] = agent_create(i, AGENT_PLAYER, action_space_size);
    }
    
    // Create initial state
    game->current_state = new GameState;
    game->current_state->game_type = game_type;
    game->current_state->num_agents = num_agents;
    game->current_state->agents = game->agents;
    game->current_state->state_size = 1000;  // Default
    game->current_state->state_vector = new double[game->current_state->state_size];
    memset(game->current_state->state_vector, 0, game->current_state->state_size * sizeof(double));
    game->current_state->timestamp = 0.0;
    game->current_state->is_terminal = false;
    game->current_state->reward = 0.0;
    
    return game;
}

void multi_agent_game_destroy(MultiAgentGame* game) {
    if (game) {
        for (size_t i = 0; i < game->num_agents; i++) {
            agent_destroy(game->agents[i]);
        }
        delete[] game->agents;
        if (game->current_state) {
            delete[] game->current_state->state_vector;
            delete game->current_state;
        }
        delete game;
    }
}

void multi_agent_game_reset(MultiAgentGame* game) {
    game->current_agent_turn = 0;
    game->is_terminal = false;
    if (game->current_state) {
        memset(game->current_state->state_vector, 0, 
               game->current_state->state_size * sizeof(double));
        game->current_state->is_terminal = false;
        game->current_state->reward = 0.0;
        game->current_state->timestamp = 0.0;
    }
}

GameState* multi_agent_game_get_state(MultiAgentGame* game) {
    return game->current_state;
}

void multi_agent_game_apply_action(MultiAgentGame* game, const GameAction* action) {
    // Update game state based on action
    // This is game-specific and would be fully implemented per game type
    game->current_state->timestamp += 1.0;
    game->current_agent_turn = (game->current_agent_turn + 1) % game->num_agents;
}

bool multi_agent_game_is_terminal(MultiAgentGame* game) {
    return game->is_terminal || (game->current_state && game->current_state->is_terminal);
}

double multi_agent_game_get_reward(MultiAgentGame* game, size_t agent_id) {
    if (agent_id >= game->num_agents) return 0.0;
    return game->current_state ? game->current_state->reward : 0.0;
}

// Chess as multi-agent
MultiAgentGame* chess_as_multi_agent_create() {
    MultiAgentGame* game = multi_agent_game_create(GAME_CHESS, 2);  // White and Black
    return game;
}

void chess_multi_agent_reset(MultiAgentGame* game) {
    multi_agent_game_reset(game);
    // Reset chess position
}

GameAction* chess_action_from_move(const ChessMove* move) {
    GameAction* action = new GameAction;
    action->agent_id = 0;
    action->action_size = 4;
    action->action_vector = new double[4];
    action->action_vector[0] = (double)move->from / 64.0;
    action->action_vector[1] = (double)move->to / 64.0;
    action->action_vector[2] = (double)move->piece / 6.0;
    action->action_vector[3] = move->is_capture ? 1.0 : 0.0;
    action->timestamp = 0.0;
    action->confidence = 1.0;
    return action;
}

ChessMove* chess_move_from_action(const GameAction* action) {
    ChessMove* move = new ChessMove;
    move->from = (Square)(action->action_vector[0] * 64.0);
    move->to = (Square)(action->action_vector[1] * 64.0);
    move->piece = (PieceType)(action->action_vector[2] * 6.0);
    move->is_capture = action->action_vector[3] > 0.5;
    move->promotion = PIECE_NONE;
    move->is_castle = false;
    move->is_en_passant = false;
    return move;
}

// Sports game implementations (simplified)
MultiAgentGame* football_game_create(size_t num_players_per_team) {
    return multi_agent_game_create(GAME_FOOTBALL, num_players_per_team * 2);
}

MultiAgentGame* basketball_game_create(size_t num_players_per_team) {
    return multi_agent_game_create(GAME_BASKETBALL, num_players_per_team * 2);
}

MultiAgentGame* baseball_game_create() {
    return multi_agent_game_create(GAME_BASEBALL, 2);  // Two teams
}

MultiAgentGame* hockey_game_create(size_t num_players_per_team) {
    return multi_agent_game_create(GAME_HOCKEY, num_players_per_team * 2);
}

MultiAgentGame* soccer_game_create(size_t num_players_per_team) {
    return multi_agent_game_create(GAME_SOCCER, num_players_per_team * 2);
}

MultiAgentGame* tennis_game_create(bool doubles) {
    return multi_agent_game_create(GAME_TENNIS, doubles ? 4 : 2);
}

void game_state_to_vector(const GameState* state, double* vector, size_t* vector_size) {
    *vector_size = state->state_size;
    memcpy(vector, state->state_vector, state->state_size * sizeof(double));
}

GameState* game_state_from_vector(const double* vector, size_t vector_size, GameType game_type) {
    GameState* state = new GameState;
    state->state_size = vector_size;
    state->state_vector = new double[vector_size];
    memcpy(state->state_vector, vector, vector_size * sizeof(double));
    state->game_type = game_type;
    state->num_agents = 2;  // Default
    state->agents = nullptr;
    state->timestamp = 0.0;
    state->is_terminal = false;
    state->reward = 0.0;
    return state;
}
