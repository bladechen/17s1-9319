import sys
from operator import itemgetter
from collections import OrderedDict
import os
total={}
def compare(x, y):
    if total[x] != total[y]:
	return (total[y] > total[x])
    else:
	return (x > y)

hh={}
flag= -1
for c in range(1, len(sys.argv)):
    # print total
    f=sys.argv[c]
    flag += 1
    if os.path.isfile(f) == False:
        print "not exist file " + f
        continue
    else:
        fd = open (f, "r")
        while True:
            line =  fd.readline()
            if not line:
                break
            line = line.strip()
            vv = line.rsplit(" ", 1)
            # print vv[0]
            # print vv[1]
            vv = vv[0]
            vv = vv.split (":", 1)
            # vv[1] =vv[1].strip()
            vv[0]=vv[0].strip()
            vv[1]=vv[1].strip()

            if len(vv) != 2:
                print "fuck your sister"
            if vv[0] not in total:
                total[vv[0]] = int(vv[1])
            else:
                total[vv[0]] += int(vv[1])

            if vv[0] not in hh:
                hh[vv[0]] = ( 1<< flag)
            else:
                hh[vv[0]] |= ( 1<< flag)
            # total[vv[1]]
# print hh
# print ((1<<(len(sys.argv) - 1)) -1)
# d = OrderedDict(sorted(total.items(), key=itemgetter(1), reverse=True))
flag=0
d = sorted(total.keys(), cmp=compare)
for vv in sorted(total.iteritems(), key=lambda(k, v): (-v, k)):
    i = vv[0]
    if i not in hh:
        print "fuck your sister"
        continue
    if hh[i] != ((1<<(len(sys.argv) - 1)) -1):
        continue
    fuck = i.split('/')
    flag =1
    print fuck[len(fuck) - 1], total[i]

if flag == 0:
    print ""
# sorted(dict.items(), lambda x, y: cmp(x[1], y[1]), reverse=True)
# print total

