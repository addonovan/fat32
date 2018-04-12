PRODUCT     := mfs
BIN         := bin
INC         := include
SRC         := src
TEST        := test
OBJ         := obj

CC          := gcc
CFLAGS      := -Wall -Wextra -Werror -Warray-bounds -g -O0

LINKER      := gcc
LFLAGS      := 

INCLUDE     := -I$(INC)

EXT         := c
MAIN        := $(SRC)/main.$(EXT)
TEST_FMT    := test_%

SRCS        := $(wildcard $(SRC)/*.$(EXT))
SRCS        := $(filter-out $(MAIN), $(SRCS))
TEST_SRCS   := $(wildcard $(TEST)/*.$(EXT))

OBJS        := $(patsubst $(SRC)/%.$(EXT), $(OBJ)/%.o, $(SRCS))
MAIN_OBJ    := $(patsubst $(SRC)/%.$(EXT), $(OBJ)/%.o, $(MAIN))
TEST_OBJS   := $(patsubst $(TEST)/%.$(EXT), $(OBJ)/$(TEST_FMT).o, $(TEST_SRCS))

TEST_BINS   := $(patsubst $(TEST)/%.$(EXT), $(BIN)/$(TEST_FMT), $(TEST_SRCS))


################################################################################
# EXECUTABLES                                                                  #
################################################################################

test: CFLAGS+=-DTEST
test: $(TEST_BINS) $(OBJS) $(TEST_OBJS)
	@for file in $(TEST_BINS); do\
		./$$file;\
		if [ "$$?" != "0" ]; then\
			echo "";\
		fi;\
	done
.PHONY: test

run: build
	./$(BIN)/$(PRODUCT)
.PHONY: run


################################################################################
# BUILDING                                                                     #
################################################################################

test_build: makedirs $(OBJS) $(TEST_BINS)
.PHONY: test_build

build: makedirs $(BIN)/$(PRODUCT)
.PHONY: build 

$(BIN)/$(PRODUCT): $(OBJS) $(MAIN_OBJ)
	$(LINKER) $(LFLAGS) $(OBJS) $(MAIN_OBJ) -o $@

$(MAIN_OBJ): $(MAIN)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(OBJ)/%.o: $(SRC)/%.$(EXT)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

# Tests

$(OBJ)/$(TEST_FMT).o: $(TEST)/%.$(EXT)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(BIN)/$(TEST_FMT): $(OBJ)/$(TEST_FMT).o $(OBJS)
	$(LINKER) $(LFLAGS) $(CFLAGS) $(OBJS) $< -o $@


################################################################################
# Miscellaneous                                                                #
################################################################################

makedirs:
	@mkdir -p $(BIN) $(OBJ)
.PHONY: makedirs

clean:
	@rm -rf $(BIN) $(OBJ)
	@mkdir -p $(BIN) $(OBJ)
.PHONY: clean
