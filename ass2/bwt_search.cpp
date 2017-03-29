#include "bwt_search.h"
#include <string.h>
#include <stdio.h>
#include "assert.h"
const char* MAGIC_STRING = "deadbeaf";
// #define MAGIC_NUMBER 0xdeadbeef
using namespace std;


int CBwtSearch::init(const std::string& bwt, const std::string& index)
{
    _bwt_file = new COpFile(bwt, O_RDONLY);
    assert(_bwt_file != NULL);
    _index_file = new COpFile(index, O_RDWR | O_CREAT);
    assert(_index_file != NULL);
    return 0;
}
int CBwtSearch::build_index()
{
    memset(this->char_bucket, 0, sizeof(this->char_bucket));

    int ret = _index_file->has_content();
    if (ret < 0)
    {
        assert(0);
    }
    if (ret)
    {
        read_index_file();
    }
    else
    {
        create_index_file();

    }

    return 0;
}

void CBwtSearch::read_index_file()
{
    int ret = _index_file->read_file(_read_buffer, strlen(MAGIC_STRING));
    assert(ret ==  strlen(MAGIC_STRING)  && strncmp(_read_buffer, MAGIC_STRING, strlen(MAGIC_STRING)) == 0);
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
    int block = 0;
    while (1)
    {
        if (block > 0)
        {
            for (int i = 0; i < 128; i ++)
            {
                block_index[block][i] = block_index[block - 1][i];
            }
        }
        _read_len = _bwt_file->read_file(_read_buffer, sizeof(_read_buffer));
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
        char_bucket[i] = sum;
        sum += tmp[i];
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
