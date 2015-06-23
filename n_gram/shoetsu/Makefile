h_files  := $(wildcard ./src/*.h)
c_files := $(wildcard ./src/*.c)
cpp_files := $(wildcard ./src/*.cpp)
opts := 
opts += -g -fopenmp -std=c++14 -O3 
//opts += -funroll-loops
//opts +=  -mmmx -msse -msse2 -msse4.1 -msse4.2 -mavx 
C := gcc
CXX := g++-5

CFLAGS := $(opts)
CXXFLAGS := $(opts)
a.out : clean
	$(CXX) -o $@ $(CXXFLAGS) $(cpp_files)  $(h_files)

clean : 
	rm -f a.out

