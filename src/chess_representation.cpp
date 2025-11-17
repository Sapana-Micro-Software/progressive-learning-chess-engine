/*
 * Copyright (C) 2025, Shyamal Suhana Chandra
 * All rights reserved.
 */
#include "../include/chess_representation.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>

// Chess Position Implementation
struct ChessPosition {
    PieceType board[64];  // 8x8 board
    Color colors[64];
    bool white_to_move;
    bool white_castle_kingside;
    bool white_castle_queenside;
    bool black_castle_kingside;
    bool black_castle_queenside;
    Square en_passant_square;
    size_t halfmove_clock;
    size_t fullmove_number;
    
    // Move history for unmake
    struct MoveHistory {
        ChessMove move;
        PieceType captured_piece;
        Color captured_color;
        bool white_castle_kingside;
        bool white_castle_queenside;
        bool black_castle_kingside;
        bool black_castle_queenside;
        Square en_passant_square;
    } move_history[1000];
    size_t move_history_count;
};

ChessPosition* chess_position_create() {                               // Create new empty chess position with default initial state
    ChessPosition* pos = new ChessPosition;                            // Allocate memory for new chess position structure
    memset(pos->board, 0, 64 * sizeof(PieceType));                     // Initialize board array to empty squares for all sixty four squares
    memset(pos->colors, 0, 64 * sizeof(Color));                       // Initialize color array to zero for all sixty four squares
    pos->white_to_move = true;                                         // Set active player to white for initial position
    pos->white_castle_kingside = true;                                 // Enable white kingside castling rights initially
    pos->white_castle_queenside = true;                                // Enable white queenside castling rights initially
    pos->black_castle_kingside = true;                                 // Enable black kingside castling rights initially
    pos->black_castle_queenside = true;                                // Enable black queenside castling rights initially
    pos->en_passant_square = 0;                                        // Initialize en passant target square to none
    pos->halfmove_clock = 0;                                           // Initialize halfmove clock for fifty move rule
    pos->fullmove_number = 1;                                         // Initialize fullmove counter starting at move one
    pos->move_history_count = 0;                                      // Initialize move history counter to zero
    
    return pos;                                                        // Return pointer to initialized chess position
}

void chess_position_destroy(ChessPosition* pos) {
    if (pos) {
        delete pos;
    }
}

// FEN parsing (simplified - full implementation would be more complex)
ChessPosition* chess_position_from_fen(const char* fen) {
    ChessPosition* pos = chess_position_create();
    
    // Simplified FEN parsing (full implementation would parse all fields)
    // Format: "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
    
    size_t square = 0;
    const char* p = fen;
    
    // Parse board
    while (*p && *p != ' ') {
        if (*p == '/') {
            // Next rank
        } else if (*p >= '1' && *p <= '8') {
            // Empty squares
            square += (*p - '0');
        } else {
            // Piece
            PieceType piece = PIECE_NONE;
            Color color = COLOR_WHITE;
            
            switch (*p) {
                case 'P': piece = PIECE_PAWN; color = COLOR_WHITE; break;
                case 'R': piece = PIECE_ROOK; color = COLOR_WHITE; break;
                case 'N': piece = PIECE_KNIGHT; color = COLOR_WHITE; break;
                case 'B': piece = PIECE_BISHOP; color = COLOR_WHITE; break;
                case 'Q': piece = PIECE_QUEEN; color = COLOR_WHITE; break;
                case 'K': piece = PIECE_KING; color = COLOR_WHITE; break;
                case 'p': piece = PIECE_PAWN; color = COLOR_BLACK; break;
                case 'r': piece = PIECE_ROOK; color = COLOR_BLACK; break;
                case 'n': piece = PIECE_KNIGHT; color = COLOR_BLACK; break;
                case 'b': piece = PIECE_BISHOP; color = COLOR_BLACK; break;
                case 'q': piece = PIECE_QUEEN; color = COLOR_BLACK; break;
                case 'k': piece = PIECE_KING; color = COLOR_BLACK; break;
            }
            
            if (square < 64) {
                pos->board[square] = piece;
                pos->colors[square] = color;
                square++;
            }
        }
        p++;
    }
    
    // Parse active color
    while (*p == ' ') p++;
    if (*p == 'w') pos->white_to_move = true;
    else if (*p == 'b') pos->white_to_move = false;
    
    // Parse castling rights (simplified)
    while (*p && *p != ' ') p++;
    while (*p == ' ') p++;
    // Full implementation would parse KQkq format
    
    return pos;
}

