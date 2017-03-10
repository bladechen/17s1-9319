#include "rle.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
using namespace std;

CRle::CRle(const std::string& readin_file,
           const std::string& writeout_file)
{
    is_operating_count = 0;
    _processed.clear();
    _infile = NULL;
    _outfile = NULL;
    _inbuf_len = 0;
    _outbuf_len = 0;
    _infile = new COpFile(readin_file, O_RDONLY);

    if (writeout_file != "")
    {
        _outfile = new COpFile(writeout_file, O_WRONLY|O_CREAT|O_TRUNC);
    }


    return;
}
CRle::~CRle()
{
    if (_infile != NULL)
    {
        delete _infile;
        _infile = NULL;
    }
    if (_outfile != NULL)
    {
        delete _outfile;
        _outfile = NULL;
    }
}
/**
 * @brief: rle decode in following steps,
 * 1. read encoded content from file
 * 2. decode() function decode the content and  save in _processed
 * 3. dump_processed2encoded() generate output from _processed into _output buffer,
 * 4. dump2file() write out _output to file.
 */

void CRle::run_decode()
{
    while (1)
    {
        _inbuf_len = _infile->read_file(_inbuf, sizeof(_inbuf));

        assert(_inbuf_len >= 0);
        decode(_inbuf, _inbuf_len);
        while (1)
        {
            dump_processed2decoded(1);
            dump2file();
            if (_processed.size() <= 1)
            {
                break;
            }
        }
        if ((size_t)_inbuf_len < sizeof(_inbuf)) //finish reading the input file
        {
            break;
        }
    }
    if (is_operating_count == true) // the last bytes of input file is a count, 3 should be added to correct the count
    {
        assert(_processed.size() > 0);
        _processed[_processed.size() - 1].second += 3;
    }
    while (1)
    {
        dump_processed2decoded(0);
        dump2file();
        if (_processed.size() == 0)
        {
            break;
        }

    }
    return;
}

/**
 * @brief:  decrease item.count in _processed by 1, and put the char into _output
 *
 * @param:  not_end
 */
void CRle::dump_processed2decoded(bool not_end)
{
    static int count = 0;
    for (int i = 0; i < (int)_processed.size() - not_end ; i ++)
    {
        if (_outbuf_len == sizeof(_output))
        {
            break;
        }
        int j = 0;
        int tmp_count = 0;
        if (_outfile != NULL)
        {
            if (count < _processed[i].second)
            {
                count = _processed[i].second;
            }
            for (j = 0; j < _processed[i].second; j ++)
            {
                _output[_outbuf_len ++ ] = _processed[i].first;
                tmp_count ++;
                if (_outbuf_len == sizeof(_output))
                {
                    break;
                }
            }
        }
        else
        {
            count  = _processed[i].second;
            tmp_count = count;
        }
        assert(tmp_count <= _processed[i].second );
        _processed[i].second -= (tmp_count);
        if (_processed[i].second == 0)
        {
            output_stdout(make_pair(_processed[i].first, count));
            count = 0;
        }
        if (_outbuf_len == sizeof(_output))
        {
            break;
        }
    }
    vector<std::pair<char, int> > tmp = _processed;
    _processed.clear();
    for (size_t i = 0; i < tmp.size() - not_end; i ++)
    {
        if (tmp[i].second != 0)
        {
            _processed.push_back(tmp[i]);
        }
    }
    if (not_end == 1) _processed.push_back(tmp[tmp.size() -1]);
    return;
}


/**
 * @brief: decode the RLE, the last item in _processed  maybe concreted with following chars(from reading files) if is_operating_count is set true
 *
 * @param:  buf the content need to be decoded
 * @param:  len
 */
void CRle::decode(char * buf, int len)
{
    int prev_char = -1;
    int char_count = 0;
    if (_processed.size() > 0)
    {
        prev_char = _processed[_processed.size() -1 ].first;
        char_count = _processed[_processed.size() -1 ].second;
    }

    for (int i = 0; i < len; i ++)
    {
        if ((buf[i] & 0x80) != 0)
        {
            if (is_operating_count == 0)
            {
                char_count = 0;
            }
            is_operating_count = 1;
            decode_count(char_count, buf[i]);

            _processed[_processed.size() -1].second = char_count ;

        }
        else
        {
            if (is_operating_count)
            {
                assert(prev_char > 0);
                _processed[_processed.size() -1].second = char_count + 3;
            }

            _processed.push_back(make_pair(buf[i],  1));

            char_count = 0;
            prev_char = buf[i];

            is_operating_count = 0;

        }

    }

    return;
}

// static int total = 0;
/**
 * @brief: rle encode in following steps,
 * 1. read content from file
 * 2. encode() function save the content in _processed
 * 3. dump_processed2encoded() generate output encoded content into _output buffer,
 * 4. dump2file() write out _output to file.
 */
