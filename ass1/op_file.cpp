#include <op_file.h>
#include <stdio.h>
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
