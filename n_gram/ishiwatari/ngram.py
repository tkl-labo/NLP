# get the number of occurances of each n-gram
def count_ngram(filename,n):

	f = open(filename)
	count_ngram = {}
	count_n_1gram = {}

	# get each string
	while 1:

		try:
			s = get_str(f)
			if s == 'EOF':
				break

			# make n-gram
			s = append_BOS_EOS(s,n)
			# print(s)
			length = len(s)

			# count ngram and n-1gram
			for i in range(length-n+1):
				ngram = ' '.join( s[i:i+n] )
				if ngram in count_ngram:
					count_ngram[ngram] += 1
				else : 
					count_ngram[ngram] = 1

				n_1gram = ' '.join( s[i+1:i+n] )
				# print(n_1gram)
				# if n_1gram != ' '.join(['EOS']* (n-1) ): # Last one must be something like 'EOS EOS', so ignore it.
				if n_1gram in count_n_1gram:
					count_n_1gram[n_1gram] += 1
				else : 
					count_n_1gram[n_1gram] = 1

		except:
			print("EXCEPTION:",s)

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
	return ['BOS']*(n-1) + str_list + ['EOS']*(n-1)

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


# compute the prob of a sentence.(EOS~EOS)
def sentence_prob(setence, n, count_ngram, count_n_1gram):

	setence_BE = append_BOS_EOS(setence, n)
	p = 1.0
	for i in range(len(setence) + n -1):
		ngram = setence_BE[i: i+n]
		ngram_probability = ngram_prob(ngram, count_ngram, count_n_1gram)
		p *= ngram_probability

		print('p(', ' '.join(ngram), ') : ', ngram_probability)


	return p
		 

# compute the perplexity of the test data.
def perplexity(filename, n, count_ngram, count_n_1gram):
	f = open(filename)

	# get each string
	while 1:
		s = get_str(f)
		if s == 'EOF':
			break
		p = sentence_prob(s, n, count_ngram, count_n_1gram)
		print('p(',' '.join(s), ') : ',p)
		print('------------------------------------------------')


def main():

	n = 3

	count_3gram, count_2gram = count_ngram('/disk5/ishiwatari/nlp-meeting/data/debug_data.txt',n)
	# count_3gram, count_2gram = count_ngram('data/mini2012.txt',n)
	# count_3gram, count_2gram = count_ngram('/disk5/ishiwatari/nlp-meeting/data/error_test.txt',n)
	# perplexity('/disk5/ishiwatari/nlp-meeting/data/mini2013.txt', n, count_3gram, count_2gram)


if __name__ == '__main__':
	main()