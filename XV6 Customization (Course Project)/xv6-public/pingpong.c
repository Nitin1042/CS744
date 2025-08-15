#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#define BUFSIZE 512

void ping_pong(int fd) 
{
	// ------------------------- Write your code here -------------------------
	char buffer[BUFSIZE];
	int bytesRead;
	
	while((bytesRead = read(fd, buffer, sizeof(buffer)-1)) > 0)
	{
		int i,j;
	
		char *pong = "ping";
	
		//i<=size-4 to stop the comparision in inner for loop if some characters remain in 
		//buffer but the remaining size is less than 4 which is length of ping
		for(i=0; i<=bytesRead-4; i++)
		{
			for(j=0; j<6; j++)
			{
				if(buffer[i+j] != pong[j])
				{
					break;
				}
			}
		
			if(j == 4)
			{
			printf(1, "pong\n");
			}
		}
	}
}

int main(int argc, char *argv[])
{
    	if (argc != 2)
    	{
        	printf(1, "Usage: %s <input_file>\n", argv[0]);
        	exit();
    	}

    	int fd = open(argv[1], O_RDONLY);
    	if (fd < 0)
    	{
        	printf(1, "Error opening file %s\n", argv[1]);
        	exit();
    	}

    	ping_pong(fd);
    	close(fd);

    	exit();
}
