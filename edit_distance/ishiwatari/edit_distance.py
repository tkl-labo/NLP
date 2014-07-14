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
	print(w1,w2)

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

	print(distance)
	return distance[ len(w1) ][ len(w2) ]

if __name__ == '__main__':

	if len(argv) != 3:
		print('Usage: python3 edit_distance.py <word1> <word2>')
		raise SystemExit(0)
	
	res = compute_dist(argv[1], argv[2])
	print('MIN_DISTANCE: ',res)
