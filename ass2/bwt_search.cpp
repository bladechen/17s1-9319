#include <string.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <map>
#include <stdio.h>
#include "bwt_search.h"
#include "assert.h"
const char* MAGIC_STRING = "deadbeaf";
// #define MAGIC_NUMBER 0xdeadbeef
using namespace std;


int CBwtSearch::init(const std::string& bwt, const std::string& index)
{
    _cur_block = -1;
    _result.clear();
    _bwt_file = new COpFile(bwt, O_RDONLY);
    assert(_bwt_file != NULL);
    _index_file = new COpFile(index, O_RDWR | O_CREAT);
    assert(_index_file != NULL);
    _bwt_file_size = _bwt_file->file_size();
    return 0;
}
int CBwtSearch::build_index()
{
    memset(this->char_bucket, -1, sizeof(this->char_bucket));

    int ret = _index_file->has_content();
    if (ret < 0)
    {
        assert(0);
    }
    //FIXME
    // if (ret)
    // {
    //     read_index_file();
    // }
    // else
    // {
        create_index_file();

    // }
    for (int i = 0; i < _block_num; i ++)
    {
        printf ("block %d\n", i);
        for (int j = 0; j < MAX_CHAR_COUNTS; j ++)
        {
            if (block_index[i][j])
            {
                printf ("%c %d\n", j , block_index[i][j]);
            }

        }
    }
    printf ("########\nbucket\n");
    for (int i = 0; i < MAX_CHAR_COUNTS; i ++)
    {
        if (char_bucket[i] != -1)
        {
            printf ("%c %d\n", i, char_bucket[i]);
        }
    }

    return 0;
}

void CBwtSearch::read_index_file()
{
    int ret = _index_file->read_file(_read_buffer, strlen(MAGIC_STRING));
    assert(ret ==  (int)strlen(MAGIC_STRING)  && strncmp(_read_buffer, MAGIC_STRING, strlen(MAGIC_STRING)) == 0);
    ret = _index_file->read_file((char*)char_bucket, sizeof(char_bucket));
    assert(ret == sizeof(char_bucket));
    ret = _index_file->read_file((char*)&_block_num, 4);
    assert(ret == 4);
    for (int i = 0; i < _block_num; i ++)
    {
        ret = _index_file->read_file((char*)block_index[i], sizeof(block_index[i]));
        assert(ret == sizeof(block_index[i]));
    }
    return;
}


void CBwtSearch::create_index_file()
{
    int tmp[128] = {0};
    for (int i = 0 ;i  < 128; i++) block_index[0][i] = 0;
    int block = 1;
    while (1)
    {
        if (block > 0)
        {
            for (int i = 0; i < 128; i ++)
            {
                block_index[block][i] = block_index[block - 1][i];
            }
        }
        _read_len = _bwt_file->read_file(_read_buffer, sizeof(_read_buffer)); // ensure MAX_BLOCK_SIZE == MAX_READ_BUFFER_SIZE
        assert(_read_len >= 0);
        for (int i = 0; i < _read_len; i++)
        {
            tmp[(int)_read_buffer[i]] ++;
            block_index[block][(int)_read_buffer[i]] ++;
        }
        block ++;

        if (_read_len < (int)sizeof(_read_buffer))
        {
            break;
        }
    }
    _block_num = block;
    int sum = 0;
    for  (int i = 0; i < 128; i ++)
    {
        if (tmp[i])
        {
            char_bucket[i] = sum;
            sum += tmp[i];
        }
    }

    write_index_file();

    return;
}

void CBwtSearch::write_index_file()
{
    int ret = _index_file->write_file((char*)MAGIC_STRING, strlen(MAGIC_STRING));
    assert(ret > 0);
    ret = _index_file->write_file((char*)char_bucket, sizeof(char_bucket));
    assert(ret == sizeof(char_bucket));
    ret = _index_file->write_file((char*)&_block_num, 4);
    assert(ret == 4);
    for (int i = 0; i <_block_num; i ++)
    {
        ret = _index_file->write_file((char*)block_index[i], sizeof(block_index[i]));
        assert(ret == sizeof(block_index[i]));
    }
    return;
}

