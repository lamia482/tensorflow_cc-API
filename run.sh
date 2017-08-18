ulimit -c unlimited
clear
export LD_LIBRARY_PATH=./submit/lib:$LD_LIBRARY_PATH
./bin/gen
