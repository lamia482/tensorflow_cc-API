clear
ulimit -c unlimited
rm -f core
make clean
make all -j32
echo ------------ COMPILE DONE AND SUCCESSFUL --------------
