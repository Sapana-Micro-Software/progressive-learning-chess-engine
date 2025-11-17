/*
 * Copyright (C) 2025, Shyamal Suhana Chandra
 * All rights reserved.
 */
#include "../include/pavlovian_learning.h"
#include "../include/chess_representation.h"
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <ctime>

// Pavlovian Learner Implementation (internal - header has typedef)
struct PavlovianLearnerImpl {
    CSUSAssociation* associations;
    size_t num_associations;
    size_t capacity;
    PavlovianType type;
    double learning_rate;
    double decay_rate;
    double threshold;
};

PavlovianLearner* pavlovian_learner_create(PavlovianType type, double learning_rate) {  // Create Pavlovian learning system with specified type and learning rate
    PavlovianLearnerImpl* learner = new PavlovianLearnerImpl;                  // Allocate memory for new Pavlovian learner structure
    learner->type = type;                                             // Set learning type classical conditioning reward or hybrid
    learner->learning_rate = learning_rate;                            // Set learning rate for association strength updates
    learner->decay_rate = 0.01;                                       // Set decay rate for extinction of associations over time
    learner->threshold = 0.1;                                         // Set threshold for association strength significance
    learner->capacity = 1000;                                         // Set initial capacity for association storage array
    learner->num_associations = 0;                                    // Initialize association count to zero
    learner->associations = new CSUSAssociation[learner->capacity];    // Allocate array for conditioned unconditioned stimulus associations
    return (PavlovianLearner*)learner;                                                    // Return pointer to initialized Pavlovian learner
}

void pavlovian_learner_destroy(PavlovianLearner* learner) {
    if (learner) {
        PavlovianLearnerImpl* impl = (PavlovianLearnerImpl*)learner;
        for (size_t i = 0; i < impl->num_associations; i++) {
            if (impl->associations[i].cs) {
                delete[] impl->associations[i].cs->stimulus_vector;
                delete impl->associations[i].cs;
            }
            if (impl->associations[i].us) {
                delete[] impl->associations[i].us->stimulus_vector;
                delete impl->associations[i].us;
            }
        }
        delete[] impl->associations;
        delete impl;
    }
}

// Find or create association
static CSUSAssociation* find_or_create_association(PavlovianLearner* learner,
                                                   const ConditionedStimulus* cs,
                                                   const UnconditionedStimulus* us) {
    PavlovianLearnerImpl* impl = (PavlovianLearnerImpl*)learner;
    // Check if association exists
    for (size_t i = 0; i < impl->num_associations; i++) {
        CSUSAssociation* assoc = &impl->associations[i];
        if (assoc->cs && assoc->us &&
            assoc->cs->stimulus_size == cs->stimulus_size &&
            assoc->us->stimulus_size == us->stimulus_size) {
            bool cs_match = true, us_match = true;
            for (size_t j = 0; j < cs->stimulus_size; j++) {
                if (fabs(assoc->cs->stimulus_vector[j] - cs->stimulus_vector[j]) > 0.01) {
                    cs_match = false;
                    break;
                }
            }
            for (size_t j = 0; j < us->stimulus_size; j++) {
                if (fabs(assoc->us->stimulus_vector[j] - us->stimulus_vector[j]) > 0.01) {
                    us_match = false;
                    break;
                }
            }
            if (cs_match && us_match) {
                return assoc;
            }
        }
    }
    
    // Create new association
    if (impl->num_associations >= impl->capacity) {
        size_t new_capacity = impl->capacity * 2;
        CSUSAssociation* new_assoc = new CSUSAssociation[new_capacity];
        memcpy(new_assoc, impl->associations, impl->num_associations * sizeof(CSUSAssociation));
        delete[] impl->associations;
        impl->associations = new_assoc;
        impl->capacity = new_capacity;
    }
    
    CSUSAssociation* assoc = &impl->associations[impl->num_associations++];
    assoc->cs = conditioned_stimulus_create(cs->stimulus_vector, cs->stimulus_size, cs->intensity);
    assoc->us = unconditioned_stimulus_create(us->stimulus_vector, us->stimulus_size, 
                                              us->reward_value, us->intensity);
    assoc->association_strength = 0.0;
    assoc->learning_rate = impl->learning_rate;
    assoc->pairings = 0;
    assoc->last_pairing_time = (double)time(nullptr);
    
    return assoc;
}

