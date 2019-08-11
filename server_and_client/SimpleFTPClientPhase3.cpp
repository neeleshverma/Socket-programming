#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>


int main(int argc, char *argv[])
{
	/* Wrong number of Arguments given*/
	if (argc != 4)
	{
		fprintf(stderr, "%s%s\n", "<serverIPAddr:port> <fileToReceive> <receiveInterval>", argv[0]);
		exit(1);
	}
	char port[50];
	char serverIPAddr[100];
	int i = 0;
	while (argv[1][i] != ':')
	{
		serverIPAddr[i] = argv[1][i];
		i++;
	}
	serverIPAddr[i] = '\0';
	i++;
	int j = 0;
	while (argv[1][i] != '\0')
	{
		port[j] = argv[1][i];
		i++;
		j++;
	}
	port[j] = '\0';
	int portNum = atoi(port);
	int sockfd;
	sockfd = socket(PF_INET, SOCK_STREAM, 0);
	/* Socket Failed */
	if (sockfd < 0)
	{
		fprintf(stderr, "%s\n", "Socket Failed");
		exit(2);
	}
	FILE *fp = fopen(argv[2], "w+");
	if (fp == NULL)
	{
		fprintf(stderr, "%s\n", "Unable to create the given file");
		close(sockfd);
		exit(3);
	}
	struct sockaddr_in server_addr; // Server's sockaddr information
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(portNum);
	inet_aton(serverIPAddr,&(server_addr.sin_addr));
	// server_addr.sin_addr.s_addr = inet_addr(serverIPAddr);
	memset(&(server_addr.sin_zero), '\0', 8);
	int connectRes = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
	if (connectRes < 0)
	{
		fprintf(stderr, "%s\n", "Connection to server failed");
		close(sockfd);
		exit(2);
	}
	printf("%s%s%s%d\n", "ConnectDone: ", serverIPAddr, " : ", portNum);
	char msg[10000];
	msg[0] = 'g';
	msg[1] = 'e';
	msg[2] = 't';
	msg[3] = ' ';
	strcat(msg,argv[2]);
	//msg[strlen(argv[3])+4] = '\0';
	int bytes_sent = send(sockfd, &msg, strlen(argv[2])+5, 0);
	if (bytes_sent == strlen(argv[2])+5)
	{
		printf("%s%s%s\n", "string ", msg, " transferred successfully");
	}
	else
	{
		fprintf(stderr, "%s\n", "string couldn't transferred successfully");
		close(sockfd);
		exit(2);
	}
	char buff[1000];
	int fSize;
	int recvRes = recv(sockfd, buff, 1000, 0);
	int writtenBytes;
	while (recvRes >= 0)
	{
		usleep(atoi(argv[3])*1000);
		if (recvRes == 0)
		{
			fseek(fp, 0, SEEK_END);
			fSize = ftell(fp);
			if (fSize < 0)
			{
				close(sockfd);
				fprintf(stderr, "%s\n", "Unable to read file");
				exit(3);
			}
			else
			{
				printf("%s%d%s\n", "file written: ", fSize, "Bytes");
				fclose(fp);
				close(sockfd);
				break;
			}
		}
		else
		{
			fwrite(buff, sizeof(char), recvRes, fp);
			recvRes = recv(sockfd, buff, 1000, 0);
			// printf("%d\n", recvRes);
		}
	}
	if (recvRes < 0)
	{
		fprintf(stderr, "%s\n", "Connection to server failed");
		close(sockfd);
		fclose(fp);
		exit(2);
	}
}
