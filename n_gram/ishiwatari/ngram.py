import sys
import pickle
import os.path
from math import *
import math

# get the number of occurances of each n-gram
def count_ngram_large_dict(filename,n, count_ngram, count_n_1gram):

	f = open(filename)
	debug_counter= 0
	s = ''

	# get each string
	while 1:

		try:
			s = get_str(f)
			if s == 'EOF':
				break

			# make n-gram
			s = append_BOS_EOS(s,n)

			# count ngram and n-1gram
			for i in range(len(s) + 1 ):
				try:
					ngram = ' '.join( s[i:i+n] )
					if ngram in count_ngram:
						count_ngram[ngram] += 1
					else : 
						count_ngram[ngram] = 1
				except:
					print("EXCEPTION : count_ngram", filename ,i)

				try:
					n_1gram = ' '.join( s[i+1:i+n] )
					if n_1gram in count_n_1gram:
						count_n_1gram[n_1gram] += 1
					else : 
						count_n_1gram[n_1gram] = 1
				except:
					print("EXCEPTION : count_n_1gram", filename ,i)

		except:
			debug_counter += 1
			print("EXCEPTION (count_ngram): ", filename, debug_counter, s)

	# print(count_ngram, count_n_1gram)
	return count_ngram, count_n_1gram


# get the lists of strings from input file
def get_str(file):

	str = ''
	str_list = []

	while str != 'EOS' :
		str = file.readline().strip().split('\t')[0]
		
		if str!= '':
			str_list.append(str)

		else:
			return 'EOF'

	return str_list[:-1]

	if len(str_list) == 0:
		return 'EOF'

	else:
		return str_list[:-1]

# append BOS and EOS to a list of strings
def append_BOS_EOS(str_list,n):
	return ['<s>']*(n-1) + str_list + ['</s>']
	# return ['BOS']*(n-1) + str_list + ['EOS']

# e.g.) get ['I','am','a'] and return count(I am a) / count(I am)
def ngram_prob(str_list, count_ngram, count_n_1gram):

	ngram = ' '.join(str_list)
	n_1gram = ' '.join(str_list[1:])

	# check if the strings are exist in learned dictionary
	if ngram not in count_ngram:
		c_ngram = 0
		return 0
	else:
		c_ngram = count_ngram[ngram]

	if n_1gram not in count_n_1gram:
		c_n_1gram = 0
		return 0
	else:
		c_n_1gram = count_n_1gram[n_1gram]

	return c_ngram / c_n_1gram

# compute the prob of a sentence.
def sentence_prob(sentence, n, count_ngram, count_n_1gram):

	token_num = len(sentence) + 1 # Should consider </s>.
	sentence_BE = append_BOS_EOS(sentence, n)

	p = 1.0

	for i in range( token_num ):
		ngram = sentence_BE[i: i+n]
		ngram_probability = ngram_prob(ngram, count_ngram, count_n_1gram)
		p *= ngram_probability

		print('p(', ' '.join(ngram), ') : ', ngram_probability)


	return p

# compute perplexity by using testfile, train
def perplexity_Laplace( filename, n, count_ngram, count_n_1gram ):

	f = open(filename)
	debug_counter= 0
	s = ''
	logPP = 0
	sentence_count = 0


	# get each string
	while 1:

		try:
			s = get_str(f)
			if s == 'EOF':
				break
			length = len(s)

			# make n-gram
			s = append_BOS_EOS(s,n)

			# count ngram and n-1gram
			for i in range( length + 1 ):
				logp = 0
				ngram = ' '.join( s[i:i+n] )
				n_1gram = ' '.join( s[i+1:i+n] )

				if ngram in count_ngram:
					c_n = count_ngram[ngram]
				else : 
					c_n = 0

				if n_1gram in count_n_1gram:
					c_n_1 = count_n_1gram[n_1gram]
				else : 
					c_n_1 = 0

				# add one smoothing
				p = (c_n + 1)/ (c_n_1 + 1)

				# p(word)
				logp += log(p)


			# compute perplexity of the sentence
			pp = ( 1 / ( math.e ** logp ) ) ** ( 1 / length +1)


			# perplexity of whole test data
			logPP += log(pp)
			# print(s,'pp:', pp)

			sentence_count += 1


		except Exception as e:
			# print(e)
			debug_counter += 1

	# normalize by the number of sentences.
	print('logPP:',logPP)
	# PP =  ( 1/(math.e ** logPP) ) ** (1/ sentence_count)
	PP = math.e ** ( -1*logPP / sentence_count )
	return PP

# decide N (for N-grams) from user input.
def get_N():
	sys.stdout.write("Please input N (for N-gram).\n")
	sys.stdout.write(">> ")
	sys.stdout.flush()
	while 1:
		line = sys.stdin.readline()
		N = line.strip()
		if N.isdigit():
			break
		else:
			sys.stdout.write(">> ")
			sys.stdout.flush()

	return int(N)

# Save data using pickle.
def save(ngram, filename):
	f = open(filename, 'wb')
	pickle.dump(ngram, f)
	f.close()
	print('Output:', filename)
	return

# Load data from pickle.
def load(filename):
	f = open(filename, 'rb')
	ngram = pickle.load(f)
	f.close()
	print('Loaded:', filename)
	return ngram


if __name__ == '__main__':
	print('do nothing.')