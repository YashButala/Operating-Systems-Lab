echo "Enter number 1-4"
read column
#echo $column
input="1e_input.txt/1d_input.txt"


temp="temp.txt"
if [ $column = 1 ]
then
    awk '{count[tolower($1)]++} END {for (word in count) print word, count[word] >"output.txt" }' $input
    sort -k2,2gr "output.txt" > "1e_output_1_column.freq"
    rm "output.txt"
elif [ $column = 2 ]
then
    awk '{count[tolower($2)]++} END {for (word in count) print word, count[word] >"output.txt" }' $input
    sort -k2,2gr "output.txt" > "1e_output_2_column.freq"
    rm "output.txt"
elif [ $column = 3 ]
then
    
    awk '{count[tolower($3)]++} END {for (word in count) print word, count[word] >"output.txt" }' $input
    sort -k2,2gr "output.txt" > "1e_output_3_column.freq"
    rm "output.txt"
else
    awk '{count[tolower($4)]++} END {for (word in count) print word, count[word] >"output.txt"}' $input    
    sort -k2,2gr "output.txt" > "1e_output_4_column.freq"
    rm "output.txt"
fi    

