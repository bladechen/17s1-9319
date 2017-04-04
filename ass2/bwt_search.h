#ifndef _BWT_SEARCH_H_
#define _BWT_SEARCH_H_
#include <string>
#include <map>
#include <vector>
#include "op_file.h"

#define MAX_CHAR_COUNTS 128
// #define MAX_BLOCK_SIZE (4096 * 16)
#define MAX_BLOCK_SIZE (5)
#define MAX_READ_BUFFER_SIZE (MAX_BLOCK_SIZE)

#define MAX_FILE_SIZE (170)
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

    int init(const std::string& bwt, const std::string& index);

    void run(const std::vector<std::string>& query_strings);
    int build_index();
protected:

    int backward_search(const std::string& search_str, int& first, int& last);
    int find_whole_string(int first, int last);
    void create_index_file();
    void write_index_file();
    void read_index_file();

    int occ(char c, int pos_not_include_self);
    int inverse_occ(char c, int l_pos) ;

    int read_block(int block_pos);
    int find_whole_string(const std::string& search_str, int first, int last);

    void backward(int cur_pos, int& next_pos, char& c);
    void forward(int cur_pos, int & next_pos, char& c);



private:
    COpFile *_index_file;
    COpFile *_bwt_file;

    char _read_buffer[MAX_READ_BUFFER_SIZE];
    int _read_len ;

    int char_bucket[128];
    int block_index[MAX_FILE_SIZE/MAX_BLOCK_SIZE][128];
    int _block_num;

    int _cur_block ; // refer to _read_buffer storing which blocks



    int _bwt_file_size;

    std::map<int, std::string> _result;

    // int char_counts[128];
};
#endif

