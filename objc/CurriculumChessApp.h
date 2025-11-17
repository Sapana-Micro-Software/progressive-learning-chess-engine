/*
 * Copyright (C) 2025, Shyamal Suhana Chandra
 * All rights reserved.
 */
#import <Cocoa/Cocoa.h>

NS_ASSUME_NONNULL_BEGIN

@interface CurriculumChessApp : NSObject <NSApplicationDelegate>

@property (strong) NSWindow* mainWindow;
@property (strong) NSViewController* mainViewController;

- (void)applicationDidFinishLaunching:(NSNotification*)notification;
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender;

@end

NS_ASSUME_NONNULL_END
