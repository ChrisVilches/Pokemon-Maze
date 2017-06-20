testcases=0; 
for f in test/in*.txt; 
do 
testcases=$((testcases+1)); 
done; 

echo "testing..."


for i in $(seq 1 $testcases)
do
	num=$(printf "%02g" $i)
	echo "Test #$num"
	diff <(./boulder.exe < test/in$num.txt) <(cat test/sol$num.txt)
done



echo "Ok"
