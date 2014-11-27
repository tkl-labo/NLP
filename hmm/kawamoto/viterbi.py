#!/usr/bin/env python
# -*- coding: utf-8 -*-
from sys import *
from collections import Counter
from collections import defaultdict
import gzip
import math


class Hmm:
	def __init__(self):
		self.type_dict = {}
		self.word_Counter = Counter()
		self.word_type_Counter = Counter()
		self.type_Counter = Counter()
		self.word_prob = defaultdict(int)
		self.trans_Counter = Counter()
		self.trans_prob = defaultdict(int)
		self.type_list = []

	def training(self,learning_file):
		# learning
		prev_type = b"<s>"
		for rawline in open(learning_file,'rb'):
			line = rawline.rstrip()
			if line == b"":
				word_type = b"</s>"
				self.Count_word(b"</s>",b"</s>")
				self.Count_word(b"<s>",b"<s>")
			else:
				word = line.split(b" ")[0]
				word_type = line.split(b" ")[1]
				self.Count_word(word,word_type)
			self.Count_trans(prev_type,word_type)
			if word_type == b"</s>":
				prev_type = b"<s>"
			else:
				prev_type = word_type
		for k,v in sorted(self.type_Counter.items()):
			self.type_list.append(k)	
		# smoothing
		for k,v in sorted(self.word_Counter.items()):
			# 尤度1以下はUNK
			if v == 1:
				self.word_type_Counter[tuple([b"<UNK>",self.type_dict[k]])] += 1
				del self.word_type_Counter[tuple([k,self.type_dict[k]])]
				del self.word_Counter[k]
			# add 1 smoothing
			elif k != b"<s>" and k != b"</s>":
					for word_type in self.type_list:
						self.word_type_Counter[tuple([k,word_type])] += 1
		for k,v in self.word_type_Counter.items():
			if k[0] == b"<s>" or k[0] == b"</s>":
				self.word_prob[k] = 1
			else:
				self.word_prob[k] = ( v ) / (self.type_Counter[k[1]] + len(self.word_Counter) - 2 + 1)
		for k,v in sorted(self.trans_Counter.items()):
			self.trans_prob[k] = ( v ) / (self.type_Counter[k[0]] + len(self.type_Counter) - 2)
		print (self.type_list)
	#	print (self.word_type_Counter)
	def Count_trans(self, from_type, to_type):
		self.trans_Counter[tuple([from_type,to_type])] += 1
	def Count_word(self, word, word_type ):
		if self.word_Counter[word] == 0:
			self.type_dict[word] = word_type
		self.word_Counter[word] += 1
		self.type_Counter[word_type] += 1
		self.word_type_Counter[word,word_type] += 1

def forward ( hmm, test_file):
	alphalist = defaultdict(int)
	alphalist[b"<s>"] = 1
	prev_type = b"<s>"
	for rawline	in open(test_file,'rb'):
		line = rawline.rstrip()
#		print (rawline)
		if line == b"":
			word = b"</s>"
			word_type = b"</s>"
		else:
			word = line.split(b" ")[0]
			word_type = line.split(b" ")[1]
		#処理はここで
		if hmm.word_Counter[word] < 2:
			word = b"<UNK>"
		print (word)
		prev_alphalist = alphalist.copy()
		alphalist.clear()
#		print (prev_alphalist)
#		print ([i for i in type_list])
		for i in hmm.type_list:
#				alphalist[word_type]=[prev_alphalist[i] * trans_prob[tuple([i,word_type])] * word_prob[tuple([word_type,word])] for i in type_list ]
			alpha = 0
			for j in hmm.type_list:
#					print (i,j,trans_prob[tuple([j,i])],word_prob[tuple([word,i])])
				if prev_alphalist[j] != 0 and hmm.trans_prob[tuple([j,i])] != 0 and hmm.word_prob[tuple([word,i])] != 0:
#					print("hogehoge")
					alphalist[i]+=math.exp(math.log(prev_alphalist[j]) + math.log(hmm.trans_prob[tuple([j,i])]) + math.log(hmm.word_prob[tuple([word,i])]))
#			print (alphalist[i])
#				else:
#					print(line,word,i,j,hmm.word_prob[tuple([word,i])])
			
#		print(word_type,[alphalist[j] for j in type_list ])
#		break

		if word_type == b"</s>":
			print (alphalist[b"</s>"])
			alphalist.clear()
			alphalist[b"<s>"] += 1
			prev_type = b"<s>"
		else:
			prev_type = word_type


