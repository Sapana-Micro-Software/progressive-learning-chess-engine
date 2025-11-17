#include "../include/curriculum_learning.h"
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <ctime>

// Difficulty Level Structure (internal implementation detail)
struct DifficultyLevel {
    DifficultyLevelEnum level;
    TrainingExample* examples;
    size_t num_examples;
    size_t capacity;
    double mastery_threshold;
    double current_accuracy;
    size_t examples_seen;
};

// Curriculum Implementation (internal - header has typedef)
struct CurriculumImpl {
    DifficultyLevel* levels;
    size_t num_levels;
    size_t current_level;
    double mastery_threshold;
    size_t examples_per_level;
};

Curriculum* curriculum_create(size_t num_levels) {                    // Create curriculum learning system with specified number of difficulty levels
    CurriculumImpl* curriculum = new CurriculumImpl;                          // Allocate memory for new curriculum structure
    curriculum->num_levels = num_levels;                             // Set total number of difficulty levels in curriculum
    curriculum->current_level = 0;                                    // Initialize current level to first level preschool
    curriculum->mastery_threshold = 0.85;                             // Set accuracy threshold required to advance to next level
    curriculum->examples_per_level = 1000;                            // Set default number of training examples per difficulty level
    
    curriculum->levels = new DifficultyLevel[num_levels];            // Allocate array of difficulty level structures
    for (size_t i = 0; i < num_levels; i++) {                         // Initialize each difficulty level with default values
        curriculum->levels[i].level = (DifficultyLevelEnum)i;         // Set level enumeration value for this difficulty level
        curriculum->levels[i].num_examples = 0;                       // Initialize example count to zero for new level
        curriculum->levels[i].capacity = curriculum->examples_per_level;  // Set initial capacity for example storage
        curriculum->levels[i].examples = new TrainingExample[curriculum->examples_per_level];  // Allocate example array
        curriculum->levels[i].mastery_threshold = 0.85;              // Set mastery threshold for this specific level
        curriculum->levels[i].current_accuracy = 0.0;                 // Initialize accuracy tracking to zero
        curriculum->levels[i].examples_seen = 0;                      // Initialize example counter to zero
    }
    
    return (Curriculum*)curriculum;                                                // Return pointer to initialized curriculum structure
}

void curriculum_destroy(Curriculum* curriculum) {
    if (curriculum) {
        CurriculumImpl* impl = (CurriculumImpl*)curriculum;
        for (size_t i = 0; i < impl->num_levels; i++) {
            for (size_t j = 0; j < impl->levels[i].num_examples; j++) {
                delete[] impl->levels[i].examples[j].input;
                delete[] impl->levels[i].examples[j].target;
            }
            delete[] impl->levels[i].examples;
        }
        delete[] impl->levels;
        delete impl;
    }
}

void curriculum_add_example(Curriculum* curriculum, TrainingExample* example, DifficultyLevelEnum level) {  // Add training example to specified difficulty level in curriculum
    CurriculumImpl* impl = (CurriculumImpl*)curriculum;
    if (level >= impl->num_levels) return;                            // Validate level index is within valid range
    
    DifficultyLevel* dl = &impl->levels[level];                 // Get pointer to difficulty level structure
    if (dl->num_examples >= dl->capacity) {                           // Check if example array needs resizing
        size_t new_capacity = dl->capacity * 2;                      // Double capacity for dynamic array growth
        TrainingExample* new_examples = new TrainingExample[new_capacity];  // Allocate larger example array
        memcpy(new_examples, dl->examples, dl->num_examples * sizeof(TrainingExample));  // Copy existing examples
        delete[] dl->examples;                                        // Free old example array memory
        dl->examples = new_examples;                                  // Update pointer to new larger array
        dl->capacity = new_capacity;                                   // Update capacity value
    }
    
    TrainingExample* ex = &dl->examples[dl->num_examples];            // Get pointer to next available example slot
    ex->input_size = example->input_size;                            // Copy input vector size from source example
    ex->target_size = example->target_size;                           // Copy target vector size from source example
    ex->difficulty = example->difficulty;                              // Copy difficulty score from source example
    ex->input = new double[example->input_size];                     // Allocate memory for input vector
    ex->target = new double[example->target_size];                   // Allocate memory for target vector
    memcpy(ex->input, example->input, example->input_size * sizeof(double));  // Copy input data to new memory
    memcpy(ex->target, example->target, example->target_size * sizeof(double));  // Copy target data to new memory
    ex->is_correct = false;                                           // Initialize correctness flag to false
    ex->attempts = 0;                                                 // Initialize attempt counter to zero
    ex->correct_streak = 0;                                          // Initialize consecutive correct counter to zero
    ex->last_reviewed = (double)time(nullptr);                       // Set last review time to current timestamp
    ex->next_review = ex->last_reviewed;                              // Set next review time to current time initially
    
    dl->num_examples++;                                               // Increment example count for this difficulty level
}

