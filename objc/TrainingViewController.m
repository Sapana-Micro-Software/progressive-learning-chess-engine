#import "TrainingViewController.h"
#import "../include/training_engine.h"
#import "../include/neural_network.h"
#import "../include/curriculum_learning.h"
#import <Foundation/Foundation.h>

@interface TrainingViewController ()
{
    TrainingEngine* trainingEngine;
    NSTimer* updateTimer;
    BOOL isTraining;
}
@end

@implementation TrainingViewController

- (void)loadView {
    // Create view programmatically
    NSView* view = [[NSView alloc] initWithFrame:NSMakeRect(0, 0, 800, 600)];
    self.view = view;
    
    // Create UI elements programmatically
    self.learningRateSlider = [[NSSlider alloc] initWithFrame:NSMakeRect(50, 50, 200, 20)];
    self.learningRateLabel = [[NSTextField alloc] initWithFrame:NSMakeRect(260, 50, 200, 20)];
    self.currentLevelLabel = [[NSTextField alloc] initWithFrame:NSMakeRect(50, 100, 300, 20)];
    self.accuracyLabel = [[NSTextField alloc] initWithFrame:NSMakeRect(50, 130, 300, 20)];
    self.lossLabel = [[NSTextField alloc] initWithFrame:NSMakeRect(50, 160, 300, 20)];
    self.startTrainingButton = [[NSButton alloc] initWithFrame:NSMakeRect(50, 200, 100, 30)];
    self.pauseTrainingButton = [[NSButton alloc] initWithFrame:NSMakeRect(160, 200, 100, 30)];
    self.optimizerPopup = [[NSPopUpButton alloc] initWithFrame:NSMakeRect(50, 240, 150, 25)];
    self.progressIndicator = [[NSProgressIndicator alloc] initWithFrame:NSMakeRect(50, 280, 300, 20)];
    self.useCurriculumCheckbox = [[NSButton alloc] initWithFrame:NSMakeRect(50, 320, 200, 20)];
    self.usePavlovianCheckbox = [[NSButton alloc] initWithFrame:NSMakeRect(50, 350, 200, 20)];
    self.useSpacedRepetitionCheckbox = [[NSButton alloc] initWithFrame:NSMakeRect(50, 380, 200, 20)];
    
    [view addSubview:self.learningRateSlider];
    [view addSubview:self.learningRateLabel];
    [view addSubview:self.currentLevelLabel];
    [view addSubview:self.accuracyLabel];
    [view addSubview:self.lossLabel];
    [view addSubview:self.startTrainingButton];
    [view addSubview:self.pauseTrainingButton];
    [view addSubview:self.optimizerPopup];
    [view addSubview:self.progressIndicator];
    [view addSubview:self.useCurriculumCheckbox];
    [view addSubview:self.usePavlovianCheckbox];
    [view addSubview:self.useSpacedRepetitionCheckbox];
    
    [self.startTrainingButton setTitle:@"Start Training"];
    [self.pauseTrainingButton setTitle:@"Pause"];
    [self.useCurriculumCheckbox setTitle:@"Use Curriculum Learning"];
    [self.usePavlovianCheckbox setTitle:@"Use Pavlovian Learning"];
    [self.useSpacedRepetitionCheckbox setTitle:@"Use Spaced Repetition"];
    
    [self.startTrainingButton setTarget:self];
    [self.startTrainingButton setAction:@selector(startTraining:)];
    [self.pauseTrainingButton setTarget:self];
    [self.pauseTrainingButton setAction:@selector(pauseTraining:)];
    [self.learningRateSlider setTarget:self];
    [self.learningRateSlider setAction:@selector(learningRateChanged:)];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    // Initialize UI
    [self.learningRateSlider setMinValue:0.0001];
    [self.learningRateSlider setMaxValue:0.1];
    [self.learningRateSlider setDoubleValue:0.01];
    [self.learningRateLabel setStringValue:@"Learning Rate: 0.01"];
    
    [self.optimizerPopup removeAllItems];
    [self.optimizerPopup addItemWithTitle:@"SGD"];
    [self.optimizerPopup addItemWithTitle:@"Adam"];
    [self.optimizerPopup addItemWithTitle:@"Adagrad"];
    [self.optimizerPopup addItemWithTitle:@"RMSprop"];
    
    [self.useCurriculumCheckbox setState:NSControlStateValueOn];
    [self.usePavlovianCheckbox setState:NSControlStateValueOn];
    [self.useSpacedRepetitionCheckbox setState:NSControlStateValueOn];
    
    [self.pauseTrainingButton setEnabled:NO];
    
    isTraining = NO;
}

