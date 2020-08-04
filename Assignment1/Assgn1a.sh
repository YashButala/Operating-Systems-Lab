case $2 in
  +)res=`echo $1 + $3 | bc` 
  ;; 
  -)res=`echo $1 - $3 | bc` 
  ;; 
  \*)res=`echo $1 \* $3 | bc` 
  ;; 
  /)res=`echo "scale=2; $1 / $3" | bc` 
  ;; 
  %)res=`echo $1 % $3 | bc`
  ;;
esac
echo "$res"
