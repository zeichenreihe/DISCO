/*
    Arduino client program for NWT
    Copyright (C) 2020  Johannes Schmatz

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>

int helptext(char *name){
	printf("NAME\n\t%s - a client programm for DISCO\n\nSYNOPSYS\n\t%s file [-s]|-l|-n\n\t%s [--help]\n\nDESCRIPTION\n\t%s is the client programm for the big Arduino program, that\n\tplays music. It allows you to play music with or without\n\tleds or to play only leds.\n\n\tYou always need a serial device as argument 1 and you might\n\tneed root rights, so maybe type a 'sudo' in front of your command.\n\n\tOptions:\n\n\t\t-s\tplays on the Arduino the song with the leds.\n\t\t-l\t shows only the leds.\n\t\t-n\tplays the song without leds.\n\t\t--help\tprints this help\n\nEXIT STATUS\n\tThe exit status is 1 if the helptext was shown and it is 0 if there was\n\tno error.\n\nAUTHOR\n\tWritten by Johannes Schmatz.\n\nCOPYRIGHT\n\tYou should have received a copy of the GNU General Public License\n\talong with this program.  If not, see <http://www.gnu.org/licenses/>.\n\tsee LICENSE\n\nEXAMPLES\n\t%s /dev/ttyUSB0\tplays music.\n\t%s /dev/ttyUSB0 -l\tplays the leds.\n\t%s /dev/ttyUSB0 -n\tplay music without leds.\n\t%s --help\t\tprint this helptext.\n", name, name, name, name, name, name, name, name);
	return 1;
}

int main(int argc, char **argv){
	if( argc == 1 ){
		return helptext(argv[0]);
	}
	if( argv[1] == "--help" ){
		return helptext(argv[0]);
	}

	FILE *fileptr;
	fileptr = fopen(argv[1], "w");

	if(argv[2] == "-l"){
		fprintf(fileptr, "1");
	}else if(argv[2] == "-n"){
		fprintf(fileptr, "2");
	}else{
		fprintf(fileptr, "0");
	}

	system("sleep 3");
	fclose(fileptr);

	return 0;
}

// syntax
// vim: ts=8 sts=8 sw=8 noet si syntax=c
