from ngram import *
from collections import Counter
import os

TRAIN_DIR = '/disk5/ishiwatari/nlp-meeting/data/2012_01/'

# get N
if len(sys.argv) != 2:
	N = get_N()
else:
	N = int(sys.argv[1])
print('Train with '+ str(N) + '-gram.')


# get a file list (*.cdr) in TRAIN_DIR
files = []
for f in sorted( os.listdir(TRAIN_DIR)):
	if f.endswith('.cdr'):
		files.append(TRAIN_DIR + f)
print('files:', files)

# make ngram dict.
ngrams = {}
n_1grams = {}
for i in range(len(files)):
	try:
		if files[i].endswith('.cdr'):
			ngrams, n_1grams = count_ngram_large_dict(files[i], N, ngrams, n_1grams)
	except:
		print("EXCEPTION (learn_1month):make ngram",files[i]) 


# save n-grams to pickle files.
filename_n = TRAIN_DIR + 'count/' +str(N) + 'gram.pic2'
filename_n_1 = TRAIN_DIR + 'count/' +str(N-1) + 'gram.pic2'
save(ngrams, filename_n)
save(n_1grams, filename_n_1)