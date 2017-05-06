#!/bin/bash
cp ../stopwords/stop.words ./
g++ -o fuck 1.cpp
cat *.words|tr '[A-Z]' '[a-z]'| grep -E '^[a-z]+$'|sort|uniq > all
./fuck |sort|uniq > word_bank
python ss.py > words_relation

g++ -o fuck1 2.cpp
./fuck1 > concept_search_dict.tmp

python3 merge.py > concept_search_dict

