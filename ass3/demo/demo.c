#include "wn.h"
int main()
{
    printf ("wn_init: %d\n", wninit());
    /* printf ("phor init: %d\n", morphinit()); */
    /* printf ("apples -> %s\n", morphword("apples", 1)); */
    /* printf ("apples -> %s\n", morphword("apples", 3)); */
    /* printf ("fucking -> %s\n", morphword("fucking", 2)); */
    printf ("in_wm apples : %d\n", in_wn("apples", 0));
    printf ("in_wm apple : %d\n", in_wn("apple", 0));
    printf ("morphword 2 apple -> %s\n", morphword("apples", 1));
    printf ("morphword 2 apples -> %s\n", morphword("apples", 1));
    printf ("morphword 2  meant-> %s\n", morphword("meant", 2));

    return 0;
}
