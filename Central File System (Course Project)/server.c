#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <dirent.h>
#include <stdbool.h>

#define PORT 1042
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
#define FILE_NAME_SIZE 256
#define USERNAME_SIZE 32
#define PASSWORD_SIZE 32
#define MAX_CHECKED_OUT_FILES 10


typedef struct 
{
	char filename[FILE_NAME_SIZE];
    	int writer; // -1 if no writer
    	pthread_mutex_t lock;
} FileEntry;

typedef struct 
{
    	char username[USERNAME_SIZE];
    	char password[PASSWORD_SIZE];
    	char *checked_out_files[MAX_CHECKED_OUT_FILES];
    	int checked_out_count;
    	bool notified;
} User;


User users[MAX_CLIENTS];
FileEntry files[MAX_CLIENTS];
int file_count = 0;
int user_count = 0;

pthread_mutex_t file_lock = PTHREAD_MUTEX_INITIALIZER;

void initialize_files()
{
	const char *directoryPath = "./files";
	
    	// Open the directory
    	DIR *dir = opendir(directoryPath);
    	if (dir == NULL) 
    	{
        	perror("Failed to open directory");
        	return;
    	}

    	struct dirent *entry;
    	// Read entries in the directory
    	while ((entry = readdir(dir)) != NULL) 
    	{
        	// Skip the "." and ".." entries
        	if (entry->d_name[0] != '.') 
        	{
        		    //printf("%s\n", entry->d_name);
        		    strcpy(files[file_count].filename, entry->d_name);
        		    //printf("Initializing: %s\n", files[file_count].filename); 
        		    file_count++;
        	}
    	}

    	// Close the directory
    	closedir(dir);
    	
    	return;
}

int return_index(char *filename)
{
	int file_index=-1;
	
	for (int i = 0; i < file_count; i++) 
        {
                if (strcmp(files[i].filename, filename) == 0) 
                {
                    	file_index = i;
                    	break;
                }
        }
        
        return file_index;
}

int return_user_id(char* username)
{
	for(int i = 0; i< user_count; i++)
	{
		if(strcmp(users[i].username, username) == 0)
		{
			return i;
		}
	}
	return -1;
}


void notify(char* filename)
{
	for(int i = 0; i< user_count; i++)
	{
		for(int j = 0; j < users[i].checked_out_count; j++)
		{
			if(strcmp(users[i].checked_out_files[j], filename) == 0)
			{
				users[i].notified = false;
			}
		}
		
	}
}


int add_checked_out_file(int user_id, const char *filename) 
{
    	if (users[user_id].checked_out_count < MAX_CHECKED_OUT_FILES) 
    	{
        	// Allocate memory for the new filename
        	users[user_id].checked_out_files[users[user_id].checked_out_count] = malloc(strlen(filename) + 1);
        	
        	if (users[user_id].checked_out_files[users[user_id].checked_out_count] == NULL) 
        	{
        		return -1;
        	}
        	
        	strcpy(users[user_id].checked_out_files[users[user_id].checked_out_count], filename);
        	users[user_id].checked_out_count++;
        	return 0; // Success
    	}
    	return -1; // Max limit reached
}

void remove_checked_out_file(int user_id, const char *filename) 
{
	for (int i = 0; i < users[user_id].checked_out_count; i++) 
    	{
        	if (strcmp(users[user_id].checked_out_files[i], filename) == 0) 
        	{
            		free(users[user_id].checked_out_files[i]); // Free the memory
            		
            		// Shift remaining files
            		for (int j = i; j < users[user_id].checked_out_count - 1; j++) 
            		{
                		users[user_id].checked_out_files[j] = users[user_id].checked_out_files[j + 1];
            		}
            		users[user_id].checked_out_files[users[user_id].checked_out_count - 1] = NULL;
            		users[user_id].checked_out_count--;
            		break;
        	}
    	}
}

void read_file(int client_socket, char *filename)
{
	int file_index = return_index(filename);
	
        if (file_index != -1) 
        {	
                char path[FILE_NAME_SIZE] = "./files/";
                strcat(path, filename);
                		
                FILE *f = fopen(path, "r");
                		
                fseek(f, 0, SEEK_END);
                int f_size = ftell(f);
                fseek(f, 0, SEEK_SET);
                
                if(f_size == 0)
                {
                	send(client_socket, "FILE IS EMPTY", 13, 0);
                	return; 
                }
                		
                char *read_buff = (char *)malloc(f_size+1);
                fread(read_buff, sizeof(char), f_size, f);
                read_buff[f_size] = '\0';
                		
                send(client_socket, read_buff, f_size, 0);
                free(read_buff);
        } 
        else 
        {
                send(client_socket, "FILE_NOT_FOUND", 15, 0);
        }
}

void check_out_file(int client_socket, char* filename, char* username)
{
	int file_index = return_index(filename);
	
	if(file_index != -1)
	{
		int user_id = return_user_id(username);
		
		if(user_id != -1)
		{
			read_file(client_socket, filename);
			add_checked_out_file(user_id, filename);
		}
	}
}


