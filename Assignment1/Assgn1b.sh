#/bin/bash
file="1b_input.txt"
file2="ans.txt"
echo 'Serial      Random string' > $file2
count=1
while read line
do
        # display $line or do somthing with $line
        #echo "$count     $line"
        echo "$count     $line" >> $file2
        count=`expr $count + 1`

done <"$file"
