import sys


def is_terminal(symbol):
    if symbol[0] == '_':
        return True
    return False


def solve(file):
    input_grammar = []
    grammar = []

    # read
    with open(file, 'r') as f:
        line = f.readline()
        while line:
            line = line.rstrip("\n")
            rule = line.split()
            # lhs: left hand side, rhs: right hand side
            lhs, rhs, prob = rule[0], rule[1:-1], rule[-1]
            input_grammar.append([lhs, rhs, prob])
            line = f.readline()

    # convert terminal to non_terminal
    for lhs, rhs, prob in input_grammar:
        if len(rhs) >= 2:
            _rhs = []
            for symbol in rhs:
                if is_terminal(symbol):
                    new_symbol = 'Z' + symbol
                    grammar.append([new_symbol, [symbol], 1.0])
                    symbol = new_symbol
                _rhs.append(symbol)
            grammar.append([lhs, _rhs, prob])
        else:
            grammar.append([lhs, rhs, prob])

    # binalize
    dummy_count = 0
    unique_rhs_rule = {}
    new_grammar = []
    for lhs, rhs, prob in grammar:
        _rhs = rhs
        while len(_rhs) >= 3:
            # print(lhs, _rhs, prob)
            # break
            # print(unique_rhs_rule)
            if tuple(_rhs[:2]) not in unique_rhs_rule:
                new_symbol = 'X{0}'.format(dummy_count)
                new_grammar.append([new_symbol, _rhs[:2], 1.0])
                unique_rhs_rule[tuple(_rhs[:2])] = new_symbol
                _rhs2 = [new_symbol]
                _rhs2.extend(_rhs[2:])
                _rhs = _rhs2
                dummy_count += 1
            else:
                _rhs2 = [unique_rhs_rule[tuple(_rhs[:2])]]
                _rhs2.extend(_rhs[2:])
                _rhs = _rhs2
        new_grammar.append([lhs, _rhs, prob])

    for lhs, rhs, prob in new_grammar:
        output = lhs
        for symbol in rhs:
            output += " {0}".format(symbol)
        output += " {0}".format(prob)
        print(output)


def main():
    args = sys.argv
    if len(args) != 2:
        print('Usage: python {0} grammar'.format(args[0]))
        sys.exit()
    solve(args[1])

if __name__ == '__main__':
    main()
