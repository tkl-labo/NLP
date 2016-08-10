txt2tagseq_h_files := common.h
txt2tagseq_cpp_files := txt2tagseq.cpp common.cpp
tagseq2tagseq_count_h_files := common.h
tagseq2tagseq_count_cpp_files := tagseq2tagseq_count.cpp common.cpp
txt2tagword_count_h_files := common.h
txt2tagword_count_cpp_files := txt2tagword_count.cpp common.cpp
viterbi_h_files := common.h
viterbi_cpp_files := viterbi.cpp common.cpp
forward_h_files := common.h
forward_cpp_files := forward.cpp common.cpp
train_h_files := common.h
train_cpp_files := train.cpp common.cpp
tagger_h_files := common.h
tagger_cpp_files := tagger.cpp common.cpp
opts :=
opts += -static-libstdc++
opts += -O3
opts += -std=c++11
debug := -g

test_opts := -lpthread
test_opts += -Wall

CXXFLAGS := $(opts)

.PHONY: all
all : txt2tagseq tagseq2tagseq_count txt2tagword_count viterbi forward train tagger

.PHONY: txt2tagseq
txt2tagseq : $(txt2tagseq_cpp_files) $(txt2tagseq_h_files)
    /usr/local/bin/g++-4.8.2 -o $@ $(CXXFLAGS) $(txt2tagseq_cpp_files)

.PHONY: tagseq2tagseq_count
tagseq2tagseq_count : $(tagseq2tagseq_count_cpp_files) $(tagseq2tagseq_count_h_files)
    /usr/local/bin/g++-4.8.2 -o $@ $(CXXFLAGS) $(tagseq2tagseq_count_cpp_files)

.PHONY: txt2tagword_count
txt2tagword_count : $(txt2tagword_count_cpp_files) $(txt2tagword_count_h_files)
    /usr/local/bin/g++-4.8.2 -o $@ $(CXXFLAGS) $(txt2tagword_count_cpp_files)

.PHONY: viterbi
viterbi : $(viterbi_cpp_files) $(viterbi_h_files)
    /usr/local/bin/g++-4.8.2 -o $@ $(CXXFLAGS) $(viterbi_cpp_files)

.PHONY: forward
forward : $(forward_cpp_files) $(forward_h_files)
    /usr/local/bin/g++-4.8.2 -o $@ $(CXXFLAGS) $(forward_cpp_files)

.PHONY: train
train : $(train_cpp_files) $(train_h_files)
    /usr/local/bin/g++-4.8.2 -o $@ $(CXXFLAGS) $(train_cpp_files)

.PHONY: tagger
tagger : $(tagger_cpp_files) $(tagger_h_files)
    /usr/local/bin/g++-4.8.2 -o $@ $(CXXFLAGS) $(tagger_cpp_files)

.PHONY: clean
clean :
	rm -f txt2tagseq
	rm -f tagseq2tagseq_count
	rm -f txt2tagword_count
	rm -f viterbi
	rm -f forward
	rm -f train
	rm -f tagger
	rm -rf *.dSYM