void write_file(int client_socket, char* filename, char choice, char* content)
{
	int file_index = return_index(filename);
	
	if(file_index != -1)
	{
		char path[FILE_NAME_SIZE] = "./files/";
                strcat(path, filename);
                
                if(choice == 'a' || choice == 'A')
                {
                	FILE *f = fopen(path, "a");
                	fprintf(f, "%s\n", content);
                	fclose(f);	
                }
                		
                if(choice == 'o' || choice == 'O')
                {
                	FILE *f = fopen(path, "w");
                	fprintf(f, "%s\n", content);
                	fclose(f);	
                }
                
                if(choice == 'c' || choice == 'C')
                {
                	FILE *f = fopen(path, "w");
                	fprintf(f, "%s\n", content);
                	fclose(f);
                	
                	send(client_socket, "COMMIT SUCCESSFUL", 17, 0);
                	return;	
                }
                
                send(client_socket, "WRITE SUCCESSFUL", 16, 0);
                notify(filename);
                
	}
	else
	{
		send(client_socket, "FILE_NOT_FOUND", 15, 0);
	}
	return;
}


void commit_file(int client_socket, char* filename, char choice, char * content, char* username)
{
	int file_index = return_index(filename);
	
	if(file_index != -1)
	{
		int user_id = return_user_id(username);
		
		if(user_id != -1)
		{
			write_file(client_socket, filename, choice, content);
			remove_checked_out_file(user_id, filename);
		}
	}
}


void delete_file(int client_socket, char* filename)
{
	int file_index = return_index(filename);
        
        if (file_index != -1) 
        {
                char path[FILE_NAME_SIZE] = "./files/";
                strcat(path, filename);
                
                remove(path);
                printf("File %s deleted\n", filename);
                
                for (int i = file_index; i < file_count - 1; i++) 
                {
                    	files[i] = files[i + 1];
                }
                file_count--;
                send(client_socket, "DELETE_SUCCESS", 14, 0);
        } 
        else 
        {
                send(client_socket, "FILE_NOT_FOUND", 14, 0);
        }
        return;
}

void create_file(int client_socket, char* filename)
{
	int file_index = return_index(filename);
        
        if (file_index == -1) 
        {
        	char path[FILE_NAME_SIZE] = "./files/";
                strcat(path, filename);
                
                printf("Create Path: %s\n", path);
                
                FILE *f = fopen(path, "w");
                
                if(f == NULL)
                {
                	perror("Error in creating File");
                	return;
                }
                
                strcpy(files[file_count].filename, filename);
     		file_count++;
     		fclose(f);
     		
     		send(client_socket, "CREATE_SUCCESS", 14, 0);
        }
        else
        {
        	send(client_socket, "FILE ALREADY EXISTS", 19, 0);
        }
}