void pavlovian_pair_stimuli(PavlovianLearner* learner,                  // Pair conditioned stimulus with unconditioned stimulus for learning
                           const ConditionedStimulus* cs,
                           const UnconditionedStimulus* us) {
    PavlovianLearnerImpl* impl = (PavlovianLearnerImpl*)learner;
    CSUSAssociation* assoc = find_or_create_association(learner, cs, us);  // Find existing association or create new one
    
    double lambda = us->reward_value > 0 ? 1.0 : (us->reward_value < 0 ? -1.0 : 0.0);  // Compute target value from reward positive negative or zero
    double delta = impl->learning_rate * (lambda - assoc->association_strength);  // Compute update using Rescorla Wagner model
    assoc->association_strength += delta;                             // Update association strength with computed delta value
    
    if (assoc->association_strength > 1.0) assoc->association_strength = 1.0;  // Clamp association strength to maximum value one
    if (assoc->association_strength < -1.0) assoc->association_strength = -1.0;  // Clamp association strength to minimum value negative one
    
    assoc->pairings++;                                                 // Increment pairing count for this association
    assoc->last_pairing_time = (double)time(nullptr);                 // Update last pairing timestamp to current time
}

double pavlovian_get_association_strength(PavlovianLearner* learner,
                                         const ConditionedStimulus* cs,
                                         const UnconditionedStimulus* us) {
    CSUSAssociation* assoc = find_or_create_association(learner, cs, us);
    return assoc->association_strength;
}

void pavlovian_extinction(PavlovianLearner* learner, const ConditionedStimulus* cs) {
    // Extinction: present CS without US
    PavlovianLearnerImpl* impl = (PavlovianLearnerImpl*)learner;
    for (size_t i = 0; i < impl->num_associations; i++) {
        CSUSAssociation* assoc = &impl->associations[i];
        if (assoc->cs && assoc->cs->stimulus_size == cs->stimulus_size) {
            bool match = true;
            for (size_t j = 0; j < cs->stimulus_size; j++) {
                if (fabs(assoc->cs->stimulus_vector[j] - cs->stimulus_vector[j]) > 0.01) {
                    match = false;
                    break;
                }
            }
            if (match) {
                // Decay association strength
                assoc->association_strength *= (1.0 - impl->decay_rate);
            }
        }
    }
}

void pavlovian_reward(PavlovianLearner* learner, 
                     const ConditionedStimulus* cs,
                     double reward_value) {
    // Create US with positive reward
    UnconditionedStimulus* us = unconditioned_stimulus_create(cs->stimulus_vector, 
                                                              cs->stimulus_size,
                                                              reward_value, 1.0);
    pavlovian_pair_stimuli(learner, cs, us);
    unconditioned_stimulus_destroy(us);
}

void pavlovian_punish(PavlovianLearner* learner,
                     const ConditionedStimulus* cs,
                     double punishment_value) {
    // Create US with negative reward
    UnconditionedStimulus* us = unconditioned_stimulus_create(cs->stimulus_vector,
                                                              cs->stimulus_size,
                                                              -punishment_value, 1.0);
    pavlovian_pair_stimuli(learner, cs, us);
    unconditioned_stimulus_destroy(us);
}

double pavlovian_get_expected_reward(PavlovianLearner* learner,
                                     const ConditionedStimulus* cs) {
    // Find strongest association for this CS
    PavlovianLearnerImpl* impl = (PavlovianLearnerImpl*)learner;
    double max_strength = 0.0;
    double expected_reward = 0.0;
    
    for (size_t i = 0; i < impl->num_associations; i++) {
        CSUSAssociation* assoc = &impl->associations[i];
        if (assoc->cs && assoc->cs->stimulus_size == cs->stimulus_size) {
            bool match = true;
            for (size_t j = 0; j < cs->stimulus_size; j++) {
                if (fabs(assoc->cs->stimulus_vector[j] - cs->stimulus_vector[j]) > 0.01) {
                    match = false;
                    break;
                }
            }
            if (match && fabs(assoc->association_strength) > fabs(max_strength)) {
                max_strength = assoc->association_strength;
                expected_reward = assoc->us ? assoc->us->reward_value : 0.0;
            }
        }
    }
    
    return expected_reward * max_strength;
}

