#include "op_file.h"
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>// stat
#include <stdio.h>   // FILE...
#include <assert.h>
using namespace std;

COpFile::COpFile(const string& file_name, const std::string& open_flag)
{
    file = NULL;
    file = fopen(file_name.c_str(), open_flag.c_str());
    if (file == NULL)
    {
         throw string("open file [") + file_name +"] failed";

    }
    return;

}

COpFile::~COpFile()
{
    if (file != NULL)
    {
        fclose(file);
        file = NULL;
    }
}

int COpFile::read_file(char* output, int max_length)
{
    size_t read_length = fread(output, max_length, 1, file);
    assert( ferror(file) == 0);
    // if (feof(file) == )
    return (int) read_length;
    // return r > 0 ? r : -errno;

}

bool COpFile::read_line(char* output, int max_length)
{
    char* r = fgets( output, max_length, file);
    if (r == NULL)
    {
        return 0;
    }
    output[strcspn(output, "\n")] = 0;
    return 1;

}
int COpFile::write_file(char* output, int length)
{
    int r = fwrite( output, length, 1, file);
    return r >= 0 ? r: -errno;

}
int COpFile::seek(size_t off)
{
    return fseek(file, off, SEEK_SET);
}

int COpFile::has_content()
{
    struct stat fileStat;
    if( -1 == fstat(fileno(file), &fileStat))
    {
        return  -1;
    }
    return fileStat.st_size > 0 ? 1:0;
}

int COpFile::file_size()
{
    struct stat fileStat;
    assert(0 == fstat(fileno(file), &fileStat));
    return fileStat.st_size ;
}


COpDir::COpDir(const std::string& dir_name)
{
    dir_path = dir_name;
    struct stat st;
    memset(&st, 0, sizeof(struct stat));
    if (stat(dir_name.c_str(), &st) == -1)
    {
        assert(mkdir(dir_path.c_str(), 0770) == 0);
    }
    dir = opendir (dir_name.c_str());
    assert(dir != NULL);

}
COpDir::~COpDir()
{
    if (dir != NULL)
    {
        closedir(dir);
        dir = NULL;
    }
}
int COpDir::read_filelist(std::vector<std::string>& out)
{
    struct dirent* in_file;
    out.clear();
    while ((in_file = readdir(dir)))
    {
        if (!strcmp (in_file->d_name, "."))
            continue;
        if (!strcmp (in_file->d_name, ".."))
            continue;
        if (in_file->d_type == DT_DIR)
        {
            //FIXME
            printf ("what happend, %s\n", in_file->d_name);
            continue;
        }
        out.push_back(dir_path + "/" + in_file->d_name);

    }
    assert(errno == 0);
    return 0;
}


int COpDir::file_exist(const std::string& file)
{
    struct stat   buffer;
    return (stat ((dir_path + "/" + file).c_str(), &buffer) == 0);
}
