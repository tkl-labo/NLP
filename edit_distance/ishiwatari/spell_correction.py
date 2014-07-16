from sys import *
import numpy as np

# change later.
def INS_COST(c):
	return 1

# change later.
def DEL_COST(c):
	return 1

# change later.
def SUBST_COST(c1, c2):
	if c1 == c2:
		return 0
	else:
		return 2

def compute_dist(w1, w2):
	# create a matrix whose size is word1 * word2
	distance = np.empty(( len(w1)+1, len(w2)+1 ))
	distance[0][0] = 0

	# first column
	for i in range( 1, len(w1)+1 ):
		distance[i][0] = distance[i-1][0] + INS_COST( w1[i-1] )

	# first row
	for j in range( 1, len(w2)+1 ):
		distance[0][j] = distance[0][j-1] + DEL_COST( w2[j-1] )

	# other cells
	for i in range(1,len(w1)+1):
		for j in range( 1, len(w2)+1 ):
			distance[i][j] = min(
				distance[i-1][j] + INS_COST( w1[i-1] ),
				distance[i-1][j-1] + SUBST_COST( w1[i-1], w2[j-1] ),
				distance[i][j-1] + DEL_COST( w2[j-1] )
				)

	print('distance between ', w1, ' and ',w2, ' : ', distance[ len(w1) ][ len(w2) ])
	return distance[ len(w1) ][ len(w2) ]

# read the vocabulary file.
def get_vocabulary(file):
	data = open(file).read().splitlines()
	# print(data)
	return data

# find similar words.
def get_similar_words(word, vocabulary):

	if word in vocabulary:
		return True

	else:
		candidates = []
		min_distance = 99999999999

		for w in vocabulary:
			distance = compute_dist(word, w)
			if distance == min_distance:
				candidates.append(w)

			elif distance < min_distance:
				candidates = [w]
				min_distance = distance

			# print(candidates, len(candidates) )

		return candidates


if __name__ == '__main__':

	if len(argv) != 2:
		print('Usage: python3 spell_correction.py <word>')
		raise SystemExit(0)
	
	vocabulary = get_vocabulary('data/vocab.nytimes.txt')
	candidates = get_similar_words(argv[1], vocabulary)

	if candidates == True:
		print('The word',argv[1],'exists in vocabulary.')
	else:
		print('Do you mean ', candidates,' ?')

