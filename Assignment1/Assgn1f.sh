awk '!seen[$1, $2]++ && !seen[$2, $1]++ && $1!=$2' 1f.graph.edgelist > 1f_output_graph.edgelist
grep -oE '[0-9]+' 1f_output_graph.edgelist | sort | uniq -c | sort -r | awk '{print $2" "$1}' > temp1f.txt
sort -k2,2gr "temp1f.txt" > "final1f.txt"
head -5 final1f.txt
rm temp1f.txt
rm final1f.txt
