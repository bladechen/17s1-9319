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
        /**
         * @brief:  if writeout_file is not provided, debug info will be print to stdout, otherwise, the encoded/decoded will be dumped into this file.
         *
         *
         *
         * @param:  readin_file
         * @param:  writeout_file
         */
        CRle(const std::string& readin_file,
             const std::string& writeout_file = "");
        virtual ~CRle();

        /**
         * @brief:  readin_file should be the text needed to be encoded
         */
        void run_encode();
        /**
         * @brief:  readin_file should be the encoded text
         */
        void run_decode();


    protected:
        void encode(char* buf, int len);
        void decode(char* buf, int len);


        void dump_processed2encoded(bool not_end);
        void dump_processed2decoded(bool not_end);

        void encode_count(int count, char* buf, int & len);
        void decode_count(int& count, char c);

        void output_stdout(const std::pair<char, int>&);

        void dump2file();


    private:
        char _inbuf[4096 * 64];
        int _inbuf_len; // _inbuf_len of _inbuf is read in


        std::vector<std::pair<char, int> > _processed; // saved as char, count of char.

        char _output[4096 * 64];
        int _outbuf_len;

        COpFile* _infile;
        COpFile* _outfile;


        bool is_operating_count; // only used when decoding, stands for reading a count from the file.
};

