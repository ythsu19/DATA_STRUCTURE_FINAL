# compile 
g++ benchmark.cpp -o benchmark -O3

# profile
valgrind --tool=cachegrind ./benchmark binary
valgrind --tool=cachegrind ./benchmark pairing