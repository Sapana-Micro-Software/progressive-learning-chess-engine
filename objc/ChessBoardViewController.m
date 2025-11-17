/*
 * Copyright (C) 2025, Shyamal Suhana Chandra
 * All rights reserved.
 */
#import "ChessBoardViewController.h"
#import "../include/inference_engine.h"
#import "../include/chess_representation.h"
#import "../include/neural_network.h"
#import <Foundation/Foundation.h>

@interface ChessBoardView : NSView
@property (weak) ChessBoardViewController* controller;
@end

@implementation ChessBoardView

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
    
    // Draw chess board
    NSRect bounds = [self bounds];
    CGFloat squareSize = bounds.size.width / 8.0;
    
    BOOL isWhite = YES;
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            NSRect square = NSMakeRect(file * squareSize, rank * squareSize, 
                                      squareSize, squareSize);
            
            NSColor* color = isWhite ? [NSColor lightGrayColor] : [NSColor darkGrayColor];
            [color setFill];
            NSRectFill(square);
            
            isWhite = !isWhite;
        }
        isWhite = !isWhite;
    }
    
    // Draw border
    [[NSColor blackColor] setStroke];
    NSBezierPath* path = [NSBezierPath bezierPathWithRect:bounds];
    [path setLineWidth:2.0];
    [path stroke];
}

- (void)mouseDown:(NSEvent*)event {
    // Handle square selection
    NSPoint location = [self convertPoint:[event locationInWindow] fromView:nil];
    NSRect bounds = [self bounds];
    CGFloat squareSize = bounds.size.width / 8.0;
    
    int file = (int)(location.x / squareSize);
    int rank = (int)(location.y / squareSize);
    
    if (file >= 0 && file < 8 && rank >= 0 && rank < 8) {
        // Handle square click
        NSLog(@"Clicked square: %c%c", 'a' + file, '1' + rank);
    }
}

@end

@interface ChessBoardViewController ()
{
    InferenceEngine* inferenceEngine;
    ChessPosition* currentPosition;
    ChessGame* currentGame;
}
@end

@implementation ChessBoardViewController

- (void)loadView {
    // Create view programmatically
    NSView* view = [[NSView alloc] initWithFrame:NSMakeRect(0, 0, 800, 600)];
    self.view = view;
    
    // Create chess board view
    ChessBoardView* boardView = [[ChessBoardView alloc] initWithFrame:NSMakeRect(50, 200, 400, 400)];
    boardView.controller = self;
    self.chessBoardView = boardView;
    [view addSubview:boardView];
    
    // Create other UI elements
    self.positionEvaluationLabel = [[NSTextField alloc] initWithFrame:NSMakeRect(50, 150, 400, 20)];
    self.movePredictionLabel = [[NSTextField alloc] initWithFrame:NSMakeRect(50, 120, 400, 20)];
    self.newGameButton = [[NSButton alloc] initWithFrame:NSMakeRect(500, 500, 100, 30)];
    self.loadModelButton = [[NSButton alloc] initWithFrame:NSMakeRect(500, 460, 100, 30)];
    self.modelPathField = [[NSTextField alloc] initWithFrame:NSMakeRect(500, 420, 250, 20)];
    
    [view addSubview:self.positionEvaluationLabel];
    [view addSubview:self.movePredictionLabel];
    [view addSubview:self.newGameButton];
    [view addSubview:self.loadModelButton];
    [view addSubview:self.modelPathField];
    
    [self.newGameButton setTitle:@"New Game"];
    [self.loadModelButton setTitle:@"Load Model"];
    [self.newGameButton setTarget:self];
    [self.newGameButton setAction:@selector(newGame:)];
    [self.loadModelButton setTarget:self];
    [self.loadModelButton setAction:@selector(loadModel:)];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    // Initialize chess position
    currentPosition = chess_position_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    currentGame = chess_game_create();
    
    inferenceEngine = nil;
}

- (IBAction)newGame:(id)sender {
    if (currentPosition) {
        chess_position_destroy(currentPosition);
    }
    if (currentGame) {
        chess_game_destroy(currentGame);
    }
    
    currentPosition = chess_position_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    currentGame = chess_game_create();
    
    [self drawChessBoard];
    [self updateEvaluation];
}

- (IBAction)loadModel:(id)sender {
    NSOpenPanel* panel = [NSOpenPanel openPanel];
    [panel setAllowedFileTypes:@[@"model", @"bin"]];
    [panel setCanChooseFiles:YES];
    [panel setCanChooseDirectories:NO];
    
    if ([panel runModal] == NSModalResponseOK) {
        NSURL* url = [panel URL];
        NSString* path = [url path];
        [self.modelPathField setStringValue:path];
        
        // Load model
        if (inferenceEngine) {
            inference_engine_destroy(inferenceEngine);
        }
        
        NeuralNetwork* nn = nn_create_hybrid(768, 512, 4096);
        inferenceEngine = inference_engine_create(nn);
        inference_engine_load_model(inferenceEngine, [path UTF8String]);
        
        [self updateEvaluation];
    }
}

- (void)drawChessBoard {
    [self.chessBoardView setNeedsDisplay:YES];
}

- (void)updateEvaluation {
    if (!inferenceEngine || !currentPosition) return;
    
    double eval = inference_engine_evaluate_position(inferenceEngine, currentPosition);
    [self.positionEvaluationLabel setStringValue:
        [NSString stringWithFormat:@"Position Evaluation: %.4f", eval]];
    
    MoveEvaluation* moveEval = inference_engine_predict_move(inferenceEngine, currentPosition);
    if (moveEval) {
        char from_file = 'a' + (moveEval->move.from % 8);
        char from_rank = '1' + (moveEval->move.from / 8);
        char to_file = 'a' + (moveEval->move.to % 8);
        char to_rank = '1' + (moveEval->move.to / 8);
        
        NSString* moveStr = [NSString stringWithFormat:@"%c%c-%c%c (%.2f)", 
                            from_file, from_rank, to_file, to_rank, moveEval->confidence];
        [self.movePredictionLabel setStringValue:moveStr];
        
        free(moveEval);
    }
}

- (void)dealloc {
    if (inferenceEngine) {
        inference_engine_destroy(inferenceEngine);
    }
    if (currentPosition) {
        chess_position_destroy(currentPosition);
    }
    if (currentGame) {
        chess_game_destroy(currentGame);
    }
}

@end
