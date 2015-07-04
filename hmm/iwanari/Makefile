h_files  := $(wildcard *.h)
cc_files := $(wildcard *.cc)
opts := 
opts += -O3
opts += -std=c++11
debug := -g

GTESTDIR = ./../../tools/googletest
TEST_LIBRARY_DIR = ./lib/gen
TEST_INCLUDE_DIR = $(GTESTDIR)/include

TEST_STATIC_LIBRARY := $(TEST_LIBRARY_DIR)/libgtest.a

test_cc_files := $(filter-out main.cc, $(cc_files))
test_cc_files += $(wildcard unittest/*.cc)
test_opts := -lpthread
test_opts += -Wall

CXXFLAGS := $(opts)

main : $(cc_files) $(h_files)
	g++ -o $@ $(CXXFLAGS) $(cc_files)

.PHONY: run
run:
	./main

test: $(test_cc_files) $(h_files) $(TEST_STATIC_LIBRARY)
	g++ -o $@ -I$(TEST_INCLUDE_DIR) $(CXXFLAGS) $(test_opts) $(test_cc_files) $(TEST_STATIC_LIBRARY)
	./$@

$(TEST_STATIC_LIBRARY):
	cd ..; git submodule init; git submodule update
	mkdir -p $(TEST_LIBRARY_DIR)
	g++ -c -I$(TEST_INCLUDE_DIR) -I$(GTESTDIR) $(GTESTDIR)/src/gtest-all.cc -o $(GTESTDIR)/gtest-all.o
	ar -rv $(TEST_STATIC_LIBRARY) $(GTESTDIR)/gtest-all.o
	rm $(GTESTDIR)/gtest-all.o

debug: $(cc_files) $(h_files)
	g++ $(debug) -o $@ $(CXXFLAGS) $(cc_files)
	gdb ./$@
	
.PHONY: clean
clean :
	rm -f main
	rm -f test
	rm -f debug
	rm -rf *.dSYM
