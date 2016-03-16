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

int main(int argc,char ** argv){
    struct sockaddr_in s_addr;
    int sfd = socket(AF_INET,SOCK_STREAM,0);
    if(sfd < 0)
        perror("Error socket: ");
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(2555);
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    int b = bind(sfd,(struct sockaddr *)&s_addr,sizeof(s_addr));
    listen(sfd,10);
    while(1){
        struct sockaddr_in c_addr;
        cout<<"Waiting for connection..\n";
        socklen_t cilen = sizeof(c_addr);
        int nsfd = accept(sfd,(struct sockaddr *)&c_addr,&cilen);
        cout<<"Connection Established with" <<inet_ntoa(c_addr.sin_addr)<<endl;
        int pid = fork();
        if(pid == 0){
            close(sfd);
            cout<<"Child server working..\n";
            char buff[20];
            while(read(nsfd,buff,sizeof(buff)) != 0){
                cout<<"Message Recieved: "<<buff<<endl;
                write(nsfd,buff,sizeof(buff));
                sleep(2);
            }
            exit(0);
        }
        else{
            close(nsfd);
        }
    }
    return 0;
}
