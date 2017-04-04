#include "op_file.h"
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>// stat
#include <stdio.h>   // FILE...
#include <assert.h>
using namespace std;

COpFile::COpFile(const string& file_name, int open_flag)
{
    fd = -1;
    fd = open(file_name.c_str(), open_flag, 0600);
    if (fd == -1)
    {
         throw string("open file [") + file_name +"] failed";

    }
    return;

}

COpFile::~COpFile()
{
    if (fd != -1)
    {
        close(fd);
        fd = -1;
    }
}

int COpFile::read_file(char* output, int max_length)
{
    int r = read(fd, output, max_length);
    return r > 0 ? r : -errno;

}

int COpFile::write_file(char* output, int length)
{
    int r = write(fd, output, length);
    return r > 0 ? r: -errno;

}

int COpFile::read_from_position(size_t off, char* buf, int max_length)
{
    int ret = lseek(fd, off, SEEK_SET);
    if (ret == -1)
    {
        return -errno;
    }
    ret = read(fd, buf, max_length);
    if (ret < 0)
    {
        return -errno;
    }
    return ret;
}
int COpFile::has_content()
{
    struct stat fileStat;
    if( -1 == fstat(fd, &fileStat))
    {
        return  -1;
    }
    return fileStat.st_size > 0 ? 1:0;
}

int COpFile::file_size()
{
    struct stat fileStat;
    assert(0 == fstat(fd, &fileStat));
    return fileStat.st_size ;


}
