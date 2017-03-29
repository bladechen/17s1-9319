#include <sys/types.h>
#include <string>
#include <sys/uio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>



class COpFile
{
    public:
    COpFile(const std::string& file_name, int open_flag);

    virtual ~COpFile();



    int read_from_position(size_t off, char* buf, int max_length);

    // int seek(size_t off);


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
    protected:
    int fd;

};
