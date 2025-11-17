/*
 * Copyright (C) 2025, Shyamal Suhana Chandra
 * All rights reserved.
 */
#import "CurriculumChessApp.h"
#import "TrainingViewController.h"
#import "ChessBoardViewController.h"

@implementation CurriculumChessApp

- (void)applicationDidFinishLaunching:(NSNotification*)notification {
    // Create main window
    NSRect frame = NSMakeRect(0, 0, 1200, 800);
    NSWindowStyleMask style = NSWindowStyleMaskTitled | 
                             NSWindowStyleMaskClosable | 
                             NSWindowStyleMaskMiniaturizable | 
                             NSWindowStyleMaskResizable;
    
    self.mainWindow = [[NSWindow alloc] initWithContentRect:frame
                                                   styleMask:style
                                                     backing:NSBackingStoreBuffered
                                                       defer:NO];
    [self.mainWindow setTitle:@"Curriculum Chess Learning System"];
    [self.mainWindow center];
    
    // Create tab view controller
    NSTabViewController* tabController = [[NSTabViewController alloc] init];
    
    // Training tab
    TrainingViewController* trainingVC = [[TrainingViewController alloc] init];
    NSTabViewItem* trainingTab = [NSTabViewItem tabViewItemWithViewController:trainingVC];
    trainingTab.label = @"Training";
    [tabController addTabViewItem:trainingTab];
    
    // Chess board tab
    ChessBoardViewController* boardVC = [[ChessBoardViewController alloc] init];
    NSTabViewItem* boardTab = [NSTabViewItem tabViewItemWithViewController:boardVC];
    boardTab.label = @"Chess Board";
    [tabController addTabViewItem:boardTab];
    
    self.mainViewController = tabController;
    [self.mainWindow setContentViewController:tabController];
    [self.mainWindow makeKeyAndOrderFront:nil];
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender {
    return YES;
}

@end
