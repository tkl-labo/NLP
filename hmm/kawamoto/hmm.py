#!/usr/bin/env python
# -*- coding: utf-8 -*-
from sys import *
from collections import Counter
from collections import defaultdict
import gzip
import math

type_dict = {}
word_Counter = Counter()
type_Counter = Counter()
word_prob = defaultdict(int)

trans_Counter = Counter()
trans_prob = defaultdict(int)
def Count_trans( from_type, to_type):
	trans_Counter[tuple([from_type,to_type])] += 1

def Count_word( word, word_type ):
	if word_Counter[word] == 0:
		type_dict [word] = word_type
	word_Counter[word] += 1
	type_Counter[word_type] += 1
#	print (word)


if __name__ == "__main__":
	if len(argv) != 2:
		print("Usage: # python %s \"SOURCE_FILE\" " % argv[0], file=stderr)
		quit()
	learning_file = argv[1]
	prev_type = b"<s>"
	for rawline in open(learning_file,'rb'):
		line = rawline.rstrip()
		if line == b"":
			word_type = b"</s>"
			Count_word(b"</s>",b"</s>")
			Count_word(b"<s>",b"<s>")
		else:
			word = line.split(b" ")[0]
			word_type = line.split(b" ")[1]
			Count_word(word,word_type)
		Count_trans(prev_type,word_type)
		if word_type == b"</s>":
			prev_type = b"<s>"
		else:
			prev_type = word_type

	for k,v in sorted(word_Counter.items()):
		word_prob[k] = v / type_Counter[type_dict[k]]
#		print (k,v)
#	print ( word_prob )
#	print (len(word_prob))
#	print (len(word_Counter))
#	print (trans_Counter)
	for k,v in sorted(trans_Counter.items()):
#		print(k)
		trans_prob[k] = v / type_Counter[k[0]]
#	print (trans_prob)
	for k,v in sorted(trans_prob.items()):
		print (k[0].decode('utf-8'),k[1].decode('utf-8'),v)

