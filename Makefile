EXE = ./bin/gen
SRC_DIR = ./src/
EXE_SRC = $(SRC_DIR)main.cc

C_COMPILER = gcc
CC_COMPILER = g++

INCLUDE_PATH = 	-I./include \
								-I$(SRC_DIR) \
				`pkg-config --cflags opencv`

CFLAGS = -g -std=c++11
OPTS = -fPIC -shared

DYNAMIC = make_so
DYNAMIC_LIB = ./lib/libtfloader.so
STATIC = make_a
STATIC_LIB = ./lib/libtfloader.a

LD_LIBRARY_PATH = -L./lib

LD_FLAGS = 	-ltensorflow_cc -lcudnn -lglog \
			`pkg-config --libs opencv` \
			-lm -lpthread

OBJ = tf_api.o
OBJS_DIR = ./obj/
OBJS = $(addprefix $(OBJS_DIR), $(OBJ))

default: $(EXE)

all: $(EXE) $(DYNAMIC) $(STATIC)
	@echo '---------------- DONE FOR ALL ---------------'

$(EXE): $(OBJS)
	$(CC_COMPILER) $(EXE_SRC) -o $(EXE) $(INCLUDE_PATH) $(LD_LIBRARY_PATH) $(CFLAGS) $(LD_FLAGS) $(OBJS)

$(OBJS_DIR)%.o: $(SRC_DIR)%.cc
	$(CC_COMPILER) $(INCLUDE_PATH) $(LD_LIBRARY_PATH) $(CFLAGS) $(OPTS) -c $< -o $@

$(DYNAMIC): $(OBJS)
	$(CC_COMPILER) $(INCLUDE_PATH) $(LD_LIBRARY_PATH) $(CFLAGS) $(OPTS) $(OBJS) -o $(DYNAMIC_LIB)

$(STATIC):	$(OBJS)
	$(AR) rsv $(STATIC_LIB) $(OBJS)

clean:
	rm -f $(OBJS) $(EXE) $(DYNAMIC_LIB) $(STATIC_LIB)
