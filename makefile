# Temporary definition to fix usage on my system.
ifeq ($(OS),Windows_NT)
	SHELL := cmd
endif

CXX = gcc
C_FLAGS = -Wall -Wextra -fno-exceptions -fno-rtti -Wno-write-strings -Wno-unused-parameter -Wno-sign-compare
LD_FLAGS = -lstdc++

SRC_DIR = src
INC_DIR = inc
OBJ_DIR = obj
BUILD_DIR = build

rwildcard = $(wildcard $(1)/$(2)) $(foreach d,$(wildcard $(1)/*),$(call rwildcard,$(d),$(2)))

SRC = $(call rwildcard,$(SRC_DIR),*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC))
DIRS = $(wildcard $(SRC_DIR)/*)

OUTPUT = block.exe

build: $(OBJS)
	@echo ==== LINKING ====
	@if not exist $(BUILD_DIR) (mkdir $(BUILD_DIR))
	$(CXX) -o $(BUILD_DIR)\$(OUTPUT) $(OBJS) -I$(INC_DIR) $(LD_FLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo ==== COMPILING ====
	@echo Location: $<
	@if not exist $(@D) (mkdir $(@D))
	$(CXX) $(C_FLAGS) -c $< -o $@ -I$(INC_DIR)

clean:
	@echo ==== CLEANING ====
	@if exist $(OBJ_DIR) (rmdir /s /q $(OBJ_DIR))
	@if exist $(BUILD_DIR) (rmdir /s /q $(BUILD_DIR))

run:
	@echo ==== RUNNING $(OUTPUT) ====
	@$(BUILD_DIR)\$(OUTPUT)

all: clean build run