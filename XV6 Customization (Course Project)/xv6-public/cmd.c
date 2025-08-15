#include "types.h"
#include "stat.h"
#include "user.h"


int main(int argc, char *argv[])

{

    	if(argc < 2)

    	{

        	printf(1, "Usage: %s <program> [arguments...]\n", argv[0]);

        	exit();

    	}

	//  --------------------------- Write your code here ---------------------------

	int pid = fork();
	
	if(pid==0)
	{
		exec(argv[1], argv+1);
		
		exit();
	}
	else
	{
		wait();
	}
	
	exit();
}
