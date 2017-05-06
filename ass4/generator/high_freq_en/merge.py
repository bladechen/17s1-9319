import sys
import os

from collections import defaultdict


new_dic = defaultdict(dict)
fd = open('./concept_search_dict.tmp', 'r')
while 1:
    l = fd.readline()
    if not l:
        break
    l = l.strip()
    vec = l.split();
    key = vec[0]
    for i in range (1, len(vec)):
        new_dic[key][vec[i]] = 1

for i in sorted(new_dic.keys()):
    print (i + " ", end = "")
    for j in new_dic[i]:
        print (j + " ", end = "")
    print ("")