void CRle::run_encode()
{
    while (1)
    {
        _inbuf_len = _infile->read_file(_inbuf, sizeof(_inbuf));
        // total += _inbuf_len;

        assert(_inbuf_len >= 0);
        encode(_inbuf, _inbuf_len);
        while (1)
        {

            dump_processed2encoded(1);
            dump2file();
            if (_processed.size() <= 1)
            {
                break;
            }
        }
        if ((size_t)_inbuf_len < sizeof(_inbuf))
        {
            break;
        }
    }
    while (1)
    {
        dump_processed2encoded(0);
        dump2file();
        if (_processed.size()  == 0)
        {
            break;
        }
    }
}


void CRle::dump2file()
{
    if (_outfile == NULL)
    {
        _outbuf_len = 0;
        return;
    }

    int r =_outfile->write_file(_output, _outbuf_len);
    assert(r == _outbuf_len);
    (void) r;
    _outbuf_len = 0;
    return;
}

void CRle::output_stdout(const std::pair<char, int>& item)
{
    if (_outfile != NULL)
    {
        return;
    }
    if (item.second > 2)
    {
        printf ("%c[%d]", item.first, item.second - 3);

    }
    else
    {
        assert(item.second > 0);
        for (int j = 0 ; j < item.second; j ++)
        {
            printf ("%c", item.first);
        }
    }
    return;

}

/**
 * @brief: generate encoded text from _processed,
 * there are 3 cases:
 * 1. {A,1} // output A
 * 2. {A,2} // output AA
 * 3. {A,X}(which X>=3)//output A[X-3]
 *
 * @param:  not_end
 */
void CRle::dump_processed2encoded(bool not_end)
{

    int processed_flag = -1;
    int i = 0;
    // before reaching to the end of file, we can only process the completed chars, the left ones may be consecutive the same with the following ones in the files.
    for (i = 0; i < (int)_processed.size() - not_end; i ++)
    {
        processed_flag  ++;
        assert(_processed[i].second > 0);
        if (_processed[i].second ==  1)
        {
            _output[_outbuf_len ++ ] = _processed[i].first;

        }
        else if (_processed[i].second == 2)
        {
            _output[_outbuf_len ++ ] = _processed[i].first;
            _output[_outbuf_len ++ ] = _processed[i].first;

        }
        else
        {
            _output[_outbuf_len ++ ] = _processed[i].first;
            int tmp_len = 0;
            encode_count(_processed[i].second - 3 , _output + _outbuf_len, tmp_len);
            _outbuf_len += tmp_len;
        }
        output_stdout(_processed[i]);
        // maximum 5 bytes for one item in _processed to stored back into _output.
        if (_outbuf_len >= (int)sizeof(_output) - 10)
        {
            break;
        }
    }
    std::vector<std::pair<char, int> > tmp = _processed;
    _processed.clear();
    for (int j = processed_flag + 1;j < (int)tmp.size();j ++)
    {
        _processed.push_back(tmp[j]);
    }

    return ;
}

void CRle::encode(char* buf, int len)
{
    int prev_char = -1;
    int char_count = 0;
    int flag = 0; // stands for there is possible following chars can be concreted into the last item in _processed
    if (_processed.size() > 0)
    {
        prev_char = _processed[_processed.size() -1 ].first;
        char_count = _processed[_processed.size() -1 ].second;
        flag = 1 ;
    }
    for (int i = 0; i < len; i ++)
    {
        if (buf[i] == prev_char)
        {
            char_count ++;
        }
        else
        {
            if (char_count >= 1)
            {
                if (flag == 0)
                    _processed.push_back(make_pair(prev_char, char_count));
                else
                {
                    _processed[_processed.size() - 1].second = char_count;
                    flag = 0;
                }
            }
            char_count  = 1;
            prev_char = buf[i];
        }
    }
    if (char_count >= 1)
    {
        if (flag == 0)
            _processed.push_back(make_pair(prev_char, char_count));
        else
        {
            _processed[_processed.size() - 1].second = char_count;
        }
    }


    return;
}

/**
 * @brief: count is a four bytes int, but it will never get bigger than 200*1024*1024 < 2 ^ 28.
 *         so every 7bits represented in one bytes with MSB set to 1
 *         for example: 0x01020304
 *         will be encoded as, 0x81, 0x82, 0x83, 0x84 in the buffer
 *         it is big endian
 *
 * @param:  count , number need to be encoded
 * @param:  buf, the buf where count encoded
 * @param:  len, the encoded length
 */
void CRle::encode_count(int count, char* buf, int & len)
{
    if (count == 0)
    {
        char tmp = 0x80;
        memcpy(buf,  &tmp, 1);
        len ++;
        return;
    }
    char tmp_buf[4];
    while (count > 0)
    {
        char tmp = count & 0x7f;
        tmp |= 0x80;
        count >>= 7;
        memcpy(tmp_buf + len,  &tmp, 1);
        len ++;
    }
    for (int i = 0; i < len; i ++)
    {
        memcpy(buf + i , tmp_buf + len - i - 1, 1);
    }
    return;

}
/**
 * @brief:  for the new comming c, decoded into count.
 * the MSB of 8bits is used for marking it as a count,
 * as it is bigendian, so just move count to left 7 bits, and append the low 7bits of c to count.
 *
 *
 * @param:  count
 * @param:  c
 */
void CRle::decode_count(int& count, char c)
{
    assert(c & 0x80);
    count <<= 7;
    count |= (c & 0x7f);
    return;
}