bool curriculum_should_advance(Curriculum* curriculum, double accuracy) {  // Check if curriculum should advance to next difficulty level
    CurriculumImpl* impl = (CurriculumImpl*)curriculum;
    if (impl->current_level >= impl->num_levels - 1) {                    // Check if already at highest difficulty level
        return false;                                                      // Return false cannot advance beyond highest level
    }
    
    DifficultyLevel* current = &impl->levels[impl->current_level];         // Get pointer to current difficulty level structure
    current->current_accuracy = accuracy;                                 // Update current accuracy value for this level
    
    return accuracy >= impl->mastery_threshold;                            // Return true if accuracy meets or exceeds mastery threshold
}

void curriculum_advance_level(Curriculum* curriculum) {                    // Advance curriculum to next higher difficulty level
    CurriculumImpl* impl = (CurriculumImpl*)curriculum;
    if (impl->current_level < impl->num_levels - 1) {                     // Check if not already at highest level
        impl->current_level++;                                             // Increment current level index to next difficulty
    }
}

DifficultyLevelEnum curriculum_get_current_level(Curriculum* curriculum) {  // Get current difficulty level enumeration value
    CurriculumImpl* impl = (CurriculumImpl*)curriculum;
    return impl->levels[impl->current_level].level;                        // Return level enumeration for current difficulty
}

// Spaced Repetition Implementation (internal - header has typedef)
struct SpacedRepetitionImpl {
    TrainingExample* examples;
    size_t num_examples;
    size_t capacity;
    double ltm_threshold;  // Correct streak needed for LTM (e.g., 5)
    double initial_interval;  // Initial review interval in hours
};

SpacedRepetition* spaced_repetition_create(size_t capacity, double ltm_threshold) {  // Create spaced repetition system with capacity and LTM threshold
    SpacedRepetitionImpl* sr = new SpacedRepetitionImpl;                        // Allocate memory for new spaced repetition structure
    sr->capacity = capacity;                                           // Set maximum number of examples that can be stored
    sr->num_examples = 0;                                              // Initialize example count to zero
    sr->examples = new TrainingExample[capacity];                       // Allocate array for training examples with spaced repetition
    sr->ltm_threshold = ltm_threshold;                                 // Set correct streak threshold for long term memory transition
    sr->initial_interval = 1.0;                                       // Set initial review interval to one hour in hours
    return (SpacedRepetition*)sr;                                                          // Return pointer to initialized spaced repetition system
}

void spaced_repetition_destroy(SpacedRepetition* sr) {
    if (sr) {
        SpacedRepetitionImpl* impl = (SpacedRepetitionImpl*)sr;
        for (size_t i = 0; i < impl->num_examples; i++) {
            delete[] impl->examples[i].input;
            delete[] impl->examples[i].target;
        }
        delete[] impl->examples;
        delete impl;
    }
}

void spaced_repetition_add_example(SpacedRepetition* sr, TrainingExample* example) {
    SpacedRepetitionImpl* impl = (SpacedRepetitionImpl*)sr;
    if (impl->num_examples >= impl->capacity) {
        size_t new_capacity = impl->capacity * 2;
        TrainingExample* new_examples = new TrainingExample[new_capacity];
        for (size_t i = 0; i < impl->num_examples; i++) {
            new_examples[i] = impl->examples[i];
        }
        delete[] impl->examples;
        impl->examples = new_examples;
        impl->capacity = new_capacity;
    }
    
    TrainingExample* ex = &impl->examples[impl->num_examples];
    ex->input_size = example->input_size;
    ex->target_size = example->target_size;
    ex->difficulty = example->difficulty;
    ex->input = new double[example->input_size];
    ex->target = new double[example->target_size];
    memcpy(ex->input, example->input, example->input_size * sizeof(double));
    memcpy(ex->target, example->target, example->target_size * sizeof(double));
    ex->is_correct = false;
    ex->attempts = 0;
    ex->correct_streak = 0;
    double now = (double)time(nullptr);
    ex->last_reviewed = now;
    ex->next_review = now + impl->initial_interval * 3600.0;  // Convert hours to seconds
    
    impl->num_examples++;
}

TrainingExample* spaced_repetition_get_next_review(SpacedRepetition* sr) {
    SpacedRepetitionImpl* impl = (SpacedRepetitionImpl*)sr;
    double now = (double)time(nullptr);
    TrainingExample* next = nullptr;
    double earliest = 1e10;
    
    for (size_t i = 0; i < impl->num_examples; i++) {
        if (impl->examples[i].next_review <= now) {
            if (impl->examples[i].next_review < earliest) {
                earliest = impl->examples[i].next_review;
                next = &impl->examples[i];
            }
        }
    }
    
    return next;
}

