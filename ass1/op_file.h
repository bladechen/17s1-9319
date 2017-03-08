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
    protected:
    int fd;

};
