#include<iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include<cstdlib>
#include<string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<unistd.h>

using namespace std;

struct Message{
    int op1;
    int op2;
    int opr;
};

int main(){
    struct sockaddr_in s_addr;
    int sfd = socket(AF_INET,SOCK_STREAM,0);
    if(sfd < 0)
        perror("Socket: ");

    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(2555);
    s_addr.sin_addr.s_addr = INADDR_ANY;

    bind(sfd,(struct sockaddr *)&s_addr,sizeof(s_addr));
    listen(sfd,10);

    while(1){
        cout<<"Waiting for connection..\n";
        struct sockaddr_in client;
        socklen_t clen = sizeof(client);
        int nsfd = accept(sfd,(struct sockaddr *)&client,&clen);
        cout<<"Connection established with "<<inet_ntoa(client.sin_addr)<<endl;
        int pid = fork();
        if(pid == 0){
            dup2(nsfd,0);
            execl("prog","prog",NULL,NULL);
        }
    }

}