void spaced_repetition_update_example(SpacedRepetition* sr, size_t index, bool is_correct) {  // Update example after review with correctness result
    SpacedRepetitionImpl* impl = (SpacedRepetitionImpl*)sr;
    if (index >= impl->num_examples) return;                            // Return early if index is out of bounds
    
    TrainingExample* ex = &impl->examples[index];                       // Get pointer to example being updated
    ex->attempts++;                                                    // Increment total number of attempts for this example
    ex->is_correct = is_correct;                                       // Update correctness flag with review result
    double now = (double)time(nullptr);                                // Get current timestamp in seconds
    ex->last_reviewed = now;                                           // Update last reviewed timestamp to current time
    
    if (is_correct) {                                                 // Check if answer was correct for this review
        ex->correct_streak++;                                          // Increment consecutive correct answer counter
        
        double interval_multiplier = 2.5;                             // Initialize interval multiplier for exponential spacing
        if (ex->correct_streak > 1) {                                 // Check if streak is greater than one for increased multiplier
            interval_multiplier = 2.5 + (ex->correct_streak - 1) * 0.5;  // Increase multiplier based on correct streak length
        }
        
        double current_interval = (now - ex->last_reviewed) / 3600.0;  // Calculate current interval in hours from last review
        if (current_interval < 0.1) current_interval = impl->initial_interval;  // Use initial interval if calculated too small
        
        double next_interval = current_interval * interval_multiplier;  // Calculate next review interval using exponential spacing
        ex->next_review = now + next_interval * 3600.0;               // Set next review time by adding interval in seconds
    } else {                                                           // Handle incorrect answer case
        ex->correct_streak = 0;                                       // Reset correct streak to zero on incorrect answer
        ex->next_review = now + impl->initial_interval * 3600.0;       // Reset next review to initial interval from now
    }
}

bool spaced_repetition_is_in_ltm(SpacedRepetition* sr, size_t index) {
    SpacedRepetitionImpl* impl = (SpacedRepetitionImpl*)sr;
    if (index >= impl->num_examples) return false;
    return impl->examples[index].correct_streak >= impl->ltm_threshold;
}

// Puzzle Generator Implementation (matches typedef in header)

PuzzleGenerator* puzzle_generator_create(Curriculum* curriculum) {
    PuzzleGenerator* pg = new PuzzleGenerator();
    pg->curriculum = curriculum;
    pg->target_level = LEVEL_PRESCHOOL;
    pg->puzzle_count = 0;
    return pg;
}

void puzzle_generator_destroy(PuzzleGenerator* pg) {
    if (pg) {
        delete pg;
    }
}

// Generate puzzle based on difficulty level
TrainingExample* puzzle_generator_create_puzzle(PuzzleGenerator* pg, DifficultyLevelEnum level) {
    TrainingExample* puzzle = new TrainingExample;
    
    // Determine puzzle characteristics based on level
    size_t input_size = 64 * 12;  // 8x8 board, 12 channels
    size_t output_size = 64 * 64;  // From square to square move probabilities
    
    switch (level) {
        case LEVEL_PRESCHOOL:
            // Very simple: single piece movements
            input_size = 64;  // Simplified
            output_size = 8;  // 8 possible directions
            break;
        case LEVEL_KINDERGARTEN:
            // Simple captures
            input_size = 64 * 2;
            output_size = 64;
            break;
        case LEVEL_ELEMENTARY:
            // Basic checkmates
            input_size = 64 * 6;
            output_size = 64 * 64;
            break;
        default:
            // Full representation
            input_size = 64 * 12;
            output_size = 64 * 64;
            break;
    }
    
    puzzle->input_size = input_size;
    puzzle->target_size = output_size;
    puzzle->input = new double[input_size];
    puzzle->target = new double[output_size];
    puzzle->difficulty = (double)level / (double)LEVEL_INFINITE;
    
    // Initialize with random values (in real implementation, would generate actual chess positions)
    for (size_t i = 0; i < input_size; i++) {
        puzzle->input[i] = ((double)rand() / RAND_MAX) * 0.1;
    }
    for (size_t i = 0; i < output_size; i++) {
        puzzle->target[i] = ((double)rand() / RAND_MAX) * 0.1;
    }
    
    puzzle->is_correct = false;
    puzzle->attempts = 0;
    puzzle->correct_streak = 0;
    double now = (double)time(nullptr);
    puzzle->last_reviewed = now;
    puzzle->next_review = now;
    
    pg->puzzle_count++;
    
    return puzzle;
}

TrainingExample* puzzle_generator_create_progressive_puzzle(PuzzleGenerator* pg, double difficulty) {
    // Map difficulty (0.0-1.0) to level
    DifficultyLevelEnum level = (DifficultyLevelEnum)(difficulty * LEVEL_INFINITE);
    if (level > LEVEL_INFINITE) level = LEVEL_INFINITE;
    
    return puzzle_generator_create_puzzle(pg, level);
}
