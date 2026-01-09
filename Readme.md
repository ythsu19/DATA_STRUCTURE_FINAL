# compile
make
# only for benchmark csv
make run
# all automation
make experiment

# trying profile
valgrind --tool=cachegrind ./benchmark binary
valgrind --tool=cachegrind ./benchmark pairing