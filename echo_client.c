#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<error.h>
#include<strings.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<string.h>

#define ERROR -1
#define BUFFER 1024

main(int argc, char **argv)
{
	struct sockaddr_in remote_server;
	int sfd;
	char input[BUFFER];
	char output[BUFFER];
	int len;

	if((sfd=socket(AF_INET, SOCK_STREAM,0))==ERROR)
	{
		perror("socket");
		exit(-1);

	}
	remote_server.sin_family = AF_INET;
	remote_server.sin_port = htons(atoi(argv[2]));
	remote_server.sin_addr.s_addr = inet_addr(argv[1]);
	bzero(&remote_server.sin_zero,8);

	if((connect(sfd,(struct sockaddr *)&remote_server,sizeof(struct sockaddr_in)))==ERROR)
	{
		perror("connect");
		exit(-1);
	}
	while(1)
	{
		fgets(input,BUFFER,stdin);
		send(sfd,input,strlen(input),0);
		len = recv(sfd,output, BUFFER,0);
		output[len]='\0';
		printf("%s\n",output);
	}
	close(sfd);
}