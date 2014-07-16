#!/usr/bin/env python
# -*- coding:utf-8 -*-
import sys,os
#import math,re,datetime
import romkan
from edit_distance import edit_distance
import unicodedata

def hiragana_distance(target,source):
    roma1 = unicodedata.normalize("NFKC",romkan.to_roma(target.decode("utf8")))
    roma2 = unicodedata.normalize("NFKC",romkan.to_roma(source.decode("utf8")))
    return edit_distance(roma1,roma2)
    

if __name__ == "__main__":
    hiragana1 = sys.argv[1]
    hiragana2 = sys.argv[2]
    print hiragana_distance(hiragana1,hiragana2)
