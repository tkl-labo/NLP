# coding: utf-8
import sys
import itertools
from collections import *

def read_rules(filename):
	grammar = defaultdict(list)
	lexicon = defaultdict(list)
	for line in open(filename, 'r'):
		token = line.rstrip().split(' ')
		if len(token) == 3:
			grammar[token[0]].append(token[1:])
		elif len(token) == 2:
			lexicon[token[0]].append(token[1].lstrip('_'))
		else:
			print('Error: read_rules:', line)
			sys.exit(0)
	
	return grammar, lexicon


def cky_recognition_old(s, grammar, lexicon):
	cky = [[[] for j in range(len(s) + 1)] for i in range(len(s))]
	
	for j in range(len(s) + 1):
		for i in reversed(range(j)):

			if j - i == 1: # グラフの底辺。lexiconを参照する場合
				for key in lexicon.keys():
					if s[i] in lexicon[key]:
						cky[i][j].append({'head': key})
						
			else: # グラフの底辺以外。各cky[i][k]、cky[j][k]の組み合わせを右辺に持つようなルールをcky[i][j]に追加する
				for k in range(i + 1, j):
					for left in cky[i][k]:
						for right in cky[k][j]:
							for candidate in grammar.keys():
								if exists_in_grammar(candidate, left['head'], right['head'], grammar):
										cky[i][j].append({'head': candidate, 'left': left['head'], 'left_place':(i, k), 'right': right['head'], 'right_place':(k, j)})
			print 'cky[', i, '][', j, ']:', cky[i][j]
			
	return cky


def cky_recognition(s, grammar, lexicon):
	cky = [[[] for j in range(len(s) + 1)] for i in range(len(s))]
	
	for j in range(len(s) + 1):
		for i in reversed(range(j)):

			if j - i == 1: # グラフの底辺。lexiconを参照する場合
				for key in lexicon.keys():
					if s[i] in lexicon[key]:
						cky[i][j].append({'head': key})
						
			else: # グラフの底辺以外。各cky[i][k]、cky[j][k]の組み合わせを右辺に持つようなルールをcky[i][j]に追加する
				for k in range(i + 1, j):
					for left in cky[i][k]:
						for right in cky[k][j]:
							for candidate in grammar.keys():
								if exists_in_grammar(candidate, left['head'], right['head'], grammar):
										cky[i][j].append({'head': candidate, 'left': left['head'], 'left_place':(i, k), 'right': right['head'], 'right_place':(k, j)})
			# print 'cky[', i, '][', j, ']:', cky[i][j]

	return cky


def exists_in_grammar(head, left, right, grammar):
	if [left, right] in grammar[head]:
		return True
	else:
		return False

def restore_tree(s, head, head_place, indent, cky, lexicon):

	for candidate in cky[head_place[0]][head_place[1]]:
		if candidate['head'] == head:
			if head[0] != 'X':
				line = ''
				for i in range(indent): line += '-'
				print line, head,
	
			# グラフの底辺(逆三角の斜辺)だった場合
			if head_place[1] - head_place[0] == 1and head[0] != 'X':
				print  '(', s[head_place[0]], ')'
				return

			# 底辺以外
			else:
				if candidate['head'] == head:
					if head[0] != 'X':
						print
						restore_tree(s, candidate['left'], candidate['left_place'], indent + 1, cky, lexicon)
						restore_tree(s, candidate['right'], candidate['right_place'], indent + 1, cky, lexicon)
					else:
						restore_tree(s, candidate['left'], candidate['left_place'], indent, cky, lexicon)
						restore_tree(s, candidate['right'], candidate['right_place'], indent, cky, lexicon)

						
if __name__ == '__main__':
	grammar, lexicon = read_rules('./data/rules.cnf')
	s = sys.argv[1:]

	cky = cky_recognition(s, grammar, lexicon)
	restore_tree(s, 'S', (0, len(s)), 0, cky, lexicon)