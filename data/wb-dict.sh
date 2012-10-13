#!/bin/bash
# 五笔编码查询
#
# Usage:
# > wb-dict.sh 我
# > wb-dict.sh wo
# > wb-dict.sh 我 wb86.txt
#

input=$1 # the word(pinyin or hanzi) to search
pyciku=`dirname $0`/py.txt
 
if [ -z "$input" ]
then
	exit -1
fi

if [ -z "$2" ]
then
	wbciku=`dirname $0`/wb98.txt
else
	wbciku=$2
fi

case "$input" in 
	[a-zA-Z]*) # input pinyin
	for word in `grep "^$input\b" $pyciku | sed "s/^$input//"`
	do
			echo -n "$word"
			for bianma in `grep "\b$word$" $wbciku | sed "s/$word$//"`
			do
					echo -n " $bianma"
			done
			echo
	done
	exit 0;;
	*) # input chinese
	echo -n "$input"
			for bianma in `grep "\b$input$" $wbciku | sed "s/$input$//"`
			do
					echo -n " $bianma"
			done
			echo
	exit 0;;
esac

