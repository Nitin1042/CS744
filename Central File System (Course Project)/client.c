#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <time.h>

#define PORT 1042
#define BUFFER_SIZE 1024

char command[BUFFER_SIZE];
char buffer[BUFFER_SIZE];

void send_login_req(int sockfd, char *command)
{
	memset(buffer, 0, BUFFER_SIZE);
		
	strcat(command, " ");
	printf("Enter Username: ");
	fgets(buffer, BUFFER_SIZE, stdin);
	buffer[strcspn(buffer, "\n")] = 0;
	strcat(command, buffer);
	strcat(command, " ");
		
	printf("Enter Password: ");
	fgets(buffer, BUFFER_SIZE, stdin);
	buffer[strcspn(buffer, "\n")] = 0;
	strcat(command, buffer);
		
	//printf("Command: %s\n",command);
		
        memset(buffer, 0, BUFFER_SIZE);
        send(sockfd, command, strlen(command), 0);
        recv(sockfd, buffer, BUFFER_SIZE, 0);	
}

char* get_file()
{
	char *filename = (char *)malloc(BUFFER_SIZE);
        		
        printf("Enter Filename: ");
	fgets(filename, BUFFER_SIZE, stdin);
	filename[strcspn(filename, "\n")] = 0;
	
	return filename;
}

char* read_req(int sockfd, char *command, char *filename)
{
	char *read_buff = (char *)malloc(BUFFER_SIZE);
			
	strcat(command, " ");
	strcat(command, filename);
	memset(buffer, 0, BUFFER_SIZE);
			
        send(sockfd, command, strlen(command), 0);
        		
        recv(sockfd, buffer, BUFFER_SIZE, 0);
        
        strcpy(read_buff, buffer);
        
        return read_buff;
}

void checkout_req(int sockfd, char *command, char *filename)
{
	char* r_buf = read_req(sockfd, command, filename);
	
	if(strcmp(r_buf, "FILE_NOT_FOUND") == 0)
	{
		printf("Server Response: %s\n", r_buf);
		free(r_buf);
		return;
	}
	
	char path[BUFFER_SIZE] = "./clientfiles/";
	strcat(path, filename);
	
	FILE *f = fopen(path, "w");
	
	if(f == NULL)
	{
		perror("Error Creating File");
		free(r_buf);
		
		return;
	}
	
	printf("File Checked out\n");
	
	fwrite(r_buf, sizeof(char), strlen(r_buf), f);
	
	fclose(f);
	free(r_buf);
	
	return;	
}

void write_req(int sockfd, char* command, char* filename, char choice, char* content)
{
	char choicestr[2];
	choicestr[0] = choice;
        choicestr[1] = '\0';
	
	strcat(command, " ");
	strcat(command, filename);
	
	strcat(command, " ");
	strcat(command, choicestr);
	
	strcat(command, " ");
	strcat(command, content);
	
	//printf("Command: %s\n", command);
	
	memset(buffer, 0, BUFFER_SIZE);
			
        send(sockfd, command, strlen(command), 0);		
        recv(sockfd, buffer, BUFFER_SIZE, 0);
        
        printf("Server Response: %s\n", buffer);
        
        return;
}

void delete_create_req(int sockfd, char* command, char* filename)
{
	strcat(command, " ");
	strcat(command, filename);
	memset(buffer, 0, BUFFER_SIZE);
			
        send(sockfd, command, strlen(command), 0);
        recv(sockfd, buffer, BUFFER_SIZE, 0);
        
        printf("Server Response: %s\n", buffer);
}

void commit_req(int sockfd, char* command, char* filename)
{
	
	char path[BUFFER_SIZE] = "./clientfiles/";
        strcat(path, filename);
                		
        FILE *f = fopen(path, "r");
        
        if(f == NULL)
        {
        	printf("No Such File in Local Machine\n");
        	return;
        }
               		
        fseek(f, 0, SEEK_END);
        int f_size = ftell(f);
        fseek(f, 0, SEEK_SET);
        
        char choice = 'c';
        char *read_buff = (char *)malloc(f_size+1);
        fread(read_buff, sizeof(char), f_size, f);
        read_buff[f_size] = '\0';
        
        write_req(sockfd, command, filename, choice, read_buff);
        free(read_buff);
        return;
        
}


