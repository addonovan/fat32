PRODUCT := mfs
BIN  	:= bin
INC 	:= include
SRC	:= src
TEST 	:= test
OBJ 	:= obj

CC	:= gcc
LINKER 	:= gcc
CFLAGS 	:= -Wall -Wextra -pedantic -Werror -g
LFLAGS 	:= 

INCLUDE  := -I$(INC)/

SRCS      := $(wildcard $(SRC)/*.c)
OBJS      := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))



################################################################################
# EXECUTABLES                                                                  #
################################################################################

run: build
	./$(BIN)/$(PRODUCT)
.PHONY: run



################################################################################
# BUILDING                                                                     #
################################################################################

build: makedirs $(BIN)/$(PRODUCT)
.PHONY: build 

$(BIN)/$(PRODUCT): $(OBJS)
	$(LINKER) $(LFLAGS) $(OBJS) -o $@

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@



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

