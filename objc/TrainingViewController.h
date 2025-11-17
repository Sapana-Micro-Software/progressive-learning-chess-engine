#import <Cocoa/Cocoa.h>

NS_ASSUME_NONNULL_BEGIN

@interface TrainingViewController : NSViewController

@property (weak) IBOutlet NSTextField* currentLevelLabel;
@property (weak) IBOutlet NSProgressIndicator* progressIndicator;
@property (weak) IBOutlet NSTextField* accuracyLabel;
@property (weak) IBOutlet NSTextField* lossLabel;
@property (weak) IBOutlet NSButton* startTrainingButton;
@property (weak) IBOutlet NSButton* pauseTrainingButton;
@property (weak) IBOutlet NSPopUpButton* optimizerPopup;
@property (weak) IBOutlet NSSlider* learningRateSlider;
@property (weak) IBOutlet NSTextField* learningRateLabel;
@property (weak) IBOutlet NSButton* useCurriculumCheckbox;
@property (weak) IBOutlet NSButton* usePavlovianCheckbox;
@property (weak) IBOutlet NSButton* useSpacedRepetitionCheckbox;

- (IBAction)startTraining:(id)sender;
- (IBAction)pauseTraining:(id)sender;
- (IBAction)learningRateChanged:(id)sender;
- (void)updateTrainingStats;

@end

NS_ASSUME_NONNULL_END
