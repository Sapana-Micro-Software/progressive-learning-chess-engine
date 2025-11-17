/*
 * Copyright (C) 2025, Shyamal Suhana Chandra
 * All rights reserved.
 */
#import <Cocoa/Cocoa.h>

NS_ASSUME_NONNULL_BEGIN

@interface ChessBoardViewController : NSViewController

@property (weak) IBOutlet NSView* chessBoardView;
@property (weak) IBOutlet NSTextField* positionEvaluationLabel;
@property (weak) IBOutlet NSTextField* movePredictionLabel;
@property (weak) IBOutlet NSButton* newGameButton;
@property (weak) IBOutlet NSButton* loadModelButton;
@property (weak) IBOutlet NSTextField* modelPathField;

- (IBAction)newGame:(id)sender;
- (IBAction)loadModel:(id)sender;
- (void)drawChessBoard;

@end

NS_ASSUME_NONNULL_END
