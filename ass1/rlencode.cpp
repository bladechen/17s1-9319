#include <stdio.h>
#include "rle.h"

int main(int argc, char** argv)
{
    if (argc != 2 && argc != 3)
    {
        printf ("./rlencode [in] [out] or ./rlencode [in]");
        return -1;
    }
    CRle *p = new CRle(argv[1], argc == 2?"": argv[2]);
    p->run_encode();

    delete p;

    return 0;
}
