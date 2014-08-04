import sys
import pickle
import collections

def train():
	"""
	# read train data from stdin and make A and B
	# return A['prev_POS']['next_POS'], B['POS']['word']
	"""
	A = collections.defaultdict(dict)
	B = collections.defaultdict(dict)
	pos_list = []
	word_list = []
	str_buf = []


	# read each line and count A and B
	for line in sys.stdin:
		line = line.split()
		# print(line)
		if len(line) == 3:
			str_buf.append((str(line[0]), str(line[1])))
			word_list.append(str(line[0]))
			pos_list.append(str(line[1]))

		else:
			# if come to the end of a sentence
			if len(str_buf) != 0:
				str_buf = [('<s>','BOS')] + str_buf + [('</s>', 'EOS')]
				word_list += ['<s>', '</s>']
				pos_list += ['BOS', 'EOS']

				for i, s in enumerate(str_buf):
					if s[0] in B[s[1]]:
						B[s[1]][s[0]] += 1
					else:
						B[s[1]][s[0]] = 1

					if s[0] != '</s>':
						# print('strbuf[i]:',str_buf[i], 's[1]', s[1])
						if str_buf[i+1][1] in A[s[1]]:
							A[s[1]][str_buf[i+1][1]] += 1
						else:
							A[s[1]][str_buf[i+1][1]] = 1

				str_buf = []

	# get unique POS list and word list
	pos_list_uniq = list(set(pos_list))
	word_list_uniq = list(set(word_list))


	# assume <UNK>, smoothing, normalize
	B_sm = collections.defaultdict(dict)
	A_sm = A.copy()

	# assume words apeear less than 2 times as <UNK>
	word_count = collections.Counter(word_list)
	for pos in B:
		for word in B[pos]:
			if word_count[word] > 1:
				B_sm[pos][word] = B[pos][word]

			else: # add <UNK> to B_sm
				word_list_uniq.remove(word)
				if '<UNK>' in B_sm[pos]:
					B_sm[pos]['<UNK>'] += 1
				else:
					B_sm[pos]['<UNK>'] = 1

	word_list_uniq += ['<UNK>']


	# add 1 smoothing
	for pos in pos_list_uniq:
		for word in word_list_uniq:
			if word in B_sm[pos]:
				B_sm[pos][word] += 1
			else:
				B_sm[pos][word] = 1

	for prev in pos_list_uniq:
		for next in pos_list_uniq:
			if next in A_sm[prev]:
				A_sm[prev][next] += 1
			else:
				A_sm[prev][next] = 1

	# delete instances like A[VB][BOS], A[EOS][VB],
	# B[VB]['</s>'], B[EOS]['Jack']

	for pos in B_sm:
		for word in B_sm[pos]:
			if (pos == 'BOS' and word != '<s>') or \
			(pos == 'EOS' and word != '</s>') or \
			(word == '<s>' and pos != 'BOS') or \
			(word == '</s>' and pos != 'EOS'):
				B_sm[pos][word] = 0

	for prev in A_sm:
		for next in A_sm[prev]:
			if prev == 'EOS' or next == 'BOS':
				A_sm[prev][next] = 0


	# normalize
	for pos in B_sm:
		s = sum(B_sm[pos].values())
		for word in B_sm[pos]:
			if B_sm[pos][word] != 0:
				B_sm[pos][word] /= s

	for prev in A_sm:
		s = sum(A_sm[prev].values())
		for next in A_sm[prev]:
			if A_sm[prev][next] != 0:
				A_sm[prev][next] /= s

	return A_sm, B_sm



if __name__ == '__main__':
	A, B = train()
	# for prev in A:
	# 	for next in A[prev]:
	# 		if A[prev][next] != 0:
	# 			print(prev, next, A[prev][next])

	for pos in B:
		for word in B[pos]:
			if B[pos][word] != 0:
				print(pos, word, B[pos][word])