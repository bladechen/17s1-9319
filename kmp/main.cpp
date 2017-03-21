#include <iostream>
#include <stdio.h>
#include <string>
using namespace std;

// 先对想要拿去匹配的模式串进行自我匹配，求出 next 模式数组
void getNext(string strSearch, int * next)
{
    int length = strSearch.length();
    // next 数组的第一位总是0
    next[0] = -1;
    // 模式串已遍历到第 j 位
    int j = 0,
    // 与第 k 位模式串比较（同时也是可能的 next 数组的值）
        k = -1;

    while (j < length - 1)
    {
        // k = -1：上一次自我匹配失败，从头开始进行下一次自我匹配
        // strSearch[i] = strSearch[k]：当前位置自我匹配成功
        if (k == -1 || strSearch[j] == strSearch[k])
        {
            // 以上两种情况均直接移动 i k 到下一位置
            j++;
            k++;
            // 同时记下当前自我匹配 k 的位置，赋值给 next 数组（next[i]）
            // KMP 模式匹配失败时，就把模式串跳转到该 k 位置，继续尝试匹配
            next[j] = k;
        }
        // 自我匹配失败，将 k 移到当前位置中 next 数组中储存的位置（next[k]），继续尝试匹配。该跳转和 KMP 模式匹配十分类似
        else
        {
            k = next[k];
        }
    }
}

// 优化后的求 next 模式数组方法
// 加快了模式匹配失败时模式串移动的位数
void getNextV2(string strSearch, int * next)
{
    int length = strSearch.length();
    next[0] = -1;
    int j = 0, k = -1;

    while (j < length - 1)
    {
        if (k == -1 || strSearch[j] == strSearch[k])
        {
            j++;
            k++;
            // 如果移动后的下一位相同，本着“一错都错”的原则，直接把前面 next 数组的值（next[k]）赋过来
            if (strSearch[j] == strSearch[k])
            {
                next[j] = next[k];
            }
            // 不相同，则记下当前自我匹配 k 的位置，赋值给 next 数组
            else
            {
                next[j] = k;
            }
        }
        else
        {
            k = next[k];
        }
    }
}

// 正式开始 KMP 模式匹配
// str：原字符串，文本串
// strSearch：想要拿去匹配的模式串
int KMPSearch(string str, string strSearch, int * next)
{
    int lengthStr = str.length();
    int lengthSearch = strSearch.length();
    // 文本串已遍历到的位置
    int i = 0,
    // 模式串已遍历到的位置
        j = 0;

    while (i < lengthStr && j < lengthSearch)
    {
        // j = -1：上一次模式匹配失败，j 因 next 数组被移动到开头位置
        // str[i] == strSearch[j]：当前位置模式匹配成功
        if (j == -1 || str[i] == strSearch[j])
        {
            i++;
            j++;
        }
        // 模式匹配失败，跳转到 next 数组位置继续尝试匹配
        else
        {
            j = next[j];
        }
    }
    // 如果是因为模式串遍历完而跳出循环，则表示 KMP 模式匹配成功，返回位置
    if (j == lengthSearch)
    {
        return i - j;
    }
    // 否则查找失败，返回-1
    else
    {
        return -1;
    }
}

// 求模式串在文本串中出现的次数
int searchCount(string str, string strSearch, int * next)
{
    int lengthStr = str.length();
    int lengthSearch = strSearch.length();
    int i = 0, j = 0, count = 0;
    while (i < lengthStr)
    {
        if (j == -1 || str[i] == strSearch[j])
        {
            i++;
            j++;
        }
        else
        {
            j = next[j];
        }
        if (j == lengthSearch)
        {
            count++;
            i--;
            j = next[j - 1];
        }
    }
    return count;
}

/*
完整题目要求：http://hihocoder.com/problemset/problem/1015
输入
第一行一个整数N，表示测试数据组数。
接下来的N*2行，每两行表示一个测试数据。在每一个测试数据中，第一行为模式串，由不超过10^4个大写字母组成，
第二行为原串，由不超过10^6个大写字母组成。
其中N<=20

输出
对于每一个测试数据，按照它们在输入中出现的顺序输出一行Ans，表示模式串在原串中出现的次数。

样例输入
5
HA
HAHAHA
WQN
WQN
ADA
ADADADA
BABABB
BABABABABABABABABB
DAD
ADDAADAADDAAADAAD

样例输出
3
1
3
1
0
*/
int main()
{
    string str, strSearch;
    int num, * out;
    cin >> num;
    out = new int[num];
    getchar();
    for (int i = 0; i < num; i++)
    {
        getline(cin, strSearch);
        getline(cin, str);
        int * next;
        next = new int[strSearch.length()];
        getNext(strSearch, next);
        out[i] = searchCount(str, strSearch, next);
        delete[] next;
    }
    for (int i = 0; i < num; i++)
    {
        cout << out[i] << endl;
    }
    delete[] out;
    return 0;
}
