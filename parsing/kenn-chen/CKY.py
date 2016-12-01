import sys
from collections import defaultdict

def find_rules(rhs, grammar):
	if rhs not in grammar:
		return []

	lhses = grammar[rhs]
	rules = lhses[:]
	for lhs in lhses:
		if lhs in grammar:
			rules.extend(find_rules(lhs, grammar))
	return set(rules)

def matching(rules1, rules2):
	rules = []
	for rule1 in rules1:
		for rule2 in rules2:
			rules.append([rule1[0] + " " + rule2[0], rule1[0], rule2[0]])
	return rules

def recover(dp, p):
	if not p[1]:
		return [[p[0], p[2]]]

	p1 = dp[p[1][0]][p[1][1]]
	left = []
	for x in p1:
		if x[0] == p[1][2]:
			left.extend(recover(dp, x))

	p2 = dp[p[2][0]][p[2][1]]
	right = []
	for x in p2:
		if x[0] == p[2][2]:
			right.extend(recover(dp, x))

	result = []
	for x in left:
		for y in right:
			result.append([p[0], x, y])
	return result

def toCFG(CNF):
	if type(CNF[1]) == str:
		return CNF

	output = []
	if not CNF[0].startswith("X"):
		output.append(CNF[0])

	for i in range(1, len(CNF)):
		if CNF[i][0].startswith("X"):
			x = toCFG(CNF[i])
			if type(x[0]) == str:
				output.append(x)
			else:
				output.extend(x)
		else:
			output.append(toCFG(CNF[i]))

	return output

def count_edge(dp, n):
	count = 0
	for i in range(0, n):
		for j in range(1, n+1):
			tmp = set([x[0] for x in dp[i][j]])
			count += len(tmp)
	return count


def cky_parser(words):
	words = words.split()
	n = len(words)
	dp = [[set()]*(n+1) for _ in range(n+1)]

	grammar = defaultdict(list)
	for line in sys.stdin:
		lhs, rhs = line.rsplit(" ", 1)[0].split(" ", 1)
		grammar[rhs].append(lhs)

	for j in range(1, n+1):
		word = "_" + words[j-1]
		rules = find_rules(word, grammar)
		rules = set([(rule, None, words[j-1]) for rule in rules])
		dp[j-1][j] = rules

		for i in range(j-2, -1, -1):
			for k in range(i+1, j):
				rhses = matching(dp[i][k], dp[k][j])
				for rhs in rhses:
					rules = find_rules(rhs[0], grammar)
					if rules:
						rules = set([(rule, (i,k,rhs[1]), (k,j,rhs[2])) for rule in rules])	
						dp[i][j] = dp[i][j].union(rules)

	last = dp[0][n]
	CNF_trees = []
	trees_count = 0
	for tree in last:
		if tree[0] == "S":
			trees_count += 1
			CNF_trees.extend(recover(dp, tree))
	
	for tree in CNF_trees:
		CFG_tree = toCFG(tree)
		print(CFG_tree)

	edges_count = count_edge(dp, n)
	print("#edge: %d" % edges_count)
	print("#tree: %d" % trees_count)

if __name__ == "__main__":
	cky_parser(sys.argv[1]);