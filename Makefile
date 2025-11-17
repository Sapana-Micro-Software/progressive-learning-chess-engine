# Makefile for Curriculum Chess Learning System

CXX = clang++
CC = clang
OBJC = clang
OBJCFLAGS = -fobjc-arc -framework Cocoa -framework Foundation

CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -I./include
CFLAGS = -std=c11 -O2 -Wall -Wextra -I./include

# Source files
CXX_SOURCES = $(wildcard src/*.cpp)
CXX_OBJECTS = $(CXX_SOURCES:.cpp=.o)

OBJC_SOURCES = $(wildcard objc/*.m)
OBJC_OBJECTS = $(OBJC_SOURCES:.m=.o)

# Targets
TARGET_CLI = curriculum_chess
TARGET_GUI = CurriculumChess.app/Contents/MacOS/CurriculumChess

.PHONY: all clean cli gui

all: cli gui

cli: $(TARGET_CLI)

gui: $(TARGET_GUI)

$(TARGET_CLI): $(CXX_OBJECTS) src/main.o
	$(CXX) $(CXXFLAGS) -o $@ $^ -lm

$(TARGET_GUI): $(CXX_OBJECTS) $(OBJC_OBJECTS) objc/main.o
	$(OBJC) $(OBJCFLAGS) $(CXXFLAGS) -o $@ $^ -lm
	mkdir -p CurriculumChess.app/Contents/MacOS
	mv $@ CurriculumChess.app/Contents/MacOS/ 2>/dev/null || true

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.m
	$(OBJC) $(OBJCFLAGS) -c $< -o $@

clean:
	rm -f $(CXX_OBJECTS) $(OBJC_OBJECTS) src/main.o objc/main.o
	rm -f $(TARGET_CLI) $(TARGET_GUI)
	rm -rf CurriculumChess.app

test: $(TARGET_CLI) test_runner
	./test_runner

# Filter out main.o from source objects for test runner
TEST_CXX_OBJECTS = $(filter-out src/main.o,$(CXX_OBJECTS))

test_runner: tests/test_main.o tests/test_harness.o tests/unit_tests.o tests/regression_tests.o tests/ab_tests.o tests/blackbox_tests.o tests/ux_tests.o $(TEST_CXX_OBJECTS)
	$(CXX) $(CXXFLAGS) -o test_runner $^ -lm

tests/%.o: tests/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
