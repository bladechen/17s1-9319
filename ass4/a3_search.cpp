#include <string.h>
#include <algorithm>    // std::sort
#include <unistd.h>
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
// const std::string INDEX_LIST = "index_list";
const std::string INDEX_FILE = "index_file.";
const std::string TMP_FILE= "tmp.";
const std::string STOP_WORDS_FILE = "stop.words";

#define INDEX_INDEX_FILE  "index"
#define INDEX_WORD_SET_FILE "wordset"
#define INDEX_ID_2_FILE "file_2_id"
#define INDEX_WORD_POS "word_pos"
#define INDEX_WORD_2_FILE_2_COUNT "word_2_file_2_count"
// #define MAGIC_NUMBER 0xdeadbeef
using namespace std;
// #define DEBUG 1

static char buf[4096 * 3];
#ifndef DEBUG
#undef assert
#define assert (void)
#endif

// inline std::string get_
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
static inline std::string to_lower(const char* c)
{
    string r = "";
    for (int i = 0; c[i] != 0; i++)
    {
        if (c[i] >= 'A' && c[i] <= 'Z')
        {
            r += c[i] - 'A' + 'a';
            continue;
        }
        else
            r += c[i];
    }
    return r;
}
int CA3Search::init(const std::string& target_folder, const std::string& index_folder)
{
    _target_dir = new COpDir(target_folder);
    _index_dir = new COpDir(index_folder);
    assert(_target_dir != NULL);
    assert(_index_dir != NULL);
    return check_index();
}
bool CA3Search::is_valid_index(const std::string& index)
{
    COpFile index_fp(_index_dir->get_dir_str() + "/" + INDEX_FILE + index, "r");
    int ret = index_fp.read_line(_read_buffer, sizeof(_read_buffer));
    if (ret == 0 || strcmp(_read_buffer, _target_dir->get_dir_str().c_str()) != 0)
    {
        return 0;
    }
    while (index_fp.read_line(_read_buffer, sizeof(_read_buffer)))
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

int CA3Search::read_index()
{
    if (index_file_list.size() != 5)
    {
        return -1;
    }
    int flag = 0;
    for (int i = 0; i < (int)index_file_list.size(); ++ i)
    {

        if (index_file_list[i] == string(INDEX_INDEX_FILE))
        {
            flag |= 1;
        }
        else if (index_file_list[i] == string(INDEX_WORD_SET_FILE))
        {
            flag |= 2;
        }
        else if (index_file_list[i] == string(INDEX_ID_2_FILE))
        {
            flag |= 4;
        }
        else if (index_file_list[i] == string(INDEX_WORD_POS))
        {
            flag |= 8;

        }
        else if (index_file_list[i] == string(INDEX_WORD_2_FILE_2_COUNT))
        {
            flag |= 16;
        }
    }

    if (flag != 31)
    {
        return -1;
    }

    read_file_2_map(wordset, _index_dir->get_dir_str() + "/" + INDEX_FILE + INDEX_WORD_SET_FILE);


    COpFile word_pos(_index_dir->get_dir_str() + "/" + INDEX_FILE + INDEX_WORD_POS, "r");
    wordid_pos.clear();
    int total_word = wordset.size();
    for (int i = 0; i < total_word; i ++)
    {
        assert(word_pos.read_file(_read_buffer, 4) == 4);
        int tmp;
        memcpy(&tmp, _read_buffer, 4);
        wordid_pos.push_back(tmp);
    }
    assert(word_pos.read_file(_read_buffer, 4) == 0);

    read_file_2_map(file_2_id, _index_dir->get_dir_str() + "/" + INDEX_FILE + INDEX_ID_2_FILE);
    id_2_file.clear();
    for (std::map<std::string, int>::iterator it = file_2_id.begin(); it != file_2_id.end(); ++it)
    {
        assert(id_2_file.find(it->second) == id_2_file.end());
        id_2_file[it->second] = it->first;
    }

    return 0;

}
int CA3Search::check_index()
{
    if (_index_dir->file_exist(string(INDEX_FILE) + INDEX_INDEX_FILE) == 0 || is_valid_index(INDEX_INDEX_FILE) == 0)
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
    while (file.read_line(_read_buffer, sizeof(_read_buffer)))
    {
        stop_words.insert(to_lower(_read_buffer));
    }
    return 0;
}
int CA3Search::build_index()
{
    assert(_index_dir->get_dir_str() != "");
    _index_dir->clean_dir();
    // string rm_str = (string("exec rm -f " ) + _index_dir->get_dir_str() + "/" + INDEX_FILE + "*");
    // system(rm_str.c_str());
    int ret = 0;
    ret = load_stop_words(STOP_WORDS_FILE);
    assert(ret == 0);

#ifdef DEBUG
    for (set<string>::iterator i = stop_words.begin() ; i != stop_words.end(); i ++ )
    {
        printf ("%s\n", (*i).c_str());
    }
    printf ("reading stop words end ###########\n\n\n");
#endif


    vector<std::string> v;
    ret = _target_dir->read_filelist(v);
    (void) ret;
    assert(ret == 0);
    sort(v.begin(), v.end());
    for (int i = 0; i < (int) v.size(); i ++)
    {
        file_2_id[v[i]] = i;
        id_2_file[i] = v[i];
#ifdef DEBUG
        printf ("%s %d\n", v[i].c_str(), i);
#endif
    }
    map<std::string, int> word_2_count;
    save_map_2_file(file_2_id, _index_dir->get_dir_str() + "/" + INDEX_FILE + "file_2_id");
    for (int i = 0; i < (int)v.size(); i ++)
    {
        word_2_count.clear();
        assert(read_original_file(word_2_count, _target_dir->get_dir_str() + "/" + v[i]) == 0);
        save_map_2_file(word_2_count, _index_dir->get_dir_str() + "/"+ TMP_FILE +  "wordcount." + v[i]   );
    }
    v.clear();


    int i = 0;
    for (map<std::string, int>::const_iterator it  = wordset.begin(); it != wordset.end(); it ++)
    {
        wordset[it->first] = i ++;
    }
    save_map_2_file(wordset, _index_dir->get_dir_str() + "/" + INDEX_FILE + "wordset");

    generate_word_count_index();
    return 0;
}
void CA3Search::generate_word_count_index()
{
    vector<COpFile*> op_files;
    for (map<std::string, int>::const_iterator it  =file_2_id.begin(); it != file_2_id.end(); it ++)
    {

#ifdef DEBUG
        // printf ("%d %d %s %s\n", op_files.size(), it->second, it->first.c_str(), id_2_file[it->second].c_str());
#endif
        assert((int)op_files.size() == it->second);
        assert(it->first == id_2_file[it->second]);
        string path = _index_dir->get_dir_str() + "/" + TMP_FILE +"wordcount." +it->first;
#ifdef DEBUG
        // printf ("%d %d %s %s\n", op_files.size(), it->second, it->first.c_str(), id_2_file[it->second].c_str());
#endif

        op_files.push_back(new COpFile(path, "r"));
#ifdef DEBUG
        // printf ("generate_word_count_index: %s %d\n", path.c_str(), it->second);
#endif

    }
    unsigned int offset = 0;
    vector<pair<int, int> > cur_wordid_2_count;
    cur_wordid_2_count.resize(2001);
    static bool end_of_file[2001];
    static bool has_word[2001];
    memset(end_of_file, 0, sizeof(end_of_file));
    memset(has_word, 0, sizeof(has_word));
    COpFile word_2_file_2_count(_index_dir->get_dir_str() + "/" + INDEX_FILE + "word_2_file_2_count", "w");
    wordid_pos.resize(wordset.size() );
    for (int i = 0; i < (int)wordid_pos.size(); i ++)
    {
        wordid_pos[i] = -1;
    }
    while (1)
    {
        for (int i = 0; i < (int)op_files.size() ;++ i )
        {
            if (end_of_file[i] == 0 && has_word[i] == 0)
            {
                // pair<string, int> tmp;
                bool success = parse_one_line((cur_wordid_2_count[i]), op_files[i]);
                if (success == false) // end of file
                {
                    end_of_file[i] = 1;
                }
                else
                {
                    has_word[i] = 1;
                }
            }

        }
        int min = -1;
        for (int i = 0; i <(int) op_files.size(); i ++)
        {
            if (end_of_file[i] == 1)
            {
                continue;
            }
            else
            {
                assert(has_word[i] == 1);
                if (cur_wordid_2_count[i].first < min || min == -1)
                {
                    min = cur_wordid_2_count[i].first;

                }

            }

        }
        if (min == -1)
        {
            break;
        }
        int length = 0;
        memcpy(_write_buffer, &min, 4); // the word id

        length = 4;
        for (int i = 0; i <(int) op_files.size(); i ++)
        {
            if (has_word[i] && cur_wordid_2_count[i].first == min)
            {
                unsigned short s = i;
                assert(s == i);
                memcpy(_write_buffer + length, &s, 2);
                length += 2;

                memcpy(_write_buffer + length, &(cur_wordid_2_count[i].second), 4);
                length += 4;
                has_word[i] = 0;

            }

        }
        assert(length > 4);
        assert(word_2_file_2_count.write_file((char*)& length, 4) == 4);
        // buf[length] = '\n';
        assert(length == word_2_file_2_count.write_file(_write_buffer, length));
        assert(wordid_pos[min] == -1);
        wordid_pos[min] = offset;
        offset += length + 4;
    }

    for (int i = 0; i < (int)op_files.size(); ++ i)
    {
        delete op_files[i];
    }

    COpFile word_pos(_index_dir->get_dir_str() + "/" + INDEX_FILE + "word_pos", "w");

    for (int i = 0; i < (int)wordid_pos.size(); ++ i)
    {
        assert( 4 == word_pos.write_file((char*)(&(wordid_pos[i])), 4));

    }
    COpFile index_op(_index_dir->get_dir_str() + "/" + INDEX_FILE  + INDEX_INDEX_FILE, "w");
    index_op.write_file(_target_dir->get_dir_str().c_str(),   _target_dir->get_dir_str().length());
    index_op.write_file("\n", 1);
    index_op.write_file(INDEX_INDEX_FILE, strlen(INDEX_INDEX_FILE));
    index_op.write_file("\n", 1);
    index_op.write_file(INDEX_WORD_SET_FILE, strlen(INDEX_WORD_SET_FILE));
    index_op.write_file("\n", 1);

    index_op.write_file(INDEX_ID_2_FILE, strlen(INDEX_ID_2_FILE));
    index_op.write_file("\n", 1);

    index_op.write_file(INDEX_WORD_POS, strlen(INDEX_WORD_POS));
    index_op.write_file("\n", 1);

    index_op.write_file(INDEX_WORD_2_FILE_2_COUNT, strlen(INDEX_WORD_2_FILE_2_COUNT));
    index_op.write_file("\n", 1);
    assert(system ((string("rm -r ") + _index_dir->get_dir_str()  + "/" + TMP_FILE + "*").c_str()) || 1 == 1);

    // _index_dir->clean_dir(TMP_FILE);
    // delete _target_dir;
    // _target_dir = NULL;
    return ;

}

bool CA3Search::parse_one_line(pair<int, int>& ret, COpFile* op)
{
    static char buf1[1001];
    bool res = op->read_line(buf, 1000);
    if (res == 0)
    {
        return 0;
    }
    else
    {
        int count = 0;
        sscanf(buf, "%s %d", buf1, &count);
        assert(wordset.find(buf1) != wordset.end());
        ret.first = wordset[buf1];
        ret.second = count;
#ifdef DEBUG
        // printf ("parse_one_line %s:%d %d\n", buf1,ret.first, count);
#endif
        return 1;
    }
}

void CA3Search::save_map_2_file(const std::map<std::string, int>& word_2_count, const std::string& wordcount_file)
{

    COpFile file (wordcount_file, "w");
    for (map<string, int>::const_iterator it = word_2_count.begin();
         it != word_2_count.end(); it ++)
    {
        assert(it->second >= 0);
        int len = snprintf(buf, 4095, "%s %d\n", it->first.c_str(), it->second );
        assert(len < 4095);

        assert(file.write_file(buf, len) > 0);

    }

}
void CA3Search::read_file_2_map(std::map<std::string, int>&m, const std::string& filename)
{
    m.clear();
    static int count = 0;
    COpFile file (filename, "r");
    while (file.read_line(buf, 4095))
    {

        int i = 0;
        count  = 0;
        for (i = strlen(buf) - 1; i>= 0; i --)
        {
            if (buf[i] >= '0' && buf[i] <= '9')
            {
                continue;
                // num = num * 10 + buf[i] - '0';
            }
            else
            {
                for (int j = i + 1; j < (int)strlen(buf) ;j ++)
                {
                    count = count * 10 + buf[j] - '0';
                }
                buf[i] = 0;
                break;
            }

        }
        // printf("###%s %d\n", buf, num);
        m[buf] = count;
    }
    return;


}
void CA3Search::insert_new_word(map<std::string, int>& word_2_count, const std::string& s)
{
    // // assert(tmp.length() < 4096)
    strncpy(buf, s.c_str(), s.length());
    buf[s.length()] = 0;
    int k = stem(buf, 0, strlen(buf) - 1);
    string tmp(buf, k + 1);

    if (stop_words.find(tmp) != stop_words.end())
    {
        return;
    }
    if (word_2_count.find(tmp) != word_2_count.end())
    {
        word_2_count[tmp] ++;
    }
    else
    {
        word_2_count[tmp] = 1;
    }
    // printf ("%s %s\n", s.c_str(), buf);
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
#ifdef DEBUG
        _read_buffer[_read_len] = 0;
        // printf ("%s: %s\n", original_file.c_str(), _read_buffer);
#endif


        // word  = "";
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
                    word = "";
                    has_symbol = 0;
                    continue;
                }
                if (has_symbol == 1)
                {
                    vector<string> vec = split(tmp, "'");
                    // assert(vec.size() == 2);
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
        // assert(vec.size() == 2);
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

void CA3Search::load_concept_dict()
{
    COpFile f("./concept_search_dict", "r");
    while (f.read_line(_read_buffer, sizeof(_read_buffer)))
    {
        vector<string> tmp = split(_read_buffer, " ");
        concept_relation[tmp[0]] = tmp;

    }
    return ;
}
bool CA3Search::read_word_count(const std::string& word, int *file_count , COpFile* fp)
{
    int total_file = file_2_id.size();
    memset (file_count, 0, total_file * 4);
    if (wordset.find(word) == wordset.end())
    {
        // all the files (the filenames only) that contain ALL input query strings (i.e., a boolean AND search)
        // output just one newline character if there are no matches for the query.
        return 0;

    }
    size_t pos = wordid_pos[wordset[word]];
    int total_len;
    _read_len = fp->read_from_position(pos , (char*)&total_len, 4);
    assert(_read_len == 4);
    assert(total_len > 4 && total_len < 20000);
    assert(total_len == fp->read_from_position(pos + 4, _read_buffer, total_len));
    int word_id;
    memcpy(&word_id, _read_buffer, 4);
    (void) word_id;
    assert(word_id == wordset[word]);
    assert((total_len - 4) % 6 == 0);
#ifdef DEBUG
    printf ("current finding key: %s, pos: %d, total count: %d\n",word.c_str(), pos, (total_len - 4) / 6);
#endif


    bool find = 0;
    int cnt = (total_len - 4) / 6;
    for (int j = 0; j < cnt; j ++)
    {
        int file_id = *(unsigned short*)(_read_buffer + 4 + 6 * j);
        int  count = *(int *)(_read_buffer + 4+6 * j + 2);
        assert(file_id >= 0 && file_id < (int)id_2_file.size());
        assert(file_count[file_id] == 0);
        assert(count > 0);
        file_count[file_id] += count;
        find = 1;
#ifdef DEBUG
        printf ("file %s, count %d\n", id_2_file[file_id].c_str(), count);

#endif
    }
#ifdef DEBUG
    printf ("###########################\n\n");

#endif

    return find;

}
void CA3Search::run(const std::vector<std::string>& query_strings, double c_value)
{
    static int contain[2001];
    static int tmp_count[2001];
    memset(contain, 0, sizeof(contain));

    int file_num = file_2_id.size();
    vector<std::string> keys;
    // vector<std::string> stemmming_strings ;
    for (int i = 0; i < (int)query_strings.size(); ++ i)
    {
        string tmp  = to_lower(query_strings[i].c_str());
        strncpy(buf, tmp.c_str(), 4095);
        buf[tmp.length()] = 0;
        int k = stem(buf, 0, strlen(buf) - 1);
        tmp = string(buf, k + 1);
        // stemmming_strings.push_back(tmp);
        keys.push_back(tmp);
    }

    vector<double> result; // index is the file id
    result.resize(id_2_file.size());
    for (int i = 0; i < (int)result.size(); ++ i)
    {
        result[i] = 0;

    }
    COpFile word_2_file_2_count(_index_dir->get_dir_str() + "/" + INDEX_FILE + INDEX_WORD_2_FILE_2_COUNT, "r");

    if (c_value == 0)
    {
        for (int i = 0; i < (int)keys.size(); i ++)
        {

            bool find = this->read_word_count(keys[i], tmp_count, &word_2_file_2_count);
            if (find == 0)
            {
                printf ("\n");
                return;
            }
            else
            {
                for (int j = 0; j < file_num ; ++ j)
                {
                    if (tmp_count[j] != 0)
                    {
                        contain[j] |= (1 << i);
                        result[j] += tmp_count[j];
// #ifdef DEBUG
                        // printf ("%s %d\n", id_2_file[j].c_str(), tmp_count[j]);
// #endif
                    }
                }
            }
        }
    }
    else
    {

        load_concept_dict();
        static bool h[2001];
        for (int i = 0; i < (int)keys.size(); ++i)
        {

            vector<std::string> concept_term;
            find_concept_term(keys[i], concept_term);
            // memset(h, 0, sizeof(h));
            this->read_word_count(keys[i], tmp_count, &word_2_file_2_count);
            for (int j = 0; j < file_num ; ++ j)
            {
                if (tmp_count[j] != 0)
                {
// #ifdef DEBUG
//                     printf ("file %s, find %s, with count: %d\n", id_2_file[j].c_str(), keys[i].c_str(), tmp_count[j]);
// #endif
                    contain[j] |= (1 << i);
                    result[j] += tmp_count[j];
                }
            }
            for (int k = 0; k < (int)concept_term.size(); ++ k)
            {

                memset(h, 0, sizeof(h));
#ifdef DEBUG
                printf("following is concept search [%s]\n", concept_term[k].c_str());
#endif
                bool b = this->read_word_count(concept_term[k], tmp_count, &word_2_file_2_count);
                if (b == 0) continue;
                for (int j = 0; j < file_num; j ++)
                {
                    // not find key term, use concept search
                    // if ((contain[j] & (1 << i)) == 0 && tmp_count[j])
                    if (tmp_count[j])
                    {
#ifdef DEBUG
                        printf ("file %s, find [%s]'s concept [%s] , with count: %d\n", id_2_file[j].c_str(), keys[i].c_str(), concept_term[k].c_str(),tmp_count[j]);
#endif

                        result[j] += c_value * tmp_count[j];
                        h[j] = 1;
                    }
                }
                for (int j = 0; j < file_num; ++ j)
                {
                    if (h[j] == 1)
                    {
                        // assert((contain[j] & (1 << i)) == 0);
                        contain[j]  |= ( 1<< i);
                    }
                }
            }
        }
    }
    output_result(result, contain, keys.size());

    return;
}

void CA3Search::find_concept_term(const std::string& key, std::vector<std::string>& concepts)
{
    concepts.clear();
    if (concept_relation.find(key) != concept_relation.end())
    {
        const vector<string>& tmp = concept_relation[key];
        for (int i = 1 ;i < (int)tmp.size(); ++ i)
        {
            concepts.push_back(tmp[i]);
#ifdef DEBUG
            printf ("find_concept_term %s -> %s\n", key.c_str(), tmp[i].c_str());
#endif

        }
    }
    return;
}
bool CA3Search::sort_func(S l,  S r)
{
    if (l.count != r.count && (l.count >= 0.01 && r.count>=0.01))
    {
        return (r.count < l.count);
    }
    else
    {
        return l.file_id < r.file_id;
    }

}
void CA3Search::output_result(const std::vector<double>& fileid_count, int contain[], int key_size)
{
    vector<S> r;
    for (int i = 0; i < (int)fileid_count.size(); i ++)
    {
        if (fileid_count[i] > 0 && (contain[i] + 1== (1 << key_size)))
        {
#ifdef DEBUG
            printf ("file: %s, values: %lf\n", id_2_file[i].c_str(), fileid_count[i]);
#endif
            r.push_back(S(i, fileid_count[i]));
        }

    }
    std::sort(r.begin(), r.end(), this->sort_func);

    if (r.size() == 0)
    {
        printf ("\n");
    }

    for (int i = 0; i < (int)r.size(); i ++)
    {
        // printf("%s %d\n", id_2_file[r[i].file_id].c_str(), (int)r[i].count);
        printf("%s\n", id_2_file[r[i].file_id].c_str());
    }
    return ;
}
