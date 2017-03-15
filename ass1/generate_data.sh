TEST_CASE=30

TEST_DIR=./test
mkdir -p $TEST_DIR

random_str()
{
    MATRIX="01Avwxyz~!@#$%^&*()_+="
    # MATRIX="012"
    LENGTH=$(($1  ))
    PASS=""
    while [ "${n:=1}" -le "$LENGTH" ]
    do
        PASS="$PASS${MATRIX:$(($RANDOM%${#MATRIX})):1}"
        let n+=1
    done
    echo $PASS
}

for ((i = 20;i < $TEST_CASE; i ++))
do
    echo `random_str $RANDOM` > $TEST_DIR/$i
    # ./rlencode ${TEST_DIR}/${i} ${TEST_DIR}/${i}.rle
    # ./rlencode $TEST_DIR/$i > $TEST_DIR/$i.rle.out
    # ./rldecode $TEST_DIR/$i.rle $TEST_DIR/$i.rle.origin
    # ./rldecode $TEST_DIR/$i.rle > $TEST_DIR/$i.rle.out.de
    # diff $TEST_DIR/$i $TEST_DIR/$i.rle.origin
    # diff $TEST_DIR/$i.rle.out $TEST_DIR/$i.rle.out.de
    # echo "pass $i"

done
