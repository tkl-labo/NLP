h_files  := $(wildcard ./src/*.h)
c_files := $(wildcard ./src/*.c)
cpp_files := $(wildcard ./src/*.cpp)
opts := 
opts += -fopenmp -std=c++14 -O2 -DDETAIL
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
	rm -f test
	rm -f debug
	rm -rf *.dSYM


//=====================================

GTESTDIR = ./../../../tools/googletest
TEST_LIBRARY_DIR = ../lib/gen
TEST_INCLUDE_DIR = $(GTESTDIR)/include
TEST_STATIC_LIBRARY := $(TEST_LIBRARY_DIR)/libgtest.a
test_cpp_files := $(filter-out main.cpp, $(cpp_files))
test_cpp_files += $(wildcard unittest/*.cpp)
test_opts := -lpthread
test_opts += -Wall

test: $(test_cpp_files) $(h_files) $(TEST_STATIC_LIBRARY)
	g++ -o $@ -I$(TEST_INCLUDE_DIR) $(CXXFLAGS) $(test_opts) $(test_cpp_files) $(TEST_STATIC_LIBRARY)
	./$@

$(TEST_STATIC_LIBRARY):
	cd ..; git submodule init; git submodule update
	mkdir -p $(TEST_LIBRARY_DIR)
	g++ -c -I$(TEST_INCLUDE_DIR) -I$(GTESTDIR) $(GTESTDIR)/src/gtest-all.cpp -o $(GTESTDIR)/gtest-all.o
	ar -rv $(TEST_STATIC_LIBRARY) $(GTESTDIR)/gtest-all.o
	rm $(GTESTDIR)/gtest-all.o


