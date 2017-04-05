#!/bin/bash

if [ $# != 1 ]
then
    echo "./auto_test.sh [your testfile dir]"
    exit -1
fi
CUR_DIR=`pwd`
DIR=$1
MAX_QUERY_STRING_LEN=256
rm "$DIR/peda-session-bwtsearch.txt.fucked" 2>/dev/null
rm "$DIR/peda-session-bwtsearch.txt" 2>/dev/null
# echo "generating fucked files..."
cd $DIR
for i in *.txt
do
    if [ ! -f $i.fucked   ]
    then
        cat $i | sed -r 's/\[/\n/g' |grep -v "^$" |awk '{print "["$0}' > $i.fucked
    fi
done


cd -

cd $DIR
# echo "begin"
for i in *.txt.fucked
do
    if [ $i == "peda-session-bwtsearch.txt.fucked" ]
    then
        continue
    fi

    file_size=`ls -l $i|awk  '{print $5}' | grep -v "^$"`
    test_count=300
    # echo $i $file_size
    if [ "$file_size" > 100000000 ]
    then
        test_count=100
    fi
    # echo "testing $i, there will be $test_count * 3 test cases"
    filename=$i
    # echo $i

    total_line=`wc -l $filename|awk '{print $1}'`
    for((t=0;t<$test_count;t++))
    do
        query_string=""
        grep_string=""
        query_num=$(($((RANDOM%3))+1))
        # query_num=$(($((RANDOM%3))+1))
        for((j=0;j<$query_num;j++))
        do
            line=$((RANDOM%total_line))
            line=$((line+1))
            line_content="`sed -n "$line,${line}p" $filename|awk -F']' '{print $2}'`"
            # echo $i $line_content
            total_len=${#line_content}
            # echo $total_len
            start_pos=$(($((RANDOM%total_len )) - 1))

            # echo $start_pos

            if [ $start_pos -lt 0 ]
            then
                start_pos=0
            fi
            substr="${line_content:$start_pos:$(($((RANDOM%256))+1))}"
            fuck_empty="`echo "$substr"|sed \"s/'/\\'/g\"`"
            query_string="$query_string"" '$substr'"
            # echo "see me: $substr"
            if [ "${#grep_string}" == "0" ]
            then
                grep_string="fgrep -e '$substr'"
            else
                grep_string=$grep_string"| fgrep -e '$substr'"
            fi
            if grep '^[[:digit:]]*$' <<< "$substr" >/dev/null 2>&1
            then
                grep_string=$grep_string"| grep  -e \"\[.*\].*$substr\""
            fi
        done
        # echo $query_string
        # echo $grep_string

        bwt_file=${i/.txt.fucked/.bwt}
        index_file=${i/.txt.fucked/.index}
        # echo $bwt_file
        # echo $index_file
        # echo ../bwtsearch ./$bwt_file ./$index_file $query_string
        echo ../bwtsearch ./$bwt_file ./$index_file $query_string
        eval "../bwtsearch ./$bwt_file ./$index_file $query_string > tmp.out1"
        # echo "cat $i|$grep_string > tmp.out2"
        eval "cat $i|$grep_string > tmp.out2"
        flag=$RANDOM
        diff tmp.out1 tmp.out2
        if [ $? -eq 1 ]
        then
            mv tmp.out1 $flag.$i.$t.prog.out
            mv tmp.out2 $flag.$i.$t.grep.out
            echo ../bwtsearch ./$bwt_file ./$index_file $query_string > $flag.$i.$t.cmd
            echo "cat $i|$grep_string ">> $flag.$i.$t.cmd
            echo "refer to $flag.$i.$t.cmd"
        fi


    done
    # echo "testing $i finished."
done