void chess_position_to_fen(ChessPosition* pos, FENString* fen) {
    char* buffer = fen->fen_string;
    size_t idx = 0;
    
    // Convert board to FEN
    for (int rank = 7; rank >= 0; rank--) {
        int empty_count = 0;
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            PieceType piece = pos->board[square];
            
            if (piece == PIECE_NONE) {
                empty_count++;
            } else {
                if (empty_count > 0) {
                    buffer[idx++] = '0' + empty_count;
                    empty_count = 0;
                }
                
                char piece_char = ' ';
                Color color = pos->colors[square];
                switch (piece) {
                    case PIECE_PAWN: piece_char = color == COLOR_WHITE ? 'P' : 'p'; break;
                    case PIECE_ROOK: piece_char = color == COLOR_WHITE ? 'R' : 'r'; break;
                    case PIECE_KNIGHT: piece_char = color == COLOR_WHITE ? 'N' : 'n'; break;
                    case PIECE_BISHOP: piece_char = color == COLOR_WHITE ? 'B' : 'b'; break;
                    case PIECE_QUEEN: piece_char = color == COLOR_WHITE ? 'Q' : 'q'; break;
                    case PIECE_KING: piece_char = color == COLOR_WHITE ? 'K' : 'k'; break;
                    default: break;
                }
                buffer[idx++] = piece_char;
            }
        }
        if (empty_count > 0) {
            buffer[idx++] = '0' + empty_count;
        }
        if (rank > 0) {
            buffer[idx++] = '/';
        }
    }
    
    // Add active color
    buffer[idx++] = ' ';
    buffer[idx++] = pos->white_to_move ? 'w' : 'b';
    
    // Add castling rights (simplified)
    buffer[idx++] = ' ';
    if (pos->white_castle_kingside) buffer[idx++] = 'K';
    if (pos->white_castle_queenside) buffer[idx++] = 'Q';
    if (pos->black_castle_kingside) buffer[idx++] = 'k';
    if (pos->black_castle_queenside) buffer[idx++] = 'q';
    if (!pos->white_castle_kingside && !pos->white_castle_queenside &&
        !pos->black_castle_kingside && !pos->black_castle_queenside) {
        buffer[idx++] = '-';
    }
    
    // Add en passant
    buffer[idx++] = ' ';
    if (pos->en_passant_square == 0) {
        buffer[idx++] = '-';
    } else {
        // Convert square to algebraic notation (simplified)
        buffer[idx++] = 'a' + (pos->en_passant_square % 8);
        buffer[idx++] = '1' + (pos->en_passant_square / 8);
    }
    
    // Add halfmove and fullmove
    sprintf(buffer + idx, " %zu %zu", pos->halfmove_clock, pos->fullmove_number);
    
    buffer[255] = '\0';
}

void chess_position_to_matrix(ChessPosition* pos, double* matrix) {     // Convert chess position to eight by eight by twelve tensor representation
    memset(matrix, 0, 8 * 8 * 12 * sizeof(double));                    // Initialize entire matrix to zero for all squares and channels
    
    for (size_t square = 0; square < 64; square++) {                   // Iterate through each square on eight by eight board
        PieceType piece = pos->board[square];                           // Get piece type at current square
        if (piece == PIECE_NONE) continue;                              // Skip empty squares with no piece present
        
        Color color = pos->colors[square];                              // Get color of piece at current square
        size_t channel = (piece - 1) * 2 + color;                      // Calculate channel index from piece type and color
        
        matrix[square * 12 + channel] = 1.0;                           // Set matrix value to one indicating piece presence
    }
}

void chess_position_from_matrix(ChessPosition* pos, const double* matrix) {
    memset(pos->board, 0, 64 * sizeof(PieceType));
    memset(pos->colors, 0, 64 * sizeof(Color));
    
    for (size_t square = 0; square < 64; square++) {
        double max_val = 0.0;
        size_t best_channel = 0;
        
        for (size_t channel = 0; channel < 12; channel++) {
            if (matrix[square * 12 + channel] > max_val) {
                max_val = matrix[square * 12 + channel];
                best_channel = channel;
            }
        }
        
        if (max_val > 0.5) {  // Threshold
            PieceType piece = (PieceType)((best_channel / 2) + 1);
            Color color = (Color)(best_channel % 2);
            pos->board[square] = piece;
            pos->colors[square] = color;
        }
    }
}

PieceType chess_position_get_piece(ChessPosition* pos, Square square) {
    if (square >= 64) return PIECE_NONE;
    return pos->board[square];
}

Color chess_position_get_color(ChessPosition* pos, Square square) {
    if (square >= 64) return COLOR_WHITE;
    return pos->colors[square];
}

bool chess_position_is_valid(ChessPosition* pos) {
    // Basic validation (full implementation would check more)
    return true;
}

bool chess_position_is_check(ChessPosition* pos, Color color) {
    // Simplified - full implementation would check if king is under attack
    return false;
}

bool chess_position_is_checkmate(ChessPosition* pos, Color color) {
    // Simplified - full implementation would check checkmate conditions
    return false;
}

bool chess_position_is_stalemate(ChessPosition* pos) {
    // Simplified - full implementation would check stalemate conditions
    return false;
}

void chess_position_generate_moves(ChessPosition* pos, Color color, ChessMove* moves, size_t* num_moves) {
    *num_moves = 0;
    // Simplified move generation - full implementation would generate all legal moves
}

bool chess_position_is_legal_move(ChessPosition* pos, const ChessMove* move) {
    // Simplified - full implementation would check move legality
    return true;
}

