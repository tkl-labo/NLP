# python3.5.1
import sys
import numpy as np

class MinimumEditDistance(object):
    # Levenshtein distance
    NO_COST = 0
    INS_COST = 1
    DEL_COST = 1
    SUBST_COST = 2

    def __init__(self):
        pass


    def _ins_cost(self):
        return MinimumEditDistance.INS_COST


    def _subst_cost(self, src_char, tgt_char):
        if src_char == tgt_char:
            return MinimumEditDistance.NO_COST
        else:
            return MinimumEditDistance.SUBST_COST


    def _del_cost(self):
        return MinimumEditDistance.DEL_COST


    def calc(self, source, target):
        '''
        target: 行方向 (a column is a character of the target word)
        source: 列方向 (a row is a character of the source word)
        '''
        # initialize
        distance = np.zeros((len(source)+1, len(target)+1))
        distance[0] = range(len(target)+1)
        distance[:,0] = range(len(source)+1)

        # update from distance[1,1] to distance[len(source), len(target)]
        for i, src_char in enumerate(source):
            for j, tgt_char in enumerate(target):
                src_idx = i+1
                tgt_idx = j+1
                distance[src_idx, tgt_idx] = min(
                    distance[src_idx-1, tgt_idx]+self._ins_cost(),
                    distance[src_idx-1, tgt_idx-1]+self._subst_cost(src_char, tgt_char),
                    distance[src_idx, tgt_idx-1]+self.DEL_COST,
                )
        return int(distance[len(source), len(target)])


if __name__ == "__main__":
    print("Please input 2 words.")
    while(True):
        input_words = input("> ")
        arr = input_words.split()
        if len(arr) == 2:
            break
        else:
            sys.stderr.write("It's not 2 words!\n")

    source_word = arr[0]
    target_word = arr[1]

    med = MinimumEditDistance()
    print(med.calc(source_word, target_word))
