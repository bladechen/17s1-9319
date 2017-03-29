#ifndef _BWT_SEARCH_H_
#define _BWT_SEARCH_H_
#include <string>
#include <vector>
#include "op_file.h"

#define MAX_BLOCK_SIZE (4096 * 16)
#define MAX_READ_BUFFER_SIZE (MAX_BLOCK_SIZE)
class CBwtSearch
{
    public:
    CBwtSearch(){
        _bwt_file = NULL;
        _index_file = NULL;
    };
    virtual ~CBwtSearch(){
        if (_bwt_file != NULL)
        {
            delete _bwt_file;
            _bwt_file = NULL;
        }
        if (_index_file != NULL)
        {
            delete _index_file;
            _index_file = NULL;
        }
    };

    int CBwtSearch::init(const std::string& bwt, const std::string& index);
    int build_index();
    private:
    int bucket_sort();
    void create_index_file();

    void write_index_file();
    void read_index_file();
    COpFile *_index_file;
    COpFile *_bwt_file;

    char _read_buffer[MAX_READ_BUFFER_SIZE];
    int _read_len ;

    int char_bucket[128];
    int block_index[170* 1024 * 1024/MAX_BLOCK_SIZE][128];
    int _block_num;
};
#endif

