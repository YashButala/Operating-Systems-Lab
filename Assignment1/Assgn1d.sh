
mkdir "1.d.files.out"
#cd "1.d.files"
srcdir="1.d.files/"
dstdir="1.d.files.out/"
finaldst="1.d.files/1.d.files"


for i in $finaldst/*
do
	echo $i
	file=${i##*/}
	sort -nr $i > $dstdir$file
done
temp="result.txt"
for i in $dstdir/*
do
	cat $i >> $temp
done
sort -nr $temp > "1.d.out.txt" 
rm $temp




	
