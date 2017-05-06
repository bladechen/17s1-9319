#!/bin/bash
cat stop*|awk '{print $1}'|tr '[A-Z]' '[a-z]'|sort|uniq > result_stoplist
g++ porter_stemmer.cpp
./a.out result_stoplist > stem_stopword
cat result_stoplist stem_stopword| sort |uniq |grep -E '[a-z]'> 'stop.words'