void *handle_client(void *arg) 
{
	int client_socket = *(int *)arg;
    	char buffer[BUFFER_SIZE];
    	int client_id = -1; // To identify the client
    	char username[USERNAME_SIZE];
    	
    	while (1) 
    	{	
        	memset(buffer, 0, BUFFER_SIZE);
        	int n = recv(client_socket, buffer, BUFFER_SIZE, 0);
        	if (n <= 0) 
        	{
            		printf("Client %d disconnected.\n", client_id);
            		break;
        	}

        	// Handle commands: REGISTER, LOGIN, CHECKOUT, COMMIT, READ, WRITE, DELETE, LIST
        	if (strncmp(buffer, "REGISTER", 8) == 0) 
        	{
            		char password[PASSWORD_SIZE];
            		sscanf(buffer + 9, "%s %s", username, password);
            
            		// Check if user already exists
            		for (int i = 0; i < user_count; i++) 
            		{
                		if (strcmp(users[i].username, username) == 0) 
                		{
                    			send(client_socket, "USER_EXISTS", 12, 0);
                    			goto end;
                		}
            		}

            		// Register new user
            		strcpy(users[user_count].username, username);
            		strcpy(users[user_count].password, password);
            		users[user_count].checked_out_count = 0;
            		users[user_count].notified = false;
            		user_count++;
            		send(client_socket, "REGISTER_SUCCESS", 16, 0);
            		client_id = client_socket;
            		continue;

        	} 
        	else if (strncmp(buffer, "LOGIN", 5) == 0) 
        	{
            		//printf("Client Sent: %s\n", buffer);
            		char password[PASSWORD_SIZE];
            		sscanf(buffer + 6, "%s %s", username, password);
            		
            		// Simple user authentication
            		for (int i = 0; i < user_count; i++) 
            		{
                		if (strcmp(users[i].username, username) == 0 && 
                			strcmp(users[i].password, password) == 0) 
                		{
                    			client_id = i; // Store client id based on user
                    			send(client_socket, "LOGIN_SUCCESS", 13, 0);
                    			users[i].notified = false;
                    			client_id = client_socket;
                    			goto end;
                		}
            		}
            		send(client_socket, "LOGIN_FAILED", 12, 0);
            		continue;

        	} 
        	else if (client_id == -1) 
        	{
            		send(client_socket, "NOT_LOGGED_IN", 14, 0);
            		continue;
        	}

        	//CHECKOUT
        	if (strncmp(buffer, "CHECKOUT", 8) == 0) 
        	{
            		printf("Server: In Checkout\n");
            		
            		char filename[FILE_NAME_SIZE];
            		sscanf(buffer + 9, "%s", filename);

            		pthread_mutex_lock(&file_lock);
            		
            		check_out_file(client_socket, filename, username);
            		
            		pthread_mutex_unlock(&file_lock);
        	}
        	//COMMIT 
        	else if (strncmp(buffer, "COMMIT", 6) == 0) 
        	{
            		printf("Server: In Commit\n");
            		
            		char filename[FILE_NAME_SIZE];
            		char choice;
            		char content[BUFFER_SIZE];
            		sscanf(buffer + 6, "%s %s %[^\n]", filename, &choice, content);

            		pthread_mutex_lock(&file_lock);
            		
            		write_file(client_socket, filename, choice, content);
            		
            		pthread_mutex_unlock(&file_lock);
        	}
        	//READ 
        	else if (strncmp(buffer, "READ", 4) == 0) 
        	{
            		printf("Server: In Read\n");
            		
            		char filename[FILE_NAME_SIZE];
            		sscanf(buffer + 5, "%s", filename);

            		pthread_mutex_lock(&file_lock);
            		
            		read_file(client_socket, filename);
            		
            		pthread_mutex_unlock(&file_lock);
        	} 
        	//WRITE
        	else if (strncmp(buffer, "WRITE", 5) == 0) 
        	{
            		printf("Server: In Write\n");
            		
            		char filename[FILE_NAME_SIZE];
            		char choice;
            		char content[BUFFER_SIZE];
            		sscanf(buffer + 6, "%s %s %[^\n]", filename, &choice, content);
            		
            		pthread_mutex_lock(&file_lock);
            		
            		commit_file(client_socket, filename, choice, content, username);
            		
            		pthread_mutex_unlock(&file_lock);
            		
        	}
        	//DELETE
        	else if (strncmp(buffer, "DELETE", 6) == 0) 
        	{
            		printf("Server: In Delete\n");
            		
            		char filename[FILE_NAME_SIZE];
            		sscanf(buffer + 7, "%s", filename);

            		pthread_mutex_lock(&file_lock);
            		delete_file(client_socket, filename);
            		pthread_mutex_unlock(&file_lock);
        	}
        	//CREATE
        	else if (strncmp(buffer, "CREATE", 6) == 0) 
        	{
            		printf("Server: In Create\n");
            		
            		char filename[FILE_NAME_SIZE];
            		sscanf(buffer + 7, "%s", filename);

            		pthread_mutex_lock(&file_lock);
            		create_file(client_socket, filename);
            		pthread_mutex_unlock(&file_lock);
        	}
        	//LIST 
        	else if (strncmp(buffer, "LIST", 4) == 0) 
        	{
            		printf("Server: In List\n");
            		
            		pthread_mutex_lock(&file_lock);
            		char list_buffer[BUFFER_SIZE] = "Available files:\n";
            		for (int i = 0; i < file_count; i++) 
            		{
                		strcat(list_buffer, files[i].filename);
                		strcat(list_buffer, "\n");
            		}
            		pthread_mutex_unlock(&file_lock);
            		send(client_socket, list_buffer, strlen(list_buffer), 0);
        	}
        	
        	for(int i = 0; i < user_count; i++)
        	{
        		if((strcmp(username, users[i].username) == 0) && (!users[i].notified))
        		{
        			send(client_socket, "Your Checked Out Files have been modified", 41, 0);
        			break;
        		}
        	}
        
    		end:
        	continue;
    	}

    	close(client_socket);
    	return NULL;
}

int main() 
{
	int server_socket, client_socket;
	struct sockaddr_in server_addr, client_addr;
    	socklen_t client_len = sizeof(client_addr);
    	pthread_t tid;

    	// Initialize some users
    	strcpy(users[user_count].username, "user1");
    	strcpy(users[user_count].password, "pass1");
    	user_count++;
    	strcpy(users[user_count].username, "user2");
    	strcpy(users[user_count].password, "pass2");
    	user_count++;
    	
    	initialize_files();
    	
    	server_socket = socket(AF_INET, SOCK_STREAM, 0);
    	server_addr.sin_family = AF_INET;
    	server_addr.sin_addr.s_addr = INADDR_ANY;
    	server_addr.sin_port = htons(PORT);

    	bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    	listen(server_socket, MAX_CLIENTS);
	
    	printf("Server listening on port %d...\n", PORT);

    	while ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len)) >= 0) 
    	{
        	pthread_create(&tid, NULL, handle_client, (void *)&client_socket);
    	}

    	close(server_socket);
    	return 0;
}
