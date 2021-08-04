CXX      := -clang++
CXXFLAGS := -W -Wall -Wextra -pedantic -Wno-vla -std=c++17
LDFLAGS  := -L/usr/lib -lstdc++ -lm
BUILD    := ./build
OBJ_DIR  := $(BUILD)/objects
APP_DIR  := $(BUILD)/apps
TARGET   := TUP
INCLUDE  := -Iinclude/
SRC      := $(wildcard src/*.cpp)

OBJECTS  := $(SRC:src/%.cpp=$(OBJ_DIR)/%.o)
DEPENDENCIES \
         := $(OBJECTS:.o=.d)

all: build $(APP_DIR)/$(TARGET)

$(OBJ_DIR)/%.o: src/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -MMD -o $(OBJ_DIR)/$(basename $(notdir $@)).o

$(APP_DIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $(APP_DIR)/$(TARGET) $^ $(LDFLAGS)
	@ln -s $(APP_DIR)/$(TARGET) $(TARGET)

-include $(DEPENDENCIES)

.PHONY: all build clean debug release info

build:
	@mkdir -p $(APP_DIR)
	@mkdir -p $(OBJ_DIR)

debug: CXXFLAGS += -DDEBUG -g
debug: all

release: CXXFLAGS += -O3
release: all

test-4: clean debug
	./TUP instances/umps4.txt 2 3 1000

test-6: clean debug
	./TUP instances/umps6.txt 2 3 1000

test-8: clean debug
	./TUP instances/umps8.txt 2 3 1000

test-10: clean debug
	./TUP instances/umps10.txt 2 3 1000

test-12: clean debug
	./TUP instances/umps12.txt 2 3 1000

test-14: clean debug
	./TUP instances/umps14.txt 2 3 1000

test-16: clean debug
	./TUP instances/umps16.txt 2 3 1000

test-18: clean debug
	./TUP instances/umps18.txt 2 3 1000

test-20: clean debug
	./TUP instances/umps20.txt 2 3 1000

test-30: clean debug
	./TUP instances/umps30.txt 2 3 1000

clean:
	-@rm -rvf $(OBJ_DIR)/*
	-@rm -rvf $(APP_DIR)/*
	-@rm -rvf $(TARGET)

info:
	@echo "[*] Application dir: ${APP_DIR}     "
	@echo "[*] Object dir:      ${OBJ_DIR}     "
	@echo "[*] Sources:         ${SRC}         "
	@echo "[*] Objects:         ${OBJECTS}     "
	@echo "[*] Dependencies:    ${DEPENDENCIES}"
