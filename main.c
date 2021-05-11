#include "declaration.h"
#include <signal.h>
#include <unistd.h>

void main(){
   	
    signal(SIGINT, status);
    //UserInput
   	input();

    //File Reading
    fileReading();

	//printf("Before main timer\n");
	sleep(2);

//	printf("After main timer\n");
	sleep(2);

    threadCreationDeletion();

while(1);
	//while(flag != 3 &&  flag != 4);
}