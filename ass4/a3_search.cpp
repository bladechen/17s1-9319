#include <string.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <stdio.h>
#include <string.h>
#include "a3_search.h"
#include "assert.h"
extern int stem(char * p, int i, int j);
const std::string  MAGIC_STRING = "deadbeaf";
const std::string INDEX_FILE = "index_file";
const std::string STOP_WORDS_FILE = "stop_words";
// #define MAGIC_NUMBER 0xdeadbeef
using namespace std;
#define DEBUG 1

#ifndef DEBUG
#undef assert
#define assert (void)
#endif

// http://stackoverflow.com/questions/236129/split-a-string-in-c
static std::vector<std::string> split(const std::string& text, const std::string& delims)
{
    std::vector<std::string> tokens;
    std::size_t start = text.find_first_not_of(delims), end = 0;

    while((end = text.find_first_of(delims, start)) != std::string::npos)
    {
        tokens.push_back(text.substr(start, end - start));
        start = text.find_first_not_of(delims, end);
    }
    if(start != std::string::npos)
        tokens.push_back(text.substr(start));

    return tokens;
}
static std::string to_lower(char* c)
{
    string r = "";
    for (int i = 0; c[i] != 0; i++)
    {
        r += c[i];
    }
    return r;
}
// static inline lower)
int CA3Search::init(const std::string& target_folder, const std::string& index_folder)
{
    _target_dir = new COpDir(target_folder);
    _index_dir = new COpDir(index_folder);
    assert(_target_dir != NULL);
    assert(_index_dir != NULL);
    // _seperator[','] = 1;
    // _seperator['.'] = 1;
    // _seperator[';'] = 1;
    // _seperator['\n'] = 1;
    // _seperator['\r'] = 1;
    // _seperator[','] = 1;
    // _seperator[','] = 1;
    // _seperator[','] = 1;
    return check_index();
}
bool CA3Search::is_valid_index(const std::string& index)
{
    COpFile index_fp(_index_dir->get_dir_str() + "/" + index, "r");
    int ret = index_fp.read_line(_read_buffer, sizeof(_read_buffer));
    if (ret == 0 || strcmp(_read_buffer, MAGIC_STRING.c_str()) != 0)
    {
        return 0;
    }
    while (index_fp.read_file(_read_buffer, sizeof(_read_buffer)))
    {
        index_file_list.push_back(_read_buffer);
    }
#ifdef DEBUG
    printf(">>>>>>>>>>>>>>>>> file list \n");
    for (int i  = 0; i < (int) index_file_list.size();i ++)
    {
        printf ("%s\n", index_file_list[i].c_str());
    }
    printf ("<<<<<<<<<<<<<<<<<\n\n");
#endif
    return 1;

}
int CA3Search::check_index()
{
    if (_index_dir->file_exist(INDEX_FILE) == 0 || is_valid_index(INDEX_FILE) == 0)
    {
        return build_index();
    }
    if (read_index())
    {
        return build_index();
    }
    return 0;
}

int CA3Search::load_stop_words(const std::string& filename)
{
    COpFile file (filename, "r");
    while (file.read_file(_read_buffer, sizeof(_read_buffer)))
    {
        stop_words.insert(to_lower(_read_buffer));
    }
    return 0;
}
int CA3Search::build_index()
{
    int ret = 0;
    ret = load_stop_words(STOP_WORDS_FILE);
    assert(ret == 0);
    vector<std::string> v;
    ret = _target_dir->read_filelist(v);
    (void) ret;
    assert(ret == 0);
    for (int i = 0; i < (int) v.size(); i ++)
    {
        file_2_id[v[i]] = i;
    }
    for (int i = 0; i < (int)v.size(); i ++)
    {
        map<std::string, int> word_2_count;
        word_2_count.clear();
        assert(read_original_file(word_2_count, _target_dir->get_dir_str() + "/" + v[i]) == 0);
        assert(write_wordcount_2_file(word_2_count, _index_dir->get_dir_str() + "/wordcount." + v[i]   ) == 0);

    }

    return 0;
}
void CA3Search::insert_new_word(map<std::string, int>& word_2_count, const std::string& s)
{
    // string tmp = s;
    static char buf[4096];
    // // assert(tmp.length() < 4096)
    strncpy(buf, s.c_str(), 4095);
    int k = stem(buf, 0, strlen(buf));
    string tmp(buf, k);

    if (word_2_count.find(tmp) != word_2_count.end())
    {
        word_2_count[tmp] ++;
    }
    else
    {
        word_2_count[tmp] = 1;
    }
    wordset[tmp] = 1;

}
int CA3Search::read_original_file(map<std::string, int>& word_2_count, const std::string& original_file)
{
    word_2_count.clear();
    COpFile file (original_file,  "r");
    string word = "";
    bool has_symbol = 0;
    while (1)
    {
        _read_len = file.read_file(_read_buffer, sizeof(_read_buffer));
        assert(_read_len >= 0);


        word  = "";
        for (int i = 0; i < _read_len; ++ i)
        {
            char cur_char = _read_buffer[i];
            if (cur_char >= 'A' && cur_char <= 'Z')
            {
                word += cur_char + 'a' - 'A';
            }
            else if (cur_char >= 'a' && cur_char <= 'z')
            {
                word +=  cur_char;
            }
            else if (cur_char == '\'')
            {
                word += cur_char;
                has_symbol = 1;
            }
            else
            {
                if (word.length() == 0)
                {
                    assert(has_symbol  == 0);
                    continue;
                }
                string tmp = word;
                // deal with 'oh', or "i'am"
                if (stop_words.find(tmp) != stop_words.end())
                {
                    continue;
                }
                if (has_symbol == 1)
                {
                    vector<string> vec = split(tmp, "'");
                    assert(vec.length() == 2);
                    for (int i = 0; i < (int)vec.size(); i ++)
                    {
                        if (stop_words.find(vec[i]) != stop_words.end())
                        {
                            continue;
                        }
                        insert_new_word(word_2_count, vec[i]);
                    }
                }
                else
                {
                    insert_new_word(word_2_count, tmp);

                }
                word = "";
                has_symbol = 0;

            } // end of the delimiter
        }

        if (_read_len < (int)sizeof(_read_buffer))
        {
            break;
        }
    }

    if (word.length() == 0)
    {
        return 0;
    }

    string tmp = word;
    // deal with 'oh', or "i'am"
    if (stop_words.find(tmp) != stop_words.end())
    {
        return 0;
    }
    if (has_symbol == 1)
    {
        vector<string> vec = split(tmp, "'");
        assert(vec.length() == 2);
        for (int i = 0; i < (int)vec.size(); i ++)
        {
            if (stop_words.find(vec[i]) != stop_words.end())
            {
                continue;
            }
            insert_new_word(word_2_count, vec[i]);
        }
    }
    else
    {
        insert_new_word(word_2_count, tmp);

    }

    // while (file.read_file(_read_buffer, sizeof(_read_buffer)))
    return 0;
}
void CA3Search::run(const std::vector<std::string>& query_strings, double c_value)
{
    return;
}
