#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include "a3_search.h"
using namespace std;
int main(int argc, char** argv)
{
    if (argc < 4)
    {
        printf ("argc < 4\n");
        return -1;
    }
	// cse has 4096 fd hard limit, which is enough for 2000 maximum orignal files.
	struct rlimit rlim;
	if (0 == getrlimit(RLIMIT_NOFILE, &rlim))
	{

		rlim.rlim_cur = rlim.rlim_max;
		setrlimit(RLIMIT_NOFILE, &rlim);
	}

	CA3Search *a3 = new CA3Search();
    if (a3->init(argv[1], argv[2]) != 0)
    {
        printf ("init error\n");
        return -2;
    }
    int idx = 3;
    double c_value = 0;
    if (strcmp(argv[3], "-c") == 0)
    {
        c_value = strtod(argv[4], NULL);
        idx = 5;
    }
    vector<string> query;
    for (int i = idx; i < argc; i ++)
    {
        query.push_back(argv[i]);
    }
    a3->run(query, c_value);
    delete a3;

    return 0;
}
