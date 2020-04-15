#!/bin/bash
if [[ $# != 2 ]];
then
	echo "usage: $0 <in_file> <outfile>"
	echo "replace all tabs with spaces"
	echo "LICENSE GPLv3"
else
	cat -T $1 | sed -e 's/\^\I/        /g'  > $2
fi
