#!/bin/bash
if [[ $# != 2 ]];
then
	echo "usage: $0 <in_file> <outfile>"
	echo "replace all tabs with spaces"
else
	cat $1 | sed -e "s/        /\t/g"  > $2
fi
