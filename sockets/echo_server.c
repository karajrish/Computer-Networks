#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<error.h>
#include<strings.h>
#include<unistd.h>
#include<arpa/inet.h>

#define ERROR -1 
#define MAX_CLIENTS 2 //defining the maximum no. of clients to connect
#define MAX_DATA 1024 //maximum size of send and recieve strings

main(int argc, char **argv)
{
	struct sockaddr_in server; //socket for server
	struct sockaddr_in client; // socket for client
	int sfd,nsfd; //server fd and client fd(new)
	int sockaddr_len = sizeof(struct sockaddr_in);  //length to be passed as argument
	int data_len; //length of string
	char data[MAX_DATA]; //data

	//starting the server process

	if((sfd = socket(AF_INET, SOCK_STREAM, 0))== ERROR) //creates a socket for server <internet, TCP,0>
	{
		perror("server socket: ");
		exit(-1);
	}

	server.sin_family = AF_INET;  //Type Internet
	server.sin_port = htons(atoi(argv[1])); //port taken on input
	server.sin_addr.s_addr = INADDR_ANY; //0.0.0.0 all interfaces on the local machine
	bzero(&server.sin_zero, 0); //zeros all remaining buffer

	if((bind(sfd, (struct sockaddr *)&server, sockaddr_len))==ERROR) //binds the socket to the port and address
	{
		perror("bind : ");
		exit(-1);
	}

	if((listen(sfd,MAX_CLIENTS))==ERROR) //assigns max clients that can connect
	{
		perror("listen");
		exit(-1);
	}

	while(1)  //better signal handling required
	{
		//till client connects it will wait
		if((nsfd = accept(sfd, (struct sockaddr *)&client, &sockaddr_len))== ERROR)
		{
			perror("accept");
			exit(-1);
		}
		printf("New client connected from port no %d and IP %s\n", ntohs(client.sin_port), inet_ntoa(client.sin_addr));

		data_len = 1;
		while(data_len)
		{
			data_len = recv(nsfd,data,MAX_DATA,0);
			if(data_len)
			{
				send(nsfd,data,data_len,0);
				data[data_len]='\0';
				printf("Sent mesg : %s",data);
			}
		}
		printf("Client disconnected\n");
		close(nsfd);
	}

	close(sfd);

}