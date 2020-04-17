#!/bin/bash
#    sed_tab_to_space.sh - shell script to convert tabs to spaces
#    Copyright (C) 2020  Johannes Schmatz
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.

if [[ $# != 2 ]];
then
	echo "usage: $0 <in_file> <outfile>"
	echo "replace all tabs with spaces"
	echo "LICENSE GPLv3"
else
	cat -T $1 | sed -e 's/\^\I/        /g'  > $2
fi

# syntax
# vim: ts=8 sts=8 sw=8 noet si syntax=bash
