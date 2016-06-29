h_files  := $(wildcard *.h)
cpp_files := $(wildcard *.cpp)
opts :=
opts += -O3
opts += -std=c++11
debug := -g

test_opts := -lpthread
test_opts += -Wall

CXXFLAGS := $(opts)

main : $(cpp_files) $(h_files)
	g++ -o $@ $(CXXFLAGS) $(cpp_files)

.PHONY: run
run:
	./main

debug: $(cpp_files) $(h_files)
	g++ $(debug) -o $@ $(CXXFLAGS) $(cpp_files)
	gdb ./$@

.PHONY: clean
clean :
	rm -f main
	rm -f test
	rm -f debug
	rm -rf *.dSYM
