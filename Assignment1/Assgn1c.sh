
field=( "$@" )
for var in "${field[@]}"; do
	echo "$var"
done
min=$field[0]
for i in ${field[@]}
do

if [[ $i < $min ]] ; then
min=$i

fi
done
gcdans=1
flag=0

for (( div=1; div<=$min; div++ ))
do
	flag=0
	for num in "${field[@]}"
	do
		ans=`echo $num % $div | bc`
		if [ $ans != 0 ]
		then 
			flag=1
		fi
	done 
	if [ $flag == 0 ]
	then 
		gcdans=$div
	fi
done
echo "gcd is : $gcdans"



