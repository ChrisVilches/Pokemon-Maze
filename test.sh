echo Testing...

for f in test/in*.txt
do
	echo Test ${f//[^0-9]/}
	diff <(./boulder.exe < $f) <(cat ${f/in/sol})
done

echo "Ok"