void waiting_state(int sockfd) 
{
    	fd_set readfds;
    	struct timeval tv;
    	int maxfd = sockfd + 1;
    
        printf("Waiting for Server....\n\n");
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        
        // Set timeout for select
        tv.tv_sec = 1;  // 1 second
        tv.tv_usec = 0;

        // Wait for input from the socket
        int activity = select(maxfd, &readfds, NULL, NULL, &tv);

        if (activity < 0) 
        {
            	perror("select error");
            	return;
        }

        // Check if there's incoming data from the server
        if (FD_ISSET(sockfd, &readfds)) 
        {
            	memset(buffer, 0, BUFFER_SIZE);
            	int bytes_received = recv(sockfd, buffer, BUFFER_SIZE, 0);
            	if (bytes_received > 0) 
            	{
                	printf("Message from server: %s\n", buffer);
            	} 
            	else 
            	{
                	printf("Server disconnected.\n");
            	}
        }
        return;
}


int main() 
{
    	int sockfd;
    	struct sockaddr_in server_addr;
    	bool success = false;
    	time_t sec1, sec2;
    	

    	sockfd = socket(AF_INET, SOCK_STREAM, 0);
    	server_addr.sin_family = AF_INET;
    	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Change if needed
    	server_addr.sin_port = htons(PORT);

    	if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) 
    	{
        	perror("Connection failed");
        	return -1;
    	}
    
    	while(!success)
    	{
    	
    		printf("Enter command (REGISTER, LOGIN, EXIT): ");
    		fgets(command, BUFFER_SIZE, stdin);
    		command[strcspn(command, "\n")] = 0; // Remove newline
	
    		if (strcmp(command, "EXIT") == 0) 
    		{
        	    	break;
        	}
		
		send_login_req(sockfd, command);
		
        	if(strncmp(buffer, "LOGIN_SUCCESS", 13) == 0 || strncmp(buffer, "REGISTER_SUCCESS", 16) == 0 )
        	{
        		success = true;
        		printf("Server Response: %s\n", buffer);
        		break;
        	}
        	else
        	{	
        		printf("Server Response: %s\n", buffer);
        		printf("Try Again!!!!\n");
        	}
    	}
    	
    	while (1) 
    	{
        	waiting_state(sockfd);
        	
        	printf("Enter command (LIST, READ, WRITE, DELETE, CHECKOUT, CREATE, COMMIT, EXIT): ");
        	
        	fgets(command, BUFFER_SIZE, stdin);
        	
        	//printf("command: %s\n",command);
        	
        	command[strcspn(command, "\n")] = 0; // Remove newline

        	if (strcmp(command, "EXIT") == 0) 
        	{
            		break;
        	}
        	//List
        	else if (strcmp(command, "LIST") == 0)
        	{
        		memset(buffer, 0, BUFFER_SIZE);
        		send(sockfd, command, strlen(command), 0);
        		recv(sockfd, buffer, BUFFER_SIZE, 0);
        		
        		printf("Server Response: %s\n", buffer);
        	}
        	//READ
        	else if(strcmp(command, "READ") == 0)
        	{
        		time(&sec1);
        		
        		char *filename = get_file();
			
        		char *r_buf = read_req(sockfd, command, filename);
        		printf("Server Response: %s\n", r_buf);
        		free(r_buf);
        		free(filename);
        		
        		time(&sec2);
        		
        		printf("Time taken for Read Completion: %ld\n", sec2-sec1);
        	}
        	//Checkout
        	else if(strcmp(command, "CHECKOUT") == 0)
        	{
        		char *filename = get_file();
			
        		checkout_req(sockfd, command, filename);
        		free(filename);
        	}
        	//Write
        	else if ((strcmp(command, "WRITE") == 0))
        	{
        		char *filename = get_file();
			
        		printf("Do You Want to Append(a) or Overwrite (o): ");
        		char choice;
        		choice = getchar();
        		
        		while(getchar() !='\n');
        		
        		memset(buffer, 0, BUFFER_SIZE);
        		printf("Enter What To Write: ");
        		fgets(buffer, BUFFER_SIZE, stdin);
        		
        		write_req(sockfd, command, filename, choice, buffer);
        		free(filename);	
        	}
        	//Delete or Create
        	else if((strcmp(command, "DELETE") == 0) || (strcmp(command, "CREATE") == 0))
        	{
        		char *filename = get_file();
        		delete_create_req(sockfd, command, filename);
        	}
        	//Commit
        	else if ((strcmp(command, "COMMIT") == 0))
        	{	
        		char *filename = get_file();
        		
        		commit_req(sockfd, command, filename);
        	}
        	else
        	{
        		printf("Invalid Command!!!!\n");
        	}
    	}

    	close(sockfd);
    	
    	return 0;
}
