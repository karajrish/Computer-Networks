#include<iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include<cstdlib>
#include<string.h>
#include <netdb.h>
#include <netinet/in.h>
#include<unistd.h>
#include <arpa/inet.h>
using namespace std;

int main(){
    struct sockaddr_in my_addr;
    int sfd = socket(AF_INET,SOCK_STREAM,0);
    if(sfd < 0)
        perror("Socket error: ");
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = INADDR_ANY;
    my_addr.sin_port = htons(2555);
    int c = connect(sfd,(struct sockaddr*)&my_addr,sizeof(my_addr));
    if(c < 0)
        perror("Connection Error: ");
    cout<<"Connection Established with the server\n";
    char buff[] = "Hello..\n";
    write(sfd,buff,sizeof(buff));
    while(read(sfd,buff,sizeof(buff)) != 0){
        cout<<"Message Recieved: "<<buff<<endl;
        sleep(4);
        write(sfd,buff,20);
    }
}
