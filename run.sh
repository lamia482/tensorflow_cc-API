ulimit -c unlimited
clear
export LD_LIBRARY_PATH=./lib:./generate:$LD_LIBRARY_PATH
./bin/gen
