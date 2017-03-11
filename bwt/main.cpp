#include <stdio.h>
#include <vector>
#include <stdlib.h>
#include <stack>
#include <map>
#include <string.h>
using namespace std;
int input_buflen = 0;
char input_buf[2000];
int rotated_index[2000];

char decoded_buf[2000];

char last_chars[2000];

#define UNIQUE_SYMBOL '#'


int string_cmp(const void* a, const  void *b)
{
    int i = *(int*)a;
    int j = *(int*)b;
    int total_length = input_buflen;
    while (total_length --)
    {
        if (input_buf[i] == input_buf[j])
        {
            i = (i + 1) % input_buflen;
            j = (j + 1) % input_buflen;
        }
        else
        {
            return input_buf[i] - input_buf[j];
        }

    }
    return 0;
}

void init_rotated_index(int n)
{
    for (int i = 0; i < n; i ++)
    {
        rotated_index[i] = i;
    }
}

void do_rotated_sorting()
{
    qsort(rotated_index, input_buflen, sizeof(int), string_cmp);

    return;
}
void get_lastchars()
{
    int tmp = 0;
    for (int i = 0; i < input_buflen; i ++)
    {
        int index = rotated_index[i] - 1;
        if (index == -1)
        {
            last_chars[tmp ++] = input_buf[input_buflen - 1];

        }
        else
        {
            last_chars[tmp ++] = input_buf[index];
        }
    }
    last_chars[tmp] = 0;
    return;
}

map<char, int> char_sum;
int pre_sum[300];
// vector<int> prev_index[300];
void reverse_bysum()
{
    char_sum.clear();
    // for (int i = 0; i < 300; i ++) prev_index.clear();
    int tmp[300];
    memset(tmp, 0, sizeof(tmp));
    memset(pre_sum, 0, sizeof(pre_sum));
    int position = 0;
    for (int i = 0; i < input_buflen;i ++)
    {
        // prev_index[(unsigned char)last_chars[i]].push_back(i);
        if (last_chars[i] == UNIQUE_SYMBOL)
        {
            position = i;
        }
        pre_sum[i] = tmp[(unsigned char)last_chars[i]];
        tmp[(unsigned char)last_chars[i]] ++;
        if (char_sum.find(last_chars[i]) == char_sum.end())
        {
            char_sum[last_chars[i]] = 1;
        }
        else
        {
            char_sum[last_chars[i]] ++;
        }

    }
    int count = 0;
    for (map<char, int>::iterator it = char_sum.begin(); it != char_sum.end(); it ++)
    {
        int t = it->second;
        it->second = count;
        count += t;

    }
    decoded_buf[input_buflen] = 0;
    int len = input_buflen;
    unsigned char cur_char = UNIQUE_SYMBOL;
    while (len --)
    {
        decoded_buf[len] = cur_char;
        int index = char_sum[cur_char] + pre_sum[position];
        position = index;
        cur_char = last_chars[index];

    }

    return;
}


char ordered_chars[2000];
int char_cmp(const void* a, const  void *b)
{
    return *(char*) a - *(char*)b;

}
void reverse_byrelation()
{
    vector<int> prev_index[300];
    memcpy(ordered_chars, last_chars, input_buflen + 1);
    qsort(ordered_chars, input_buflen, sizeof(char), char_cmp);
    for (int i = 0; i < 300; i ++) prev_index[i].clear();
    for (int i = 0; i < input_buflen;i ++)
    {
        prev_index[(unsigned char)ordered_chars[i]].push_back(i);
    }
    int position = 0;
    for (int i = 0; i < input_buflen; i ++)
    {
        if (last_chars[i] == UNIQUE_SYMBOL)
        {
            position = i;
        }
    }
    decoded_buf[input_buflen] = 0;
    int len = input_buflen;
    unsigned char cur_char = UNIQUE_SYMBOL;
    int cur_index = 0;
    printf ("%s\n%s\n", ordered_chars, last_chars);
    while (len --)
    {
        decoded_buf[len] = cur_char;
        cur_index = prev_index[cur_char].back();
        prev_index[cur_char].pop_back();
        printf ("%c %d\n", cur_char, cur_index);
        cur_char = last_chars[cur_index];
    }
    return;
}

int main()
{
    gets(input_buf);
    input_buflen = strlen(input_buf);
    input_buf[input_buflen] = UNIQUE_SYMBOL; // append a uniq symbol
    input_buflen ++;
    input_buf[input_buflen] = 0;
    printf ("begin doing encoding, length: %d, %s\n", input_buflen, input_buf);
    init_rotated_index(input_buflen);
    do_rotated_sorting();
    get_lastchars();
    printf ("%s\n", last_chars);

    reverse_bysum();

    decoded_buf[input_buflen - 1] = 0;
    printf ("decoded: %s\n", decoded_buf);

    reverse_byrelation();
    decoded_buf[input_buflen - 1] = 0;
    printf ("decoded: %s\n", decoded_buf);

    return 0;
}
