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

int sfd;
struct Message{
    int op1;
    int op2;
    int opr;
    int rslt;
};

void * read_func(void * arg){
    struct Message msg;

    while(read(sfd,&msg,sizeof(struct Message)) != 0){
        cout<<"Message Received: "<<msg.rslt<<endl;
    }
}

void * write_func(void * arg){
    struct Message msg;

    while(1){
        cout<<"Operator1: ";cin>>msg.op1;
        cout<<"Operator2: ";cin>>msg.op2;
        cout<<"Byte Sent: "<<write(sfd,&msg,sizeof(struct Message))<<endl;
    }
}

int main(){
    sfd = socket(AF_INET,SOCK_STREAM,0);
    if(sfd < 0)
        perror("Socket: ");
    struct sockaddr_in s_addr;
    s_addr.sin_port = htons(2555);
    s_addr.sin_addr.s_addr = INADDR_ANY;
    s_addr.sin_family = AF_INET;
    int c = connect(sfd,(struct sockaddr *)&s_addr,sizeof(s_addr));
    cout<<"Connected...\n";
    pthread_t read_thread,write_thread;
    pthread_create(&read_thread,NULL,read_func,NULL);
    pthread_create(&write_thread,NULL,write_func,NULL);
    pthread_join(read_thread,NULL);
    return 0;
}
