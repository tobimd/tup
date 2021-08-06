CXX      := -clang++
CXXFLAGS := -W -Wall -Wextra -pedantic -Wno-vla -std=c++17
LDFLAGS  := -L@/usr/lib -lstdc++ -lm
BUILD    := ./build
OBJ_DIR  := $(BUILD)/objects
APP_DIR  := $(BUILD)/apps
TARGET   := TUP
INCLUDE  := -Iinclude/
SRC      :=                                  \
	$(subst src/,, $(wildcard src/**/*.cpp)) \
	$(subst src/,, $(wildcard src/*.cpp))    \

OBJECTS  := $(SRC:%.cpp=$(OBJ_DIR)/%.o)
DEPENDENCIES \
         := $(OBJECTS:.o=.d)

Q1       := 2
Q2       := 3
K        := 5

all: build $(APP_DIR)/$(TARGET)

$(OBJ_DIR)/%.o: src/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -MMD -o $@

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
	@/usr/bin/time -p ./TUP instances/umps4.txt $(Q1) $(Q2) $(K)

test-6: clean debug
	@/usr/bin/time -p ./TUP instances/umps6.txt $(Q1) $(Q2) $(K)

test-8: clean debug
	@/usr/bin/time -p ./TUP instances/umps8.txt $(Q1) $(Q2) $(K)

test-10: clean debug
	@/usr/bin/time -p ./TUP instances/umps10.txt $(Q1) $(Q2) $(K)

test-12: clean debug
	@/usr/bin/time -p ./TUP instances/umps12.txt $(Q1) $(Q2) $(K)

test-14: clean debug
	@/usr/bin/time -p ./TUP instances/umps14.txt $(Q1) $(Q2) $(K)

test-16: clean debug
	@/usr/bin/time -p ./TUP instances/umps16.txt $(Q1) $(Q2) $(K)

test-18: clean debug
	@/usr/bin/time -p ./TUP instances/umps18.txt $(Q1) $(Q2) $(K)

test-20: clean debug
	@/usr/bin/time -p ./TUP instances/umps20.txt $(Q1) $(Q2) $(K)

test-30: clean debug
	@/usr/bin/time -p ./TUP instances/umps30.txt $(Q1) $(Q2) $(K)

run-4:
	@/usr/bin/time -p ./TUP instances/umps4.txt $(Q1) $(Q2) $(K)

run-6:
	@/usr/bin/time -p ./TUP instances/umps6.txt $(Q1) $(Q2) $(K)

run-8:
	@/usr/bin/time -p ./TUP instances/umps8.txt $(Q1) $(Q2) $(K)

run-10:
	@/usr/bin/time -p ./TUP instances/umps10.txt $(Q1) $(Q2) $(K)

run-12:
	@/usr/bin/time -p ./TUP instances/umps12.txt $(Q1) $(Q2) $(K)

run-14:
	@/usr/bin/time -p ./TUP instances/umps14.txt $(Q1) $(Q2) $(K)

run-16:
	@/usr/bin/time -p ./TUP instances/umps16.txt $(Q1) $(Q2) $(K)

run-18:
	@/usr/bin/time -p ./TUP instances/umps18.txt $(Q1) $(Q2) $(K)

run-20:
	@/usr/bin/time -p ./TUP instances/umps20.txt $(Q1) $(Q2) $(K)

run-30:
	@/usr/bin/time -p ./TUP instances/umps30.txt $(Q1) $(Q2) $(K)

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
