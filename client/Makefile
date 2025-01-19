CXX      := -c++
CXXFLAGS := -std=c++11 -pedantic-errors -Wall -Wextra -Werror -pthread
LDFLAGS  := -L/usr/lib -lstdc++ -lm -lncursesw
BUILD    := ./build
OBJ_DIR  := $(BUILD)/objects
TEST_DIR  := $(BUILD)/test
APP_DIR  := $(BUILD)/apps
TARGET   := main
INCLUDE  := -Iinclude/
SRC      :=                      \
   $(wildcard src/lib/*.cpp) \
   $(wildcard src/*.cpp) \
   $(wildcard src/lib/common/*.cpp) \

TEST_SRC      :=                      \
   $(wildcard src/test/*.cpp) 
TEST_BASE := $(basename $(TEST_SRC))
TEST_FILES := $(notdir $(TEST_SRC))
TEST_OBJECTS := $(TEST_FILES:%.cpp=$(TEST_DIR)/%)

OBJECTS  := $(SRC:%.cpp=$(OBJ_DIR)/%.o)
DEPENDENCIES \
         := $(OBJECTS:.o=.d)


all: build $(APP_DIR)/$(TARGET)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -MMD -o $@

$(APP_DIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $(APP_DIR)/$(TARGET) $^ $(LDFLAGS) 

-include $(DEPENDENCIES)

.PHONY: all build clean debug release info test

src/test/%:
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $(TEST_DIR)/$(@F) $@.cpp

test: build $(TEST_BASE)

build:
	@mkdir -p $(TEST_DIR)
	@mkdir -p $(APP_DIR)
	@mkdir -p $(OBJ_DIR)

debug: CXXFLAGS += -DDEBUG -g
debug: all

release: CXXFLAGS += -O2
release: all

clean:
	-@rm -rvf $(OBJ_DIR)/*
	-@rm -rvf $(APP_DIR)/*
	-@rm -rvf $(TEST_DIR)/*

info:
	@echo "[*] Application dir: ${APP_DIR}     "
	@echo "[*] Object dir:      ${OBJ_DIR}     "
	@echo "[*] Sources:         ${SRC}         "
	@echo "[*] Objects:         ${OBJECTS}     "
	@echo "[*] Dependencies:    ${DEPENDENCIES}"

