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
//#include <system.h>

int main(int argc, char **argv){
        if( argc == 1 ){
                printf("NAME\n");
                printf("        %s - a client program for the Arduino program\n\n", argv[0]);
                printf("SYNOPSYS\n");
                printf("        %s file [-s]|-l|-n\n", argv[0]);
                printf("        %s [--help]\n\n", argv[0]);
                printf("DESCRIPTION");
                printf("        %s is the client program for the big Arduino program, that\n", argv[0]);
                printf("        plays music. It allows you to play music with or without\n");
                printf("        leds or to play only leds.\n\n");
                printf("        You always need a serial device as argument 1 and you might\n");
                printf("        need root rights, so maybe type a 'sudo' in front of your command.\n\n");
                printf("        Options:\n\n");
                printf("                -s      plays on the Arduino the song with the leds.\n");
                printf("                -l      shows only the leds.\n");
                printf("                -n      plays the song without leds.\n\n");
                printf("EXIT STATUS\n");
                printf("        The exit status is 1 if the helptext was shown and it is 0 if there was\n");
                printf("        no error.\n\n");
                printf("AUTHOR\n");
                printf("        Written by Johannes Schmatz.\n\n");
                printf("COPYRIGHT\n");
                printf("        Arduino client program for NWT\n");
                printf("        Copyright (C) 2020  Johannes Schmatz\n\n");
                printf("        This program is free software: you can redistribute it and/or modify\n");
                printf("        it under the terms of the GNU General Public License as published by\n");
                printf("        the Free Software Foundation, either version 3 of the License, or\n");
                printf("        (at your option) any later version.\n\n");
                printf("        This program is distributed in the hope that it will be useful,\n");
                printf("        but WITHOUT ANY WARRANTY; without even the implied warranty of\n");
                printf("        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n");
                printf("        GNU General Public License for more details.\n\n");
                printf("        You should have received a copy of the GNU General Public License\n");
                printf("        along with this program.  If not, see <http://www.gnu.org/licenses/>.\n\n");
                printf("EXAMPLES\n");
                printf("        %s /dev/ttyUSB0       plays music.\n", argv[0]);
                printf("        %s /dev/ttyUSB0 -l    plays the leds.\n", argv[0]);
                printf("        %s /dev/ttyUSB0 -n    play music without leds.\n", argv[0]);
                printf("        %s                    print this helptext.\n", argv[0]);
                return 1;
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
