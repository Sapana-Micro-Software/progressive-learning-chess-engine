#ifndef PAVLOVIAN_LEARNING_H
#define PAVLOVIAN_LEARNING_H

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations
typedef struct ChessPosition ChessPosition;

// Pavlovian learning types
typedef enum {
    PAVLOVIAN_CLASSICAL_CONDITIONING,  // CS-US association
    PAVLOVIAN_REWARD_BASED,            // Reward/punishment learning
    PAVLOVIAN_INSTRUMENTAL,            // Operant conditioning
    PAVLOVIAN_HYBRID                   // Combined approach
} PavlovianType;

// Stimulus types
typedef struct {
    double* stimulus_vector;
    size_t stimulus_size;
    double intensity;
    double timestamp;
    size_t occurrence_count;
} ConditionedStimulus;

typedef struct {
    double* stimulus_vector;
    size_t stimulus_size;
    double reward_value;      // Positive or negative reward
    double intensity;
    double timestamp;
} UnconditionedStimulus;

// Association strength
typedef struct {
    ConditionedStimulus* cs;
    UnconditionedStimulus* us;
    double association_strength;  // 0.0 to 1.0
    double learning_rate;
    size_t pairings;
    double last_pairing_time;
} CSUSAssociation;

// Pavlovian Learner
typedef struct {
    CSUSAssociation* associations;
    size_t num_associations;
    size_t capacity;
    PavlovianType type;
    double learning_rate;
    double decay_rate;
    double threshold;
} PavlovianLearner;

// Pavlovian Learning API
PavlovianLearner* pavlovian_learner_create(PavlovianType type, double learning_rate);
void pavlovian_learner_destroy(PavlovianLearner* learner);

// Classical Conditioning (CS-US pairing)
void pavlovian_pair_stimuli(PavlovianLearner* learner, 
                           const ConditionedStimulus* cs,
                           const UnconditionedStimulus* us);
double pavlovian_get_association_strength(PavlovianLearner* learner,
                                         const ConditionedStimulus* cs,
                                         const UnconditionedStimulus* us);
void pavlovian_extinction(PavlovianLearner* learner, const ConditionedStimulus* cs);

// Reward-based learning
void pavlovian_reward(PavlovianLearner* learner, 
                     const ConditionedStimulus* cs,
                     double reward_value);
void pavlovian_punish(PavlovianLearner* learner,
                     const ConditionedStimulus* cs,
                     double punishment_value);
double pavlovian_get_expected_reward(PavlovianLearner* learner,
                                     const ConditionedStimulus* cs);

// Instrumental/Operant conditioning
void pavlovian_reinforce_action(PavlovianLearner* learner,
                               const ConditionedStimulus* cs,
                               const double* action,
                               size_t action_size,
                               double reward);
void pavlovian_punish_action(PavlovianLearner* learner,
                            const ConditionedStimulus* cs,
                            const double* action,
                            size_t action_size,
                            double punishment);

// Integration with neural networks
void pavlovian_update_neural_network(PavlovianLearner* learner,
                                    void* neural_network,  // NeuralNetwork*
                                    const ConditionedStimulus* cs,
                                    double target_output);

// Stimulus creation
ConditionedStimulus* conditioned_stimulus_create(const double* vector, size_t size, double intensity);
void conditioned_stimulus_destroy(ConditionedStimulus* cs);
UnconditionedStimulus* unconditioned_stimulus_create(const double* vector, size_t size, 
                                                    double reward_value, double intensity);
void unconditioned_stimulus_destroy(UnconditionedStimulus* us);

// For chess: positions are CS, outcomes (win/loss/draw) are US
ConditionedStimulus* chess_position_to_cs(const ChessPosition* pos);
UnconditionedStimulus* chess_outcome_to_us(double outcome);  // 1.0=win, 0.0=draw, -1.0=loss

#ifdef __cplusplus
}
#endif

#endif // PAVLOVIAN_LEARNING_H
