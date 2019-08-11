#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define BACKLOG 1
int main(int argc, char* argv[])
{
	/* Wrong number of arguments given */
	if (argc != 2)
	{
		fprintf(stderr, "%s <portNum>\n",argv[0]);
		exit(1);
	}
	int sockfd, new_fd;
	sockfd = socket(PF_INET, SOCK_STREAM, 0);
	/* Socket Failed */
	if (sockfd < 0)
	{
		fprintf(stderr, "%s\n", "Socket Failed");
		exit(2);
	}
	int portNum = atoi(argv[1]);
	struct sockaddr_in my_addr; // Server's sockaddr information
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(portNum);
	my_addr.sin_addr.s_addr = INADDR_ANY;
	memset(&(my_addr.sin_zero), '\0', 8);
	unsigned int sin_size;
	struct sockaddr_in their_addr; // Client's sockaddr information
	int bindRes = bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr));
	/* Bind Failed */
 	if (bindRes < 0)
	{ 
		fprintf(stderr, "%s%d\n", "Bind Failed on portNum: ", portNum);
		exit(2);
	}
	printf("%s%d\n", "Bind Done: ", portNum);
	int listenRes = listen(sockfd, BACKLOG);
	/* Listen Failed */
	if (listenRes < 0)
	{
		fprintf(stderr, "%s\n", "Listen Failed");
		close(sockfd);
		exit(2);
	}
	printf("%s%d\n", "Listen Done: ", portNum);
	sin_size = sizeof(struct sockaddr_in);
	while (true)
	{
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		/* Accept Failed */
		if (new_fd < 0)
		{
			fprintf(stderr, "%s\n", "Accept Failed");
			close(new_fd);
			continue;
		}
		printf("%s%s%s%d\n", "Client: ", inet_ntoa(their_addr.sin_addr), ":", their_addr.sin_port);
		// char* buffer = (char* ) malloc(1000);
		char* filestring = (char* ) malloc(10000);
		char* fileName = (char* ) malloc(10000);
		int recvRes = recv(new_fd, filestring, 10000, 0);
		if (recvRes < 0)
		{
			printf("%s\n", "Connection to Client failed");
			close(new_fd);
			continue;
		}
		if (filestring[0] != 'g')
		{
			printf("%s\n", "UnknownCmd");
			fprintf(stderr, "%s\n", "Wrong Format");
			close(new_fd);
			continue;
		}
		if (filestring[1] != 'e')
		{
			printf("%s\n", "UnknownCmd");
			fprintf(stderr, "%s\n", "Wrong Format");
			close(new_fd);
			continue;				
		}
		if (filestring[2] != 't')
		{
			printf("%s\n", "UnknownCmd");
			fprintf(stderr, "%s\n", "Wrong Format");
			close(new_fd);
			continue;
		}
		if (filestring[3] != ' ')
		{
			printf("%s\n", "UnknownCmd");
			fprintf(stderr, "%s\n", "Wrong Format");
			close(new_fd);
			continue;
		}
		int i = 4;
		while ((filestring[i] != '\0')&&(i < 10000))
		{
			fileName[i-4] = filestring[i];
			i++;
		}
		if (i == 10000)
		{
			printf("%s\n", "UnknownCmd");
			fprintf(stderr, "%s\n", "No endline character");
			close(new_fd);
			continue;
		}
		fileName[i] = '\0';
		printf("%s%s\n", "File Requested: ", fileName);
		FILE *fp = fopen(fileName, "rb");
		/* File not found */
		if (fp == NULL)
		{
			printf("%s\n", "File Transfer Fail");
			fprintf(stderr, "%s%s\n", "Unable to find requested file : ", fileName);
			close(new_fd);
			continue;
		}
		int fSize;
		fseek(fp, 0, SEEK_END);
		fSize = ftell(fp);
		if (fSize < 0)
		{
			fprintf(stderr, "%s\n", "Unable to read file");
			close(new_fd);
			continue;
		}
		rewind(fp);
		char *msg = (char *) malloc(fSize);
		if (fread(msg, fSize, 1, fp) != 1)
		{
			fprintf(stderr, "%s\n", "Unable to open or read file");
			close(new_fd);
			continue;
		}
		fclose(fp);
		printf("%s%s%s%d\n", "Client: ", inet_ntoa(their_addr.sin_addr), ":", their_addr.sin_port);
		int bytes_sent = send(new_fd, msg, fSize, 0);
		if (bytes_sent == fSize)
		{
			printf("%s%d%s\n", "TransferDone: ", fSize, " bytes");
		}
		else
		{
			printf("%s\n", "TransferFailed");
			fprintf(stderr, "%s\n", "file couldn't transferred successfully");	
		}
		close(new_fd);
	}
}
