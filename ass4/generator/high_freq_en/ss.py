from __future__ import print_function
from __future__ import print_function
import traceback
import logging

from gensim.models import KeyedVectors



def is_ascii(s):
    for i in s:
        if (i >= 'A' and i <= 'Z') or (i >= 'a' and i <= 'z'):
            continue
        else:
            return False
    return True
    # return all(ord(c) < 128 for c in s)


# Creating the model
en_model = KeyedVectors.load_word2vec_format('./wiki.simple.vec')

# Getting the tokens
words = []
for word in en_model.vocab:
    words.append(word)

# Printing out number of tokens available
# print("Number of Tokens: {}".format(len(words)))

# Printing out the dimension of a word vector
# print("Dimension of a word vector: {}".format(
#     len(en_model[words[0]])
# ))
#
# # Print out the vector of a word
# print("Vector components of a word: {}".format(
#     en_model[words[0]]
# ))
# find_similar_to = 'father'
#
# # Finding out similar words [default= top 10]
# for similar_word in en_model.similar_by_word(find_similar_to):
#     print("Word: {0}, Similarity: {1:.2f}".format(
#         similar_word[0], similar_word[1]
#     ))
#
# print ("\n\n\n\n\n")
#
# find_similar_to = 'god'
#
# # Finding out similar words [default= top 10]
# for similar_word in en_model.similar_by_word(find_similar_to):
#     print("Word: {0}, Similarity: {1:.2f}".format(
#         similar_word[0], similar_word[1]
#     ))
#
# print ("\n\n\n\n\n")
#
# # Pick a word
# find_similar_to = 'car'
#
# # Finding out similar words [default= top 10]
# for similar_word in en_model.similar_by_word(find_similar_to):
#     print("Word: {0}, Similarity: {1:.2f}".format(
#         similar_word[0], similar_word[1]
#     ))
# find_similar_to = 'a'

# Finding out similar words [default= top 10]
# for similar_word in en_model.similar_by_word(find_similar_to):
#     print("Word: {0}, Similarity: {1:.2f}".format(
#             similar_word[0].encode('utf-8').strip(), similar_word[1]
#         ))
#
    # print (similar_word)


fd = open ('./word_bank', 'r')
while True:
    ss = fd.readline();
    if not ss:
        break
    ss = ss.strip()
    print (ss + " ", end = "")
    try:
        for similar_word in en_model.similar_by_word(ss, topn = 100):
            try:
                if is_ascii(similar_word[0]) is True:
                    print ( ',' + similar_word[0].lower()  , end = "")
                    # print("Word: {0}, Similarity: {1:.2f}".format(
                    #     similar_word[0], similar_word[1]
                    # ))

            except Exception as e:
                pass
                # print (traceback.format_exc())
    except:
        pass
        # print (traceback.format_exc())
    print ("")

# Output
# Word: cars, Similarity: 0.83
# Word: automobile, Similarity: 0.72
# Word: truck, Similarity: 0.71
# Word: motorcar, Similarity: 0.70
# Word: vehicle, Similarity: 0.70
# Word: driver, Similarity: 0.69
# Word: drivecar, Similarity: 0.69
# Word: minivan, Similarity: 0.67
# Word: roadster, Similarity: 0.67
# Word: racecars, Similarity: 0.67

# Test words

# Output

# Word : delhi , Similarity: 0.77
# Word : indore , Similarity: 0.76
# Word : bangalore , Similarity: 0.75
# Word : mumbai , Similarity: 0.75
# Word : kolkata , Similarity: 0.75
# Word : calcutta,india , Similarity: 0.75
# Word : ahmedabad , Similarity: 0.75
# Word : pune , Similarity: 0.74
# Word : kolkata,india , Similarity: 0.74
#
