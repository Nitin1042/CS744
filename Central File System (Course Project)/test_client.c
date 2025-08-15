#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <time.h>

#define PORT 1042
#define BUFFER_SIZE 1024


int main() 
{
    	int sockfd;
    	struct sockaddr_in server_addr;
    	char heartbeat[] = "HEARTBEAT";
    	

    	sockfd = socket(AF_INET, SOCK_STREAM, 0);
    	server_addr.sin_family = AF_INET;
    	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Change if needed
    	server_addr.sin_port = htons(PORT);

    	if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) 
    	{
        	perror("Connection failed");
        	return -1;
    	}
    	
    	while(1)
    	{
    		send(sockfd, heartbeat, strlen(heartbeat), 0);
    		usleep(10);
    	}
    	
    	close(sockfd);
    	
    	return 0;
}