void pavlovian_reinforce_action(PavlovianLearner* learner,
                               const ConditionedStimulus* cs,
                               const double* action,
                               size_t action_size,
                               double reward) {
    // Create action-based CS
    double* action_cs_vector = new double[cs->stimulus_size + action_size];
    memcpy(action_cs_vector, cs->stimulus_vector, cs->stimulus_size * sizeof(double));
    memcpy(action_cs_vector + cs->stimulus_size, action, action_size * sizeof(double));
    
    ConditionedStimulus* action_cs = conditioned_stimulus_create(action_cs_vector, 
                                                                 cs->stimulus_size + action_size,
                                                                 cs->intensity);
    pavlovian_reward(learner, action_cs, reward);
    
    conditioned_stimulus_destroy(action_cs);
    delete[] action_cs_vector;
}

void pavlovian_punish_action(PavlovianLearner* learner,
                            const ConditionedStimulus* cs,
                            const double* action,
                            size_t action_size,
                            double punishment) {
    // Create action-based CS
    double* action_cs_vector = new double[cs->stimulus_size + action_size];
    memcpy(action_cs_vector, cs->stimulus_vector, cs->stimulus_size * sizeof(double));
    memcpy(action_cs_vector + cs->stimulus_size, action, action_size * sizeof(double));
    
    ConditionedStimulus* action_cs = conditioned_stimulus_create(action_cs_vector,
                                                                 cs->stimulus_size + action_size,
                                                                 cs->intensity);
    pavlovian_punish(learner, action_cs, punishment);
    
    conditioned_stimulus_destroy(action_cs);
    delete[] action_cs_vector;
}

void pavlovian_update_neural_network(PavlovianLearner* learner,
                                    void* neural_network,
                                    const ConditionedStimulus* cs,
                                    double target_output) {
    // Update neural network based on Pavlovian learning
    // This would integrate with the neural network training
    // For now, this is a placeholder
}

// Stimulus creation
ConditionedStimulus* conditioned_stimulus_create(const double* vector, size_t size, double intensity) {
    ConditionedStimulus* cs = new ConditionedStimulus;
    cs->stimulus_size = size;
    cs->intensity = intensity;
    cs->stimulus_vector = new double[size];
    memcpy(cs->stimulus_vector, vector, size * sizeof(double));
    cs->timestamp = (double)time(nullptr);
    cs->occurrence_count = 1;
    return cs;
}

void conditioned_stimulus_destroy(ConditionedStimulus* cs) {
    if (cs) {
        delete[] cs->stimulus_vector;
        delete cs;
    }
}

UnconditionedStimulus* unconditioned_stimulus_create(const double* vector, size_t size, 
                                                     double reward_value, double intensity) {
    UnconditionedStimulus* us = new UnconditionedStimulus;
    us->stimulus_size = size;
    us->reward_value = reward_value;
    us->intensity = intensity;
    us->stimulus_vector = new double[size];
    memcpy(us->stimulus_vector, vector, size * sizeof(double));
    us->timestamp = (double)time(nullptr);
    return us;
}

void unconditioned_stimulus_destroy(UnconditionedStimulus* us) {
    if (us) {
        delete[] us->stimulus_vector;
        delete us;
    }
}

// Chess-specific helpers
ConditionedStimulus* chess_position_to_cs(const ChessPosition* pos) {
    double* vector = new double[64 * 12];
    chess_position_to_matrix((ChessPosition*)pos, vector);
    ConditionedStimulus* cs = conditioned_stimulus_create(vector, 64 * 12, 1.0);
    delete[] vector;
    return cs;
}

UnconditionedStimulus* chess_outcome_to_us(double outcome) {
    double* vector = new double[1];
    vector[0] = outcome;
    UnconditionedStimulus* us = unconditioned_stimulus_create(vector, 1, outcome, 1.0);
    delete[] vector;
    return us;
}