void CBwtSearch::run(const std::vector<std::string>& query_strings)
{
    bool need_search = 1;
    std::string search_str = "";
    int search_index = 0;
    for (size_t i = 0; i < query_strings.size(); ++ i)
    {
        // find the longest string as the search string
        if (query_strings[i].length() > search_str.length())
        {
            search_str = query_strings[i];
            search_index = i;
        }
        // if there is a char not appearing in the raw content, nothing need to search
        for (size_t k = 0; k < query_strings[i].size(); k ++)
        {
            if (block_index[_block_num - 1][(int)query_strings[i][k]] == 0)
            {
                need_search = 0;
                break;
            }

        }
    }
    if (need_search == 0)
    {
        return;
    }
    assert(search_str != "");
    int last = 0;
    int first = 0;
    if (backward_search(search_str, first, last) != 0)
    {
        return;
    }
    printf ("final f l: %d %d\n", first, last);
    if (first > last)
    {
        return;
    }

    if (find_whole_string(search_str, first, last) != 0)
    {
        return;
    }
    for (std::map<int, std::string>::iterator it = _result.begin();
         it != _result.end() ;
         it ++)
    {
        bool all_match = 1;
        for (int i = 0; i < (int)query_strings.size(); ++ i)
        {
            if (search_index  == i)
            {
                continue;
            }
            if (strstr(it->second.c_str(), query_strings[i].c_str()) == NULL) // this one is boyer more?
            {
                all_match = 0;
                break;
            }
        }
        if (all_match  == 1)
        {
            printf ("[%d]%s\n",it->first, it->second.c_str() );
        }

    }
    return;
}
int CBwtSearch::backward_search(const std::string& search_str,int& first, int& last)
{

    assert(search_str.length() > 0);
    int cur_index = search_str.length() - 1;
    int cur_char = search_str[cur_index];
    first = char_bucket[cur_char];
    last = block_index[_block_num - 1][cur_char] + first - 1;
    if (last < first)
    {
        return -1;
    }
    while (cur_index -- && first <= last)
    {
        printf ("first last %d %d\n", first, last);
        cur_char = search_str[cur_index];
        int cur_char_count_first = occ(cur_char, first - 1);
        int cur_char_count_last = occ(cur_char, last );
        if (cur_char_count_first == cur_char_count_last)
        {
            // nothing find?
            return -1;
        }
        printf ("occ %d %d\n", cur_char_count_first, cur_char_count_last);
        first = cur_char_count_first + char_bucket[cur_char] ;
        last = cur_char_count_last + char_bucket[cur_char] - 1;
    }
    return 0;
}

int CBwtSearch::occ(char c, int pos_not_include_self)
{
    int prev_block = pos_not_include_self / MAX_BLOCK_SIZE;
    int remain_chars = pos_not_include_self % MAX_BLOCK_SIZE ;
    assert(prev_block <= _block_num - 2);

    int count = block_index[prev_block][(int)c];
    if (_cur_block == -1 || _cur_block != prev_block)
    {
        assert(read_block(prev_block) == 0);
        _cur_block = prev_block;
    }
    // int end_pos = pos_not_include_self -
    for (int i = 0; i <= remain_chars;  ++i)
    {
        if (_read_buffer[i] == c)
        {
            count ++;
        }
    }
    return count;
}

int CBwtSearch::read_block(int block_pos)
{
    int offset = block_pos * MAX_BLOCK_SIZE ;
    int ret = _bwt_file->read_from_position(offset, _read_buffer, MAX_READ_BUFFER_SIZE);
    (void)ret;
    if (block_pos == _block_num - 2)
    {
        assert(ret == _bwt_file_size % MAX_READ_BUFFER_SIZE);
    }
    else
    {
        assert(ret == MAX_READ_BUFFER_SIZE);
    }
    return 0;
}
int CBwtSearch::find_whole_string(const std::string& search_str, int first, int last)
{
    for (int i = first; i <= last; ++i)
    {
        string result = "";
        int cur_pos = i;
        int next_pos = -1;
        char ch;
        while (1)
        {
            backward(cur_pos, next_pos, ch);
            result.insert(0, 1, ch);
            cur_pos = next_pos;
            if (ch == '[')
                break;
        }
        result += search_str[0];
        cur_pos = i;
        while (1)
        {
            forward(cur_pos, next_pos, ch);
            cur_pos = next_pos;
            if (ch == '[') // FIXME,may when cur pos>=file size?
                break;
            result += ch;
        }
        size_t  left = result.find ('[');
        size_t  right = result.find (']');
        if (left != 1 || right == string::npos )
        {
            printf ("why %s\n", result.c_str());
            continue;
        }
        int id = 0;
        for (int j = left + 1; j < (int)right; j ++)
        {
            id = id * 10 + result[j] - '0';
        }
        _result[id] = result.substr(right + 1);
    }
    return 0;
}

void CBwtSearch::backward(int cur_pos, int& next_pos, char& c)
{
    assert(_bwt_file->read_from_position(cur_pos, &c, 1) == 1);
    int occ_n = occ(c, cur_pos);
    next_pos = occ_n + char_bucket[(int)c] - 1;
    return;
}

void CBwtSearch::forward(int cur_pos, int& next_pos, char& c)
{
    next_pos = inverse_occ(c, cur_pos);
    for (int i = MAX_CHAR_COUNTS - 1; i >= 0; i--)
    {
        if (char_bucket[i] <= next_pos)
        {
            c = i;
            return;
        }
    }
    return;
}

int CBwtSearch::inverse_occ(char c, int l_pos)
{
    int occ_n = l_pos - char_bucket[(int)c];
    int low = 0;
    int high = _block_num - 1;
    while (low <= high)
    {
        int mid = (low + high) >> 1;
        int tmp_count = block_index[mid][(int)c];
        if (tmp_count <= occ_n)
        {
            low = mid + 1;
        }
        else
        {
            high = mid - 1;
        }
    }
    read_block(high);
    int pos = high * MAX_BLOCK_SIZE;
    for (int i = 0 ;i < MAX_BLOCK_SIZE; i ++)
    {
        if (_read_buffer[i] == c)
        {
            pos += i;
        }

    }
    return pos;
}
