//TODO - file
// compile with -x c

#include <stdlib.h>
#include <stdio.h>

#define FINISH 0
#define TODO 1

int music_hear(){
	//system("bash -c 'music_dir_queen; music -r'");

	printf("STATE: music_hear:\tFINISH\n\n");
	return FINISH;
}

int led(){
	printf("pos: BR S21 T9 (END)\n");
	printf("0	\n");
	printf("1	\n");
	printf("2 1	\n");
	printf("3	\n");
	printf("4 1	\n");
	printf("5	\n");
	printf("6 1	\n");
	printf("7	\n");

	printf("STATE: led:\tFINISH\n\n");
	return FINISH;
}

int XXX(){
	printf("correct all XXX\n");

	printf("STATE: XXX:\tFINISHED\n\n");
	return FINISH;
}

int music(){
	printf("finished: Bohemain Rhapsody\n");
	printf("finished: What Shall We Do With A Drunken Sailor\n");
	printf("finished: The Final Countdown\n");

	printf("STATE: music:\tFINISHED\n\n");
	return FINISH;
}

int servo(){
	printf("STATE: servo:\tFINISH\n");
	return FINISH;
}
int guitarr(){
	printf("STATE: guitarr: \tFINICH\n");
	return FINISH;
}

int main(){
	music_hear();
	led();
	XXX();
	music();
	servo();
	guitarr();
}

// syntax
// vim: ts=8 sts=8 sw=8 noet si syntax=c
