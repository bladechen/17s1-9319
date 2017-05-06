#ifndef _A3_SEARCH_H_
#define _A3_SEARCH_H_
#include <string>
#include <map>
#include <set>
#include <vector>
#include "op_file.h"

#define MAX_FILE_SIZE (160*1024*1024+1000)
#define MAX_READ_BUFFER_SIZE (1024*1024*64)
class CA3Search
{
public:
    CA3Search()
    {
        _target_dir = NULL;
        _index_dir = NULL;
    };
    virtual ~CA3Search()
    {
        if (_target_dir != NULL)
        {
            delete _target_dir;
            _target_dir = NULL;
        }
        if (_index_dir != NULL)
        {
            delete _index_dir;
            _index_dir = NULL;
        }

    };

    int init(const std::string& target_folder, const std::string& index_folder);
    void run(const std::vector<std::string>& query_strings, double c_value);
protected:
    int build_index();
    bool is_valid_index(const std::string& index);
    int check_index();
    int read_index();
    int load_stop_words(const std::string& filename);

    int read_original_file(std::map<std::string, int>& word_2_count, const std::string& original_file);
    // int write_wordcount_2_file(const std::map<std::string, int>& word_2_count, const std::string& wordcount_file);
    inline void insert_new_word(std::map<std::string, int>& word_2_count, const std::string& s);

    void save_map_2_file(const std::map<std::string, int>& m, const std::string& filename);
    void read_file_2_map(std::map<std::string, int>&m, const std::string& filename);
    bool parse_one_line(std::pair<int, int>& ret, COpFile* op);
    void generate_word_count_index();
    void output_result(const std::vector<int>& fileid_count);
    void load_concept_dict();

    struct S
    {
        int file_id;
        int count;
        S()
        {
            file_id = 0;
            count = 0;
        }
        S(int a, int b)
        {
            file_id = a;
            count = b;
        }
    };


    static bool sort_func(S l,  S r);
private:
    std::vector<std::string> index_file_list;
    std::map<std::string, int> file_2_id;
    COpDir *_target_dir;
    COpDir *_index_dir;
    char _read_buffer[MAX_READ_BUFFER_SIZE];
    int _read_len ;
    char _write_buffer[MAX_READ_BUFFER_SIZE];

    std::set<std::string> stop_words;


    std::map<std::string, int > wordset;
    std::map<int, std::string> id_2_word;
    std::vector<int> wordid_pos;
    std::map<int, std::string> id_2_file;

    std::map<std::string, std::vector<std::string> > concept_relation;
    // std::map<int,
    // std::
};
#endif

