PRODUCT		:= mfs
BIN		:= bin
INC		:= include
SRC		:= src
TEST		:= test
OBJ		:= obj

CXX		:= g++
CXXFLAGS	:= -Wall -Wextra -pedantic -Werror -g

LINKER		:= g++
LFLAGS		:= 

INCLUDE		:= -I$(INC)

EXT		:= cpp

SRCS		:= $(wildcard $(SRC)/*.$(EXT))
OBJS		:= $(patsubst $(SRC)/%.$(EXT), $(OBJ)/%.o, $(SRCS))



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

$(OBJ)/%.o: $(SRC)/%.$(EXT)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@



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

