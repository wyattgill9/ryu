CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -O2
LDFLAGS = 
SRC_DIR = ./src
BUILD_DIR = ./build
INCLUDE_DIRS = ./include 
LIB_DIRS = 
TARGET = ryu

SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp) 
OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

$(shell mkdir -p $(BUILD_DIR))

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(CXX) $(OBJ_FILES) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIRS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all clean run
