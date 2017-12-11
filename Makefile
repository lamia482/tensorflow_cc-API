BIN_DIR = ./bin/
EXE = $(BIN_DIR)gen
SRC_DIR = ./src/
EXE_SRC = $(SRC_DIR)main.cc $(SRC_DIR)laMiaSocket.cc
GEN_DIR = ./submit/
GEN_INC = $(GEN_DIR)include/
GEN_LIB = $(GEN_DIR)lib/
LIB_DIR = ./lib/

C_COMPILER = gcc
CC_COMPILER = g++

CFLAGS = -g -std=c++11 -DLinux -fpermissive
OPTS = -fPIC -shared

DYNAMIC = make_so
DYNAMIC_LIB = $(GEN_LIB)libtfloader.so
STATIC = make_a
STATIC_LIB = $(GEN_LIB)libtfloader.a

INCLUDE_PATH = -I./include \
               -I$(SRC_DIR) \
               `pkg-config --cflags opencv`

LD_LIBRARY_PATH = -L$(LIB_DIR) 

LD_FLAGS = -ltensorflow_cc -lcudnn \
           `pkg-config --libs opencv` \
           -lglog -lm -lpthread

OBJ = tensorflow_loader.o tf_api.o read_options.o
OBJS_DIR = ./obj/
OBJS = $(addprefix $(OBJS_DIR), $(OBJ))

default: $(EXE)
	@echo '---------------- DONE FOR DEFAULT ---------------'

all: $(EXE) copy_deps
	@echo '---------------- DONE FOR ALL ---------------'

$(EXE): make_dir $(OBJS) 
	$(CC_COMPILER) $(EXE_SRC) -o $(EXE) -I$(INCLUDE_PATH) $(CFLAGS) -L$(LIB_DIR) $(OBJS) $(LD_FLAGS)

install: copy_deps
	@echo '---------------- DONE FOR INSTALL ----------------'

$(OBJS_DIR)%.o: $(SRC_DIR)%.cc
	$(CC_COMPILER) $(INCLUDE_PATH) $(LD_LIBRARY_PATH) $(CFLAGS) $(OPTS) -c $< -o $@

$(DYNAMIC): $(OBJS)
	$(CC_COMPILER) $(INCLUDE_PATH) $(LD_LIBRARY_PATH) $(CFLAGS) $(OPTS) $(OBJS) -o $(DYNAMIC_LIB)

$(STATIC): $(OBJS)
	$(AR) rsv $(STATIC_LIB) $(OBJS)
	
copy_deps: $(DYNAMIC) $(STATIC)
	cp $(SRC_DIR)tensorflow_loader.h $(GEN_INC)
	cp $(LIB_DIR)libtensorflow_cc.so $(GEN_LIB)
	cp $(LIB_DIR)libcudnn.so $(GEN_LIB)

run:
	bash ./run.sh

make_dir:
	mkdir -p $(BIN_DIR) $(OBJS_DIR) $(GEN_INC) $(GEN_LIB)

clean:
	rm -rf $(BIN_DIR) $(OBJS_DIR) $(GEN_DIR) 
