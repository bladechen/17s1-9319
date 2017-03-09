#include <rle.h>
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
        if ((size_t)_inbuf_len < sizeof(_inbuf))
        {
            break;
        }
    }
    if (is_operating_count == true)
    {
        assert(_processed.size() > 0);
        _processed[_processed.size() - 1].second += 3;
    }
    while (1)
    {
        dump_processed2decoded(0);
        // output();
        dump2file();
        if (_processed.size() == 0)
        {
            break;
        }

    }
    return;
}

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


void CRle::decode(char * buf, int len)
{
    int prev_char = -1;
    int char_count = 0;
    if (_processed.size() > 0)
    {
        prev_char = _processed[_processed.size() -1 ].first;
        char_count = _processed[_processed.size() -1 ].second;
    }

    for (int i = 0; i < _inbuf_len; i ++)
    {
        if ((_inbuf[i] & 0x80) != 0)
        {
            if (is_operating_count == 0)
            {
                char_count = 0;
            }
            is_operating_count = 1;
            decode_count(char_count, _inbuf[i]);

            _processed[_processed.size() -1].second = char_count ;

        }
        else
        {
            if (is_operating_count)
            {
                assert(prev_char > 0);
                _processed[_processed.size() -1].second = char_count + 3;
            }

            _processed.push_back(make_pair( _inbuf[i],  1));

            char_count = 0;
            prev_char = _inbuf[i];

            is_operating_count = 0;

        }

    }

    return;
}

static int total = 0;
void CRle::run_encode()
{
    while (1)
    {
        _inbuf_len = _infile->read_file(_inbuf, sizeof(_inbuf));
        total += _inbuf_len;

        assert(_inbuf_len >= 0);
        encode(_inbuf, _inbuf_len);
        dump_processed2encoded(1);
        dump2file();
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


void CRle:: dump2file()
{
    if (_outfile == NULL)
    {
        _outbuf_len = 0;
        return;
    }

    int r =_outfile->write_file(_output, _outbuf_len);
    // printf ("%d %d\n", r, _outbuf_len);
    assert(r == _outbuf_len);
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

void CRle::dump_processed2encoded(bool not_end)
{

    int processed_flag = -1;
    int i = 0;
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
    int flag = 0;
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
void CRle::decode_count(int& count, char c)
{
    assert(c & 0x80);
    count <<= 7;
    count |= (c & 0x7f);
    return;
}
