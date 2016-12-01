import sys
from collections import defaultdict

dummy_count = 0
dummies = {}

def is_terminal(symbol):
	return symbol.startswith("_")

def count(rhs):
	terminal = 0
	for symbol in rhs:
		if is_terminal(symbol):
			terminal += 1
	return terminal, len(rhs)-terminal

def get_dummy(rhs):
	global dummies
	rhs = "-".join(rhs)
	if rhs in dummies:
		return dummies[rhs]

	global dummy_count
	dummy_count += 1
	new_dummy = "X" + str(dummy_count)
	dummies[rhs] = new_dummy
	return new_dummy

def convert():
	rules = defaultdict(list)
	
	for line in sys.stdin:
		rule, prob = line.rstrip().rsplit(" ", 1)
		prob = float(prob)
		lhs, rhs = rule.split(" ", 1)
		rhs = rhs.split()

		t, nt = count(rhs)

		if t > 1 or (nt > 0 and t > 0):
			for i in range(len(rhs)):
				if is_terminal(rhs[i]):
					new_dummy = get_dummy([rhs[i]])
					rules[new_dummy] = [([s], 1.0)]
					rhs[i] = new_dummy

		_, nt = count(rhs)
		while nt > 2:
			new_dummy = get_dummy(rhs[0:2])
			rules[new_dummy] = [(rhs[0:2], 1.0)]
			rhs = [new_dummy] + rhs[2:]
			nt -= 1
		
		rules[lhs].append((rhs, prob))

	for lhs in rules:
		for rhs_prob in rules[lhs]:
			print(lhs, ' '.join(rhs_prob[0]), rhs_prob[1])


if __name__ == "__main__":
	convert()