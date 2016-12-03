from collections import defaultdict
import sys

grammar = defaultdict(list)
table = []
words = []


def find_lhses(rhs):
    # TODO: treat with loop unit production (rules: A -> B, B -> A)
    # for unit production
    lhses = []
    for lhs in grammar[rhs]:
        lhses.append(lhs)
        lhses.extend(find_lhses((lhs,)))
    return lhses


def parse(sentence):
    sentence = "\"book the flight\""
    print(sentence)
    global words
    words = sentence[1:-1].split()

    n = len(words)
    global table
    table = [[defaultdict(list) for i in range(n+1)] for j in range(n+1)]
    for j in range(1, n+1):
        print("========================")
        print("words[j-1]:", words[j-1])
        print("j:", j)
        rhs = ('_{0}'.format(words[j-1]),)
        for lhs in find_lhses(rhs):
            table[j-1][j][lhs].append(rhs)
            print("(x,y):", j-1, j)
            print("(lhs,rhs):", lhs, rhs)
        for i in reversed(range(j-1)):
            print("i:", i)
            for k in range(i+1, j):
                print("(i,j,k):", i, j, k)
                print("(x,y):", i, j)
                # B of A -> BC
                for B, rhs_B in table[i][k].items():
                    print("(B, rhs_B):", B, rhs_B)
                    # C of A -> BC
                    for C, rhs_C in table[k][j].items():
                        print("(C, rhs_C):", C, rhs_C)
                        rhs = (B, C)
                        for lhs in find_lhses(rhs):
                            table[i][j][lhs].append([{B: rhs_B}, {C: rhs_C}])
                            print("(lhs,rhs):", lhs, rhs)
    if 'S' in table[0][n]:
        print('success')
    else:
        print('fail')


def stat():
    # the number of edges
    n = len(words)
    print("length:   {0}".format(n))
    num_edges = sum(sum(len(cell)for cell in row) for row in table)
    print("# edges:  {0}".format(num_edges))
    trees = table[0][n]['S']
    print("# trees:  {0}".format(len(trees)))


def recursive_print(parts):
    for part in parts:
        print("part:", part)
        if isinstance(part, dict):
            for key, value in part.items():
                if isinstance(value, dict):
                    print("  {0}".format(key))
                    recursive_print(value)
                else:
                    print("  {0} {1}".format(key, value))


def trees():
    """
    output parse tree
    FIXME: this function is incomplete
    """
    n = len(words)
    print('S')
    trees = table[0][n]['S']
    for tree in trees:
        print(tree)
        recursive_print(tree)


def read_grammar(file):
    # read
    with open(file, 'r') as f:
        line = f.readline()
        while line:
            line = line.rstrip("\n")
            rule = line.split()
            # lhs: left hand side, rhs: right hand side
            lhs, rhs, prob = rule[0], rule[1:-1], rule[-1]
            grammar[tuple(rhs)].append(lhs)
            line = f.readline()


def main():
    args = sys.argv
    if len(args) != 2:
        print('Usage: python {0} grammar'.format(args[0]))
        sys.exit()
    read_grammar(args[1])

    sys.stdout.write("> ")
    # input loop
    for line in iter(sys.stdin.readline, ""):
        parts = line.split()
        if len(parts) == 0:
            print("Unknown command. Please input 'parse \"sentence\"' or 'stat'")
        else:
            command = parts[0]
            if command == 'parse':
                parse(' '.join(parts[1:]))
            elif command == 'stat':
                stat()
            elif command == 'trees':
                trees()
            else:
                print("Unknown command. Please input 'parse \"sentence\"' or 'stat'")
        sys.stdout.write("> ")


if __name__ == '__main__':
    main()
