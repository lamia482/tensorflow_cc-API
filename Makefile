BIN_DIR = ./bin/
EXE = $(BIN_DIR)gen
SRC_DIR = ./src/
EXE_SRC = $(SRC_DIR)main.cc

C_COMPILER = gcc
CC_COMPILER = g++

INCLUDE_PATH = -I./include \
               -I$(SRC_DIR) \
               `pkg-config --cflags opencv`

CFLAGS = -g -std=c++11 -DLinux
OPTS = -fPIC -shared

DYNAMIC = make_so
DYNAMIC_LIB = ./lib/libtfloader.so
STATIC = make_a
STATIC_LIB = ./lib/libtfloader.a

LD_LIBRARY_PATH = -L./lib

LD_FLAGS = -ltensorflow_cc -lcudnn -lglog \
           `pkg-config --libs opencv` \
            -lm -lpthread

OBJ = tensorflow_loader.o tf_api.o laMiaSocket.o
OBJS_DIR = ./obj/
OBJS = $(addprefix $(OBJS_DIR), $(OBJ))

default: $(EXE)

all: $(DYNAMIC) $(STATIC) $(EXE)
	@echo '---------------- DONE FOR ALL ---------------'

$(EXE): make_dir $(OBJS)
	$(CC_COMPILER) $(EXE_SRC) -o $(EXE) $(INCLUDE_PATH) $(LD_LIBRARY_PATH) $(CFLAGS) $(OBJS) $(LD_FLAGS)

$(OBJS_DIR)%.o: $(SRC_DIR)%.cc
	$(CC_COMPILER) $(INCLUDE_PATH) $(LD_LIBRARY_PATH) $(CFLAGS) $(OPTS) -c $< -o $@

$(DYNAMIC): $(OBJS)
	$(CC_COMPILER) $(INCLUDE_PATH) $(LD_LIBRARY_PATH) $(CFLAGS) $(OPTS) $(OBJS) -o $(DYNAMIC_LIB)

$(STATIC):	$(OBJS)
	$(AR) rsv $(STATIC_LIB) $(OBJS)

make_dir:
	mkdir -p $(BIN_DIR) $(OBJS_DIR)

clean:
	rm -rf $(OBJS) $(EXE) $(DYNAMIC_LIB) $(STATIC_LIB) $(BIN_DIR) $(OBJ_DIR)
