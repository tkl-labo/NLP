# python3.5.1
import sys
import numpy as np

class MinimumEditDistance(object):
    # Levenshtein distance
    NO_COST = 0
    INS_COST = 1
    DEL_COST = 1
    SUBST_COST = 2

    def __init__(self, source, target):
        '''
        target: 行方向 (a column is a character of the target word)
        source: 列方向 (a row is a character of the source word)
        '''
        self.source = source
        self.target = target

    @classmethod
    def _ins_cost(cls, tgt_char):
        return cls.INS_COST


    @classmethod
    def _subst_cost(cls, src_char, tgt_char):
        if src_char == tgt_char:
            return cls.NO_COST
        else:
            return cls.SUBST_COST


    @classmethod
    def _del_cost(cls, src_char):
        return cls.DEL_COST


    def calc(self):
        # initialize
        distance = np.zeros((len(self.source)+1, len(self.target)+1))
        distance[0] = range(len(self.target)+1)
        distance[:,0] = range(len(self.source)+1)

        # update from distance[1,1] to distance[len(self.source), len(self.target)]
        for i, src_char in enumerate(self.source):
            for j, tgt_char in enumerate(self.target):
                src_idx = i+1
                tgt_idx = j+1
                distance[src_idx, tgt_idx] = min(
                    distance[src_idx-1, tgt_idx]+self._ins_cost(tgt_char),
                    distance[src_idx-1, tgt_idx-1]+self._subst_cost(src_char, tgt_char),
                    distance[src_idx, tgt_idx-1]+self._del_cost(src_char),
                )
        return int(distance[len(self.source), len(self.target)])


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

    med = MinimumEditDistance(source_word, target_word)
    print(med.calc())