def viterbi ( hmm, test_file):
	alphalist = defaultdict(int)
	alphalist[b"<s>"] = 1
	prev_type = b"<s>"
	backpointer = {}
	sentence = []
	word_list = []
	word_type_list = []
	sentence_index = 0
	all_num = 0
	all_index = 0
	# backpointer[tuple(word , word_type)] = prev_word_type
	for rawline	in open(test_file,'rb'):
		line = rawline.rstrip()
#		print (rawline)
		if line == b"":
			word = b"</s>"
			word_type = b"</s>"
		else:
			word = line.split(b" ")[0]
			word_type = line.split(b" ")[1]
		word_list.append(word)
		word_type_list.append(word_type)
		#処理はここで
		if hmm.word_Counter[word] < 2:
			word = b"<UNK>"
#		print (word)
		sentence.append(word) 
		prev_alphalist = alphalist.copy()
		alphalist.clear()
#		print (prev_alphalist)
#		print ([i for i in type_list])
		for i in hmm.type_list:
#				alphalist[word_type]=[prev_alphalist[i] * trans_prob[tuple([i,word_type])] * word_prob[tuple([word_type,word])] for i in type_list ]
			alpha_candidate = 0
			backpointer_candidate = b"NULL"
			for j in hmm.type_list:
#					print (i,j,trans_prob[tuple([j,i])],word_prob[tuple([word,i])])
				if prev_alphalist[j] != 0 and hmm.trans_prob[tuple([j,i])] != 0 and hmm.word_prob[tuple([word,i])] != 0:
#					print("hogehoge")
					tmp = math.exp(math.log(prev_alphalist[j]) + math.log(hmm.trans_prob[tuple([j,i])]) + math.log(hmm.word_prob[tuple([word,i])]))
					if alpha_candidate	< tmp:
						alpha_candidate = tmp
						backpointer_candidate = j
			alphalist[i] = alpha_candidate
			backpointer[tuple([sentence_index,i])] = backpointer_candidate
			if word == b"</s>" and backpointer_candidate != b"NULL":
				last_backpointer = backpointer_candidate
#			print (alphalist[i])			
#		print(word_type,[alphalist[j] for j in type_list ])
#		break
		if word_type == b"</s>":
#			print (alphalist[b"</s>"])
			index = len(sentence) - 1
			num = 0
			for v in reversed(sentence):
				if v == b"</s>":
#					print ("ESTIMATION :",v, b"</s>", "TRUE :" ,word_list[index],word_type_list[index])
					prev_backpointer = last_backpointer
				else:
#					print ("ESTIMATION :",v, prev_backpointer, "TRUE :" ,word_list[index],word_type_list[index])
#					for k0,v0 in backpointer.items():
#						if k0[0]==b"," or k0[0]==b"<UNK>":
#							print (k0,v0)
					if prev_backpointer == word_type_list[index]:
						num = num + 1
					else:
						print (v,word_list[index],prev_backpointer,word_type_list[index])
					prev_backpointer = backpointer[index,prev_backpointer]
				index = index -1
#			print (alphalist[b"</s>"], "ACCURACY_RATE :", num/(len(sentence)-1))
			all_num += num
			all_index += len(sentence)-1
			alphalist.clear()
			sentence.clear()
			word_list.clear()
			word_type_list.clear()
			alphalist[b"<s>"] += 1
			prev_type = b"<s>"
			sentence_index = 0
		else:
			prev_type = word_type
			sentence_index += 1
	return all_num / all_index




if __name__ == "__main__":
	if len(argv) != 3:
		print("Usage: # python %s \"TRAIN_FILE\" \"TEST_FILE\" " % argv[0], file=stderr)
		quit()
	# training
	hmm = Hmm()
	training_file = argv[1]
	hmm.training(training_file)
	# forward algolithm
	test_file = argv[2]
#	forward( hmm, test_file)
	print("ACCURACY_RATE :",viterbi( hmm, test_file),file=stderr)


#	for k,v in sorted(trans_prob.items()):
#		print (k[0].decode('utf-8'),k[1].decode('utf-8'),v)
#	for k,v in sorted(word_prob.items()):
#		print (k[0].decode('utf-8'),k[1].decode('utf-8'),v)

