#!/bin/bash

make all
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
rm $DIR/*.index
rm $DIR/massif.out.*
rm /tmp/fuck_9319/*.index
RESULT=./result
rm $DIR/$RESULT
# echo "generating fucked files..."
cd $DIR
for i in *.txt
do
    if [ ! -f $i.fucked   ]
    then
        cat $i | sed -r 's/\[/\n/g' |grep -v "^$" |awk '{print "["$0}' > $i.fucked
    fi
done

function getTiming() {
    start=$1
    end=$2

    start_s=$(echo $start | cut -d '.' -f 1)
    start_ns=$(echo $start | cut -d '.' -f 2)
    end_s=$(echo $end | cut -d '.' -f 1)
    end_ns=$(echo $end | cut -d '.' -f 2)


# for debug..
#    echo $start
#    echo $end


    time=$(( ( 10#$end_s - 10#$start_s ) * 1000 + ( 10#$end_ns / 1000000 - 10#$start_ns / 1000000 ) ))


    echo "$time ms" >> $RESULT
}




echo "This is only a test to get a ms level time duration..."
# ls >& /dev/null    # hey, be quite, do not output to console....

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
    test_count=200
    if [ $file_size -lt 1000 ]
    then
        test_count=10
    fi
    # echo $i $file_size
    # echo "testing $i, there will be $test_count * 3 test cases"
    filename=$i
    # echo $i

    total_line=`wc -l $filename|awk '{print $1}'`
    for((t=0;t<$test_count;t++))
    do
        query_string=""
        grep_string=""
        query_num=$(($((RANDOM%3))+1))
        # query_num=1
        for((j=0;j<$query_num;j++))
        do
            # echo $total_len
            substr=""
            for ((k=0;k<100;k++))
            do
                line=$((RANDOM%total_line))
                line=$((line+1))
                line_content="`sed -n "$line,${line}p" $filename|awk -F']' '{print $2}'`"
                # echo $i $line_content
                total_len=${#line_content}

                start_pos=$(($((RANDOM%total_len )) - 2))

                # echo $start_pos

                if [ $start_pos -lt 0 ]
                then
                    start_pos=0
                fi
                substr="${line_content:$start_pos:$(($((RANDOM%256))+3))}"
                tmp="fgrep -e '$substr'"
                if grep '^[[:digit:]]*$' <<< "$substr" >/dev/null 2>&1
                then
                    tmp=$tmp"| grep  -e \"\[.*\].*$substr\""
                fi

            echo "cat $i|$tmp|wc -l >>$RESULT"
            a=$(eval "cat $i|$tmp|wc -l")
            echo $a
            if [ $a -lt 7000 ]
            then
                break
            fi
            echo fuck!!!!!!!


            done
            fuck_empty="`echo "$substr"|sed \"s/'/\\\\'/g\"`"
            query_string="$query_string"" '$fuck_empty'"
            # echo "see me: $substr"
            tmp=""
            tmp="fgrep -e '$substr'"
            if [ "${#grep_string}" == "0" ]
            then
                grep_string="fgrep -e '$substr'"
            else
                grep_string=$grep_string"| fgrep -e '$substr'"
            fi
            if grep '^[[:digit:]]*$' <<< "$substr" >/dev/null 2>&1
            then
                grep_string=$grep_string"| grep  -e \"\[.*\].*$substr\""
                tmp=$tmp"| grep  -e \"\[.*\].*$substr\""
            fi

            echo "cat $i|$tmp|wc -l >>$RESULT"
            eval "cat $i|$tmp|wc -l >>$RESULT"
        done
        # echo $query_string
        # echo $grep_string

        bwt_file=${i/.txt.fucked/.bwt}
        index_file=${i/.txt.fucked/.index}
        # echo $bwt_file
        # echo $index_file
        # echo ../bwtsearch ./$bwt_file ./$index_file $query_string
        echo $CUR_DIR/bwtsearch ./$bwt_file ./$index_file $query_string
        eval " valgrind --tool=massif  --pages-as-heap=yes $CUR_DIR/bwtsearch ./$bwt_file ./$index_file $query_string > tmp.out1"
        start=$(date +%s.%N)
        eval "$CUR_DIR/bwtsearch ./$bwt_file ./$index_file $query_string |sort |uniq> tmp.out1"
        end=$(date +%s.%N)
        getTiming $start $end

        # echo "cat $i|$grep_string > tmp.out2"
        eval "cat $i|$grep_string|sort|uniq > tmp.out2"
        flag=$RANDOM
        echo -n "result line: " >>$RESULT
        cat tmp.out1|wc -l >>$RESULT
        diff tmp.out1 tmp.out2
        if [ $? -eq 1 ]
        then
            mv tmp.out1 $flag.$i.$t.prog.out
            mv tmp.out2 $flag.$i.$t.grep.out
            echo $CUR_DIR/bwtsearch ./$bwt_file ./$index_file $query_string > $flag.$i.$t.cmd
            echo "cat $i|$grep_string ">> $flag.$i.$t.cmd
            echo "fuck u.....refer to $flag.$i.$t.cmd" >> $RESULT
        fi
        echo "">>$RESULT


    done
    # echo "testing $i finished."
done


rm /import/adams/3/z5048663/17s1-9319/ass2/rr
echo "mem:"
grep mem_heap_B  massif.out.* |awk -F '=' '{print $2}'|sort -n|tail -50 >>  /import/adams/3/z5048663/17s1-9319/ass2/rr
echo "error: " >>  /import/adams/3/z5048663/17s1-9319/ass2/rr
grep "refer to" $RESULT >>  /import/adams/3/z5048663/17s1-9319/ass2/rr
echo "end error" >>  /import/adams/3/z5048663/17s1-9319/ass2/rr
grep "ms$" $RESULT|awk '{print $1}' |sort -n|tail -50 >> /import/adams/3/z5048663/17s1-9319/ass2/rr




