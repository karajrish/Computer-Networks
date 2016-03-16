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

void * read_thread_func(void * arg){
    int sfd = *((int *)arg);
    char buff[20];
    while(read(sfd,buff,20) != 0){
        cout<<"Message Received: "<<buff<<endl;
    }
}

int main(int argc,char ** arg){
    pthread_t read_thread;
    struct sockaddr_in my_addr;
    int sfd = socket(AF_INET,SOCK_STREAM,0);
    if(sfd < 0)
        perror("Socket error: ");
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = INADDR_ANY;
    my_addr.sin_port = htons(6000);
    int c = connect(sfd,(struct sockaddr*)&my_addr,sizeof(my_addr));
    if(c < 0)
        perror("Connection Error: ");
    cout<<"Connection Established with the server\n";
    char buff[20];
    pthread_create(&read_thread,NULL,read_thread_func,(void * )&sfd);
    while(1){
        char buff[20];
        cin>>buff;
        write(sfd,buff,20);
        cout<<"Message Sent..\n";
    }
}
