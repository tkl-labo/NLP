cdr2sentence_h_files  := common.h
cdr2sentence_cpp_files := cdr2sentence.cpp common.cpp
sentence2ngram_h_files  := common.h
sentence2ngram_cpp_files := sentence2ngram.cpp common.cpp
perplexity_h_files  := common.h
perplexity_cpp_files := perplexity.cpp common.cpp
opts :=
opts += -O3
opts += -std=c++11
debug := -g

test_opts := -lpthread
test_opts += -Wall

CXXFLAGS := $(opts)

.PHONY: all
all : cdr2sentence sentence2ngram perplexity

.PHONY: cdr2sentence
cdr2sentence : $(cdr2sentence_cpp_files) $(cdr2sentence_h_files)
	g++ -o $@ $(CXXFLAGS) $(cdr2sentence_cpp_files)

.PHONY: sentence2ngram
sentence2ngram : $(sentence2ngram_cpp_files) $(sentence2ngram_h_files)
	g++ -o $@ $(CXXFLAGS) $(sentence2ngram_cpp_files)

.PHONY: perplexity
perplexity : $(perplexity_cpp_files) $(perplexity_h_files)
	g++ -o $@ $(CXXFLAGS) $(perplexity_cpp_files)


debug: $(main_cpp_files) $(main_h_files)
	g++ $(debug) -o $@ $(CXXFLAGS) $(main_cpp_files)
	gdb ./$@


.PHONY: clean
clean :
	rm -f cdr2sentence
	rm -f sentence2ngram
	rm -f perplexity
	rm -f debug
	rm -rf *.dSYM
