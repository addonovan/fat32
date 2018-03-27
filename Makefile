PRODUCT := mfs
BIN  	:= bin
INC 	:= include
SRC	:= src
TEST 	:= test
OBJ 	:= obj

CC	:= gcc
CXX	:= g++
LINKER	:= gcc 

CXXFLAGS := -Wall -Wextra -pedantic -Werror -g3 -std=c++11
CFLAGS 	 := -Wall -Wextra -pedantic -Werror -g
LFLAGS 	 :=

INCLUDE  := -I$(INC) 

MAIN      := $(SRC)/$(PRODUCT).c
TEST_MAIN := $(TEST)/main.cpp

TEST_SRCS := $(wildcard $(TEST)/*.cpp)
SRCS      := $(wildcard $(SRC)/*.c)

TEST_SRCS := $(filter-out $(TEST_MAIN), $(TEST_SRCS))
SRCS      := $(filter-out $(MAIN), $(SRCS))

OBJS      := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))

#
# Test Builds
#

$(BIN)/tests: $(OBJ)/test.o makedirs
	$(CXX) $(CXXFLAGS) $(LFLAGS) $(INCLUDE) $< $(TEST_SRCS) -o $@

$(OBJ)/test.o: $(TEST)/main.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

#
# Full builds
#

run: build
	./$(BIN)/$(PRODUCT)
.PHONY: run

build: makedirs $(BIN)/$(PRODUCT)
.PHONY: build 

$(BIN)/$(PRODUCT): $(OBJS)
	$(LINKER) $(LFLAGS) $(INCLUDE) $^ src/main.cpp -o $@

$(OBJ)/%.o: $(SRCS)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

#
# Miscellaneous
#

makedirs:
	@mkdir -p $(BIN) $(OBJ)
.PHONY: makedirs

clean:
	@rm -rf $(BIN) $(OBJ)
	@mkdir -p $(BIN) $(OBJ)
.PHONY: clean