void chess_position_make_move(ChessPosition* pos, const ChessMove* move) {
    if (pos->move_history_count >= 1000) return;
    
    auto* hist = &pos->move_history[pos->move_history_count];
    hist->move = *move;
    hist->captured_piece = pos->board[move->to];
    hist->captured_color = pos->colors[move->to];
    hist->white_castle_kingside = pos->white_castle_kingside;
    hist->white_castle_queenside = pos->white_castle_queenside;
    hist->black_castle_kingside = pos->black_castle_kingside;
    hist->black_castle_queenside = pos->black_castle_queenside;
    hist->en_passant_square = pos->en_passant_square;
    
    // Make move
    pos->board[move->to] = pos->board[move->from];
    pos->colors[move->to] = pos->colors[move->from];
    pos->board[move->from] = PIECE_NONE;
    
    pos->white_to_move = !pos->white_to_move;
    pos->move_history_count++;
}

void chess_position_unmake_move(ChessPosition* pos) {
    if (pos->move_history_count == 0) return;
    
    auto* hist = &pos->move_history[pos->move_history_count - 1];
    ChessMove* move = &hist->move;
    
    // Restore position
    pos->board[move->from] = pos->board[move->to];
    pos->colors[move->from] = pos->colors[move->to];
    pos->board[move->to] = hist->captured_piece;
    pos->colors[move->to] = hist->captured_color;
    
    pos->white_castle_kingside = hist->white_castle_kingside;
    pos->white_castle_queenside = hist->white_castle_queenside;
    pos->black_castle_kingside = hist->black_castle_kingside;
    pos->black_castle_queenside = hist->black_castle_queenside;
    pos->en_passant_square = hist->en_passant_square;
    
    pos->white_to_move = !pos->white_to_move;
    pos->move_history_count--;
}

// Move Sequence Implementation
MoveSequence* move_sequence_create(size_t capacity) {
    MoveSequence* seq = new MoveSequence;
    seq->capacity = capacity;
    seq->num_moves = 0;
    seq->moves = new ChessMove[capacity];
    return seq;
}

void move_sequence_destroy(MoveSequence* seq) {
    if (seq) {
        delete[] seq->moves;
        delete seq;
    }
}

void move_sequence_add_move(MoveSequence* seq, const ChessMove* move) {
    if (seq->num_moves >= seq->capacity) {
        size_t new_capacity = seq->capacity * 2;
        ChessMove* new_moves = new ChessMove[new_capacity];
        memcpy(new_moves, seq->moves, seq->num_moves * sizeof(ChessMove));
        delete[] seq->moves;
        seq->moves = new_moves;
        seq->capacity = new_capacity;
    }
    seq->moves[seq->num_moves++] = *move;
}

void move_sequence_to_vector(MoveSequence* seq, double* vector) {
    // Convert move sequence to input vector (simplified)
    for (size_t i = 0; i < seq->num_moves && i < 100; i++) {
        vector[i * 4 + 0] = (double)seq->moves[i].from / 64.0;
        vector[i * 4 + 1] = (double)seq->moves[i].to / 64.0;
        vector[i * 4 + 2] = (double)seq->moves[i].piece / 6.0;
        vector[i * 4 + 3] = seq->moves[i].is_capture ? 1.0 : 0.0;
    }
}

MoveSequence* move_sequence_from_vector(const double* vector, size_t vector_size) {
    MoveSequence* seq = move_sequence_create(vector_size / 4);
    
    for (size_t i = 0; i < vector_size / 4; i++) {
        ChessMove move;
        move.from = (Square)(vector[i * 4 + 0] * 64.0);
        move.to = (Square)(vector[i * 4 + 1] * 64.0);
        move.piece = (PieceType)(vector[i * 4 + 2] * 6.0);
        move.is_capture = vector[i * 4 + 3] > 0.5;
        move_sequence_add_move(seq, &move);
    }
    
    return seq;
}

// Game Implementation
struct ChessGame {
    ChessPosition* position;
    MoveSequence* moves;
};

ChessGame* chess_game_create() {
    ChessGame* game = new ChessGame;
    game->position = chess_position_create();
    game->moves = move_sequence_create(1000);
    return game;
}

void chess_game_destroy(ChessGame* game) {
    if (game) {
        chess_position_destroy(game->position);
        move_sequence_destroy(game->moves);
        delete game;
    }
}

void chess_game_add_move(ChessGame* game, const ChessMove* move) {
    chess_position_make_move(game->position, move);
    move_sequence_add_move(game->moves, move);
}

ChessPosition* chess_game_get_position(ChessGame* game) {
    return game->position;
}

void chess_game_to_input_vector(ChessGame* game, double* vector, size_t vector_size) {
    // Convert game state to input vector
    chess_position_to_matrix(game->position, vector);
    // Could also append move history
}

// Chess Variant Implementation
ChessVariant* chess_variant_create_infinite() {
    ChessVariant* variant = new ChessVariant;
    variant->infinite_board = true;
    variant->custom_pieces = false;
    variant->variant_rules = false;
    variant->board_width = 8;
    variant->board_height = 8;
    return variant;
}

void chess_variant_destroy(ChessVariant* variant) {
    if (variant) {
        delete variant;
    }
}

ChessPosition* chess_variant_create_position(ChessVariant* variant) {
    return chess_position_create();
}
