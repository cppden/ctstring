#! /bin/sh

if [ "$#" -lt 1 ]; then
	echo "Need input file name"
	exit -1
fi

INPUT=$1

#strings -3 --output-separator= $INPUT | sed 's/@//g'| egrep "(BIN|INA|NAR|ARY)" > /dev/null 
strings -3 --output-separator= $INPUT | sed -r 's/(@|_|%|[0-9]|-|=|\.|\^|\$)//g' | egrep "(BIN|INA|NAR|ARY)"


OUT=$?
if [ $OUT -eq 0 ];then
    echo "Found parts of hidden string!"
    exit 1
else
    #objdump -Ctw $INPUT
    nm -C $INPUT | egrep "::ic<|XChars<" 
	OUT=$?
	if [ $OUT -eq 0 ];then
		echo "Found symbols!"
		exit 2
	else
	    echo "PASSED"
	    exit 0
	fi
fi
 
