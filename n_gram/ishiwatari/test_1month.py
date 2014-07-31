from ngram import *
from collections import Counter
import os
import sys

TRAIN_DIR = '/disk5/ishiwatari/nlp-meeting/data/2012_01/'
TEST_DIR = '/disk5/ishiwatari/nlp-meeting/data/2013_01/'

# get N
if len(sys.argv) != 2:
	N = get_N()
else:
	N = int(sys.argv[1])
print('Compute perplexity with '+ str(N) + '-gram.')

# load train data
filename_n = TRAIN_DIR + '/count/' +str(N) + 'gram.pic'
filename_n_1 = TRAIN_DIR + '/count/' +str(N-1) + 'gram.pic'
count_ngram = load(filename_n)
count_n_1gram = load(filename_n_1)


# get a file list (*.cdr) in TEST_DIR
test_files = []
for f in sorted( os.listdir(TEST_DIR)):
	if f.endswith('.cdr'):
		test_files.append(TEST_DIR + f)
print('test_files:', test_files)

# compute perplexity with Laplace smoothing.
for f in test_files:
	PP = perplexity_Laplace( f, N, count_ngram, count_n_1gram)
	print(f,': Perplexity =', PP)