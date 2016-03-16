#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<errno.h>

main(){

	int sfd, nsfd;
	//sfd : socket descriptor for server
	//nsfd :  socket descriptor for new client
	//server structure given by us and client structure returned by accept()
	struct sockaddr_in server, client;
	unsigned int sent; //no. of bytes sent
	char mesg[] = "Hello there!";
	if((sfd = socket(AF_INET, SOCK_STREAM,0))==-1)
	{
		perror("socket: ");
		exit(-1);
	} 
	//assigning ip address and port address
	server.sin_family = AF_INET;
	server.sin_port = htons(10000);
	
	server.sin_addr.s_addr = INADDR_ANY;
	//INADDR_ANY should bind all available interfaces on the local machine
	
	//for padding the rest of the structure
	bzero(&server.sin_zero,8);
	unsigned int len;
	//returns the length of the structure
	len = sizeof(struct sockaddr_in);
	
	//binding the ip address and port address to the socket
	if((bind(sfd, (struct sockaddr *)&server, len))==-1){
		perror("bind");
		exit(-1);

	}

	//now we listen for connections
	if((listen(sfd,5))==-1){
		perror("listen:");
		exit(-1);
	}

	//now we wait for connections (more than one)

	while(1){
			if((nsfd=accept(sfd,(struct sockaddr *)&client,&len))==-1){
				perror("accept:");
				exit(-1);
			}

			sent = send(nsfd,mesg,strlen(mesg),0);
			printf("Sent %d bytes to client : %s\n",sent,inet_ntoa(client.sin_addr));
			close(nsfd);
	}

}
 