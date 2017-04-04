#include <stdio.h>
#include "fuck.h"
#include "bwt_search.h"
#include <vector>
using namespace std;
int main(int argc, char** argv)
{
    if (argc < 4)
    {
        printf ("argc < 4\n");
        return -1;
    }
    CBwtSearch *bwt = new CBwtSearch();
    if (bwt->init(argv[1], argv[2]) != 0)
    {
        printf ("init error\n");
        return -2;
    }
    if (bwt->build_index() != 0)
    {
        printf ("build index error\n");
        return -3;
    }
    vector<string> q;
    for (int i = 3; i < argc; i ++)
    {
        q.push_back(argv[i]);
    }
    bwt->run(q);
    delete bwt;

    return 0;
}
