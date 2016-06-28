# insertion cost: 		1
# deletion cost:		1
# substitution cost: 	2
class Action:
	KEEP, INS, DE, SUB = range(4)

class Edist:
	def __init__(self, source, target, ins=1, de=1, sub=2):
		self.source = source
		self.target = target
		self.ins = ins
		self.de  = de
		self.sub = sub

		self.dp = None
		self.actions = None

	def dist(self):
		if self.dp: return self.dp[-1][-1]

		m, n = len(self.source), len(self.target)
		dp = [[0]*(n+1) for _ in range(m+1)]
		for i in range(1, m):
			dp[i][0] = i
		for j in range(1, n):
			dp[0][j] = j

		for i in range(1, m+1):
			for j in range(1, n+1):
				if source[i-1] == target[j-1]:
					dp[i][j] = dp[i-1][j-1]
				else:
					dp[i][j] = min(dp[i-1][j]+self.de, dp[i][j-1]+self.ins, dp[i-1][j-1]+self.sub)
		self.dp = dp
		return dp[m][n]
		
	def edit(self):
		if self.actions: return self.actions
		if not self.dp: self.dist()

		dp = self.dp
		m, n = len(self.source), len(self.target)
		path = [[(0, 0, 0)]*(n+1) for _ in range(m+1)]

		for i in range(1, m):
			path[i][0] = (i-1, 0, Action.DE)
		for j in range(1, n):
			path[0][j] = (0, j-1, Action.INS)

		for i in range(1, m+1):
			for j in range(1, n+1):
				if source[i-1] == target[j-1]:
					path[i][j] = (i-1, j-1, Action.KEEP)
				else:
					min_cost = min(dp[i-1][j]+self.de, dp[i][j-1]+self.ins, dp[i-1][j-1]+self.sub)
					if   min_cost == dp[i-1][j-1]+self.sub:
						path[i][j] = (i-1, j-1, Action.SUB)
					elif min_cost == dp[i][j-1]+self.ins:
						path[i][j] = (i, j-1, Action.INS)
					else:
						path[i][j] = (i-1, j, Action.DE)

		i, j = m, n
		actions = []
		while i != 0 or j != 0:
			actions.append(path[i][j][-1])
			i, j = path[i][j][:2]
		self.actions = actions[::-1]
		return self.actions

	def eprint(self, actions):
		action_str = "action: "
		source_str = "source: "
		target_str = "target: "

		i = j = 0
		for action in actions:
			if   action == Action.KEEP:
				action_str += '{:^3}'.format("=")
				source_str += '{:^3}'.format(self.source[i])
				target_str += '{:^3}'.format(self.target[j])
				i += 1; j += 1
			elif action == Action.INS:
				action_str += '{:^3}'.format("+")
				source_str += '{:^3}'.format(" ")
				target_str += '{:^3}'.format(self.target[j])
				j += 1
			elif action == Action.DE:
				action_str += '{:^3}'.format("-")
				source_str += '{:^3}'.format(self.source[i])
				target_str += '{:^3}'.format(" ")
				i += 1
			else:
				action_str += '{:^3}'.format("*")
				source_str += '{:^3}'.format(self.source[i])
				target_str += '{:^3}'.format(self.target[j])
				i += 1; j += 1
		
		print("\n".join(["alignment: ", action_str, source_str, target_str]))


if __name__ == "__main__":
	source = input("source: ")
	target = input("target: ")
	edist  = Edist(source, target)
	print()
	print("edit distance: %d" % edist.dist())
	print()
	edist.eprint(edist.edit())