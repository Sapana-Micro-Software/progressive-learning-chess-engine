#import <Cocoa/Cocoa.h>
#import "CurriculumChessApp.h"

int main(int argc, const char* argv[]) {
    @autoreleasepool {
        NSApplication* app = [NSApplication sharedApplication];
        CurriculumChessApp* delegate = [[CurriculumChessApp alloc] init];
        [app setDelegate:delegate];
        [app run];
    }
    return 0;
}
