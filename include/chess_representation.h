/*
 * Copyright (C) 2025, Shyamal Suhana Chandra
 * All rights reserved.
 */
#ifndef CHESS_REPRESENTATION_H
#define CHESS_REPRESENTATION_H

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations for structs defined in .cpp files
typedef struct ChessPosition ChessPosition;
typedef struct ChessGame ChessGame;

// Piece types
typedef enum {
    PIECE_NONE = 0,
    PIECE_PAWN,
    PIECE_ROOK,
    PIECE_KNIGHT,
    PIECE_BISHOP,
    PIECE_QUEEN,
    PIECE_KING
} PieceType;

// Colors
typedef enum {
    COLOR_WHITE = 0,
    COLOR_BLACK = 1
} Color;

// Square representation (0-63)
typedef unsigned char Square;

// Move representation
typedef struct {
    Square from;
    Square to;
    PieceType piece;
    PieceType promotion;  // For pawn promotion
    bool is_castle;
    bool is_en_passant;
    bool is_capture;
} ChessMove;

// FEN string representation
typedef struct {
    char fen_string[256];
} FENString;

// Board matrix representation (8x8x12 channels: 6 piece types x 2 colors)
#define BOARD_CHANNELS 12
#define BOARD_SIZE 8

// Move sequence representation
typedef struct {
    ChessMove* moves;
    size_t num_moves;
    size_t capacity;
} MoveSequence;

// Chess Position API
ChessPosition* chess_position_create();
void chess_position_destroy(ChessPosition* pos);
ChessPosition* chess_position_from_fen(const char* fen);
void chess_position_to_fen(ChessPosition* pos, FENString* fen);
void chess_position_to_matrix(ChessPosition* pos, double* matrix);  // 8x8x12 output
void chess_position_from_matrix(ChessPosition* pos, const double* matrix);
PieceType chess_position_get_piece(ChessPosition* pos, Square square);
Color chess_position_get_color(ChessPosition* pos, Square square);
bool chess_position_is_valid(ChessPosition* pos);
bool chess_position_is_check(ChessPosition* pos, Color color);
bool chess_position_is_checkmate(ChessPosition* pos, Color color);
bool chess_position_is_stalemate(ChessPosition* pos);

// Move generation
void chess_position_generate_moves(ChessPosition* pos, Color color, ChessMove* moves, size_t* num_moves);
bool chess_position_is_legal_move(ChessPosition* pos, const ChessMove* move);
void chess_position_make_move(ChessPosition* pos, const ChessMove* move);
void chess_position_unmake_move(ChessPosition* pos);

// Move sequence API
MoveSequence* move_sequence_create(size_t capacity);
void move_sequence_destroy(MoveSequence* seq);
void move_sequence_add_move(MoveSequence* seq, const ChessMove* move);
void move_sequence_to_vector(MoveSequence* seq, double* vector);  // Convert to input vector
MoveSequence* move_sequence_from_vector(const double* vector, size_t vector_size);

// Game API
ChessGame* chess_game_create();
void chess_game_destroy(ChessGame* game);
void chess_game_add_move(ChessGame* game, const ChessMove* move);
ChessPosition* chess_game_get_position(ChessGame* game);
void chess_game_to_input_vector(ChessGame* game, double* vector, size_t vector_size);

// Infinite chess variants
typedef struct {
    bool infinite_board;
    bool custom_pieces;
    bool variant_rules;
    size_t board_width;
    size_t board_height;
} ChessVariant;

ChessVariant* chess_variant_create_infinite();
void chess_variant_destroy(ChessVariant* variant);
ChessPosition* chess_variant_create_position(ChessVariant* variant);

#ifdef __cplusplus
}
#endif

#endif // CHESS_REPRESENTATION_H
