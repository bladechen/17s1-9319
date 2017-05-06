#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <string>
#include <sys/uio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>

#include <string>
#include <vector>


class COpFile
{
    public:
        COpFile(const std::string& file_name, const std::string& open_flag);

    virtual ~COpFile();



    /**
     * @brief:  seek and read
     *
     * @param:  off
     * @param:  buf
     * @param:  max_length
     *
     * @return: >=0 indicates the bytes read, other wise -errno
     */
    inline int read_from_position(const size_t& off, char* buf, const int& max_length)
    {
        int ret = seek(off);
        if (ret == -1)
        {
            return -errno;
        }
        ret = fread(buf, max_length, 1, file);
        assert(ret > 0);
        return ret;

    };

    int seek(size_t off);


    // int write_to_posi
    /**
     * @brief:
     *
     * @param: output
     * @param:  max_length
     *
     * @return: negative indicates error, positive means the bytes read
     */
    int read_file(char* output, int max_length);

    bool read_line(char* output, int max_length);

    /**
     * @brief:
     *
     * @param:  output
     * @param:  length
     *
     * @return: negative indicates error, which is -errno, positive means the bytes write to files
     */
    int write_file(char* output, int length);




    int has_content();
    int file_size();
    protected:
    FILE* file;

};

class COpDir
{
    public:
        COpDir(const std::string& dir);
        virtual ~COpDir();
        int read_filelist(std::vector<std::string>& out);
        // int create_file(const std::string& filename);

        int file_exist(const std::string& file);

        std::string get_dir_str()
        {
            return dir_path;
        }

    private:
        std::string dir_path;
        DIR* dir;
};
