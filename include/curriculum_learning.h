/*
 * Copyright (C) 2025, Shyamal Suhana Chandra
 * All rights reserved.
 */
#ifndef CURRICULUM_LEARNING_H
#define CURRICULUM_LEARNING_H

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations for structs defined in .cpp files
typedef struct DifficultyLevel DifficultyLevel;

// Difficulty levels (preschool to advanced)
typedef enum {
    LEVEL_PRESCHOOL = 0,      // Basic piece movements
    LEVEL_KINDERGARTEN,       // Simple captures
    LEVEL_ELEMENTARY,          // Basic checkmates
    LEVEL_MIDDLE_SCHOOL,      // Tactical patterns
    LEVEL_HIGH_SCHOOL,        // Strategic concepts
    LEVEL_UNDERGRAD,          // Complex tactics
    LEVEL_GRADUATE,           // Advanced strategy
    LEVEL_MASTER,             // Master-level play
    LEVEL_GRANDMASTER,        // GM-level play
    LEVEL_INFINITE            // Infinite chess variants
} DifficultyLevelEnum;

// Example structure
typedef struct {
    double* input;            // Game state representation
    double* target;           // Expected output
    double difficulty;        // Difficulty score (0.0 - 1.0)
    size_t input_size;
    size_t target_size;
    bool is_correct;          // For spaced repetition
    size_t attempts;          // Number of attempts
    size_t correct_streak;    // Consecutive correct answers
    double last_reviewed;     // Timestamp of last review
    double next_review;       // When to review next (spaced repetition)
} TrainingExample;

// Curriculum structure
typedef struct {
    DifficultyLevel* levels;
    size_t num_levels;
    size_t current_level;
    double mastery_threshold;  // Required accuracy to advance
    size_t examples_per_level;
} Curriculum;

// Spaced Repetition System (Quizlet-like)
typedef struct {
    TrainingExample* examples;
    size_t num_examples;
    size_t capacity;
    double ltm_threshold;     // Correct streak needed for LTM
    double initial_interval; // Initial review interval (hours)
} SpacedRepetition;

// Curriculum API
Curriculum* curriculum_create(size_t num_levels);
void curriculum_destroy(Curriculum* curriculum);
void curriculum_add_example(Curriculum* curriculum, TrainingExample* example, DifficultyLevelEnum level);
bool curriculum_should_advance(Curriculum* curriculum, double accuracy);
void curriculum_advance_level(Curriculum* curriculum);
DifficultyLevelEnum curriculum_get_current_level(Curriculum* curriculum);

// Spaced Repetition API
SpacedRepetition* spaced_repetition_create(size_t capacity, double ltm_threshold);
void spaced_repetition_destroy(SpacedRepetition* sr);
void spaced_repetition_add_example(SpacedRepetition* sr, TrainingExample* example);
TrainingExample* spaced_repetition_get_next_review(SpacedRepetition* sr);
void spaced_repetition_update_example(SpacedRepetition* sr, size_t index, bool is_correct);
bool spaced_repetition_is_in_ltm(SpacedRepetition* sr, size_t index);

// Puzzle Generator
typedef struct {
    Curriculum* curriculum;
    DifficultyLevelEnum target_level;
    size_t puzzle_count;
} PuzzleGenerator;

PuzzleGenerator* puzzle_generator_create(Curriculum* curriculum);
void puzzle_generator_destroy(PuzzleGenerator* pg);
TrainingExample* puzzle_generator_create_puzzle(PuzzleGenerator* pg, DifficultyLevelEnum level);
TrainingExample* puzzle_generator_create_progressive_puzzle(PuzzleGenerator* pg, double difficulty);

#ifdef __cplusplus
}
#endif

#endif // CURRICULUM_LEARNING_H
