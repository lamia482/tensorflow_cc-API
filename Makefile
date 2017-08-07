EXE = gen
EXE_SRC = main.cc

C_COMPILER = gcc
CC_COMPILER = g++

INCLUDE_PATH = 	-I./include

CFLAGS = -g -std=c++11

LD_LIBRARY_PATH = -L./lib

LD_FLAGS = -ltensorflow_cc -lcudnn -lglog

OBJ = tf_api.o
OBJS_DIR = ./
OBJS = $(addprefix $(OBJS_DIR), $(OBJ))



all: $(EXE)
	@echo '---------------- DONE FOR ALL ---------------'

$(EXE): $(OBJS)
	$(CC_COMPILER) $(EXE_SRC) -o $(EXE) $(INCLUDE_PATH) $(LD_LIBRARY_PATH) $(CFLAGS) $(LD_FLAGS) $(OBJS)
	rm -f $(OBJS)

$(OBJS_DIR)%.o: %.cc
	$(CC_COMPILER) $(INCLUDE_PATH) $(LD_LIBRARY_PATH) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXE)