- (IBAction)startTraining:(id)sender {
    if (isTraining) return;
    
    // Create neural network
    NeuralNetwork* nn = nn_create_hybrid(768, 512, 4096);  // 8x8x12 input, 64x64 output
    
    // Create training config
    TrainingConfig config;
    config.optimizer_type = (OptimizerType)[self.optimizerPopup indexOfSelectedItem];
    config.learning_rate = [self.learningRateSlider doubleValue];
    config.momentum = 0.9;
    config.weight_decay = 0.0001;
    config.batch_size = 32;
    config.max_epochs = 1000;
    config.early_stopping_threshold = 0.001;
    config.use_curriculum = [self.useCurriculumCheckbox state] == NSControlStateValueOn;
    config.use_pavlovian = [self.usePavlovianCheckbox state] == NSControlStateValueOn;
    config.use_spaced_repetition = [self.useSpacedRepetitionCheckbox state] == NSControlStateValueOn;
    config.mastery_threshold = 0.85;
    config.patience = 10;
    
    // Create training engine
    trainingEngine = training_engine_create(nn, &config);
    
    // Start training in background
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        isTraining = YES;
        dispatch_async(dispatch_get_main_queue(), ^{
            [self.startTrainingButton setEnabled:NO];
            [self.pauseTrainingButton setEnabled:YES];
            [self.progressIndicator startAnimation:nil];
        });
        
        // Training loop
        while (isTraining) {
            if (config.use_curriculum) {
                training_engine_train_with_curriculum(trainingEngine);
            } else {
                training_engine_train_epoch(trainingEngine);
            }
            
            dispatch_async(dispatch_get_main_queue(), ^{
                [self updateTrainingStats];
            });
            
            // Small delay
            [NSThread sleepForTimeInterval:0.1];
        }
        
        dispatch_async(dispatch_get_main_queue(), ^{
            [self.startTrainingButton setEnabled:YES];
            [self.pauseTrainingButton setEnabled:NO];
            [self.progressIndicator stopAnimation:nil];
        });
    });
    
    // Start update timer
    updateTimer = [NSTimer scheduledTimerWithTimeInterval:1.0
                                                   target:self
                                                 selector:@selector(updateTrainingStats)
                                                 userInfo:nil
                                                  repeats:YES];
}

- (IBAction)pauseTraining:(id)sender {
    isTraining = NO;
    if (updateTimer) {
        [updateTimer invalidate];
        updateTimer = nil;
    }
}

- (IBAction)learningRateChanged:(id)sender {
    double lr = [self.learningRateSlider doubleValue];
    [self.learningRateLabel setStringValue:[NSString stringWithFormat:@"Learning Rate: %.4f", lr]];
}

- (void)updateTrainingStats {
    if (!trainingEngine) return;
    
    TrainingStats* stats = training_engine_get_stats(trainingEngine);
    
    const char* levelNames[] = {
        "Preschool", "Kindergarten", "Elementary", "Middle School",
        "High School", "Undergrad", "Graduate", "Master", "Grandmaster", "Infinite"
    };
    
    [self.currentLevelLabel setStringValue:[NSString stringWithFormat:@"Current Level: %s", 
                                            levelNames[stats->current_level]]];
    [self.accuracyLabel setStringValue:[NSString stringWithFormat:@"Accuracy: %.2f%%", 
                                       stats->accuracy * 100.0]];
    [self.lossLabel setStringValue:[NSString stringWithFormat:@"Loss: %.6f", 
                                   stats->current_loss]];
    
    [self.progressIndicator setDoubleValue:stats->epoch % 100];
}

- (void)dealloc {
    if (trainingEngine) {
        training_engine_destroy(trainingEngine);
    }
    if (updateTimer) {
        [updateTimer invalidate];
    }
}

@end
