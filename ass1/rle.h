#include <string>
#include <utility>
#include <vector>
#include <sys/types.h>
#include <op_file.h>
#include <sys/stat.h>
#include <fcntl.h>

class CRle
{
    public:
        CRle(const std::string& readin_file,
             const std::string& writeout_file = "");
        virtual ~CRle();

        void run_encode();
        void run_decode();


    protected:
        void encode(char* buf, int len);
        void decode(char* buf, int len);


        void dump_processed2encoded(bool not_end);
        void dump_processed2decoded(bool not_end);

        void encode_count(int count, char* buf, int & len);
        void decode_count(int& count, char c);

        // void
        void output_stdout(const std::pair<char, int>&);

        void dump2file();


    private:
        char _inbuf[4096 * 5];
        int _inbuf_len; // _inbuf_len of _inbuf is read in


        std::vector<std::pair<char, int> > _processed; // saved as char, count of char.

        char _output[4096 * 5];
        int _outbuf_len;

        COpFile* _infile;
        COpFile* _outfile;


        bool is_operating_count;
};

