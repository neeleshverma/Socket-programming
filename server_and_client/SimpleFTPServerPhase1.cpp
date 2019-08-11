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
	if (argc != 3)
	{
		fprintf(stderr, "%s <portNum> <fileToTransfer>\n",argv[0]);
		exit(1);
	}
	int sockfd,new_fd;
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
	FILE *fp = fopen(argv[2], "rb");
	/* File not found */
	if (fp == NULL)
	{
		fprintf(stderr, "%s%s\n", "Unable to open or Read File : ", argv[2]);
		close(sockfd);
		exit(3);
	}
	int fSize;
	fseek(fp, 0, SEEK_END);
	fSize = ftell(fp);
	if (fSize < 0)
	{
		fprintf(stderr, "%s\n", "Unable to open or read file");
		close(sockfd);
		exit(3);
	}
	rewind(fp);
	char *msg = (char *) malloc(fSize);
	if (fread(msg, fSize, 1, fp) != 1)
	{
		fprintf(stderr, "%s\n", "Unable to open or read file");
		close(sockfd);
		exit(3);
	}
	fclose(fp);
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
	new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
	/* Accept Failed */
	if (new_fd < 0)
	{
		fprintf(stderr, "%s\n", "Accept Failed");
		close(sockfd);
		close(new_fd);
		exit(2);
	}
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
	close(sockfd);
	close(new_fd);
	free(msg);
}
