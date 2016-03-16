#include<iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include<cstdlib>
#include<string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<unistd.h>
#include<poll.h>
using namespace std;


int sfd1,sfd2;
int my_pipe1[2];
int my_pipe2[2];

void * service1(void * arg){
    int fd = *(int *)arg;
    cout<<"service 1 started..\n";
    char buff[20];
    while(read(fd,buff,20) != 0){
        cout<<"Service 1 received: "<<buff<<endl;
        write(fd,buff,20);
    }
    cout<<"Client Leaving..\n";
}

void * service2(void * arg){
    int fd = *(int *)arg;
    cout<<"service 2 started..\n";
    char buff[20];
    while(read(fd,buff,20) != 0){
        cout<<"Service 2 received: "<<buff<<endl;
        write(fd,buff,20);
    }
    cout<<"Client Leaving..\n";
}

void * port1_listener(void * arg){
    bool flag = false;
    int portno = 2555;
    struct sockaddr_in s_addr;
    if(sfd1 < 0)
        perror("Error socket: ");
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(portno);
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    int b = bind(sfd1,(struct sockaddr *)&s_addr,sizeof(s_addr));
    listen(sfd1,10);
        while(1){
            struct sockaddr_in c_addr;
            cout<<"Waiting for connection..\n";
            socklen_t cilen = sizeof(c_addr);
            int * nsfd = new int;
            *nsfd = accept(sfd1,(struct sockaddr *)&c_addr,&cilen);
            cout<<"Port no: "<<portno<<" "<<endl;
            cout<<"Connection Established with" <<inet_ntoa(c_addr.sin_addr)<<endl;
            pthread_t * service_thread = new pthread_t();
            pthread_create(service_thread,NULL,service1,(void *)nsfd);
        }
}

void * port2_listener(void * arg){
    int portno = 2556;
    struct sockaddr_in s_addr;
    //int sfd = socket(AF_INET,SOCK_STREAM,0);
    if(sfd2 < 0)
        perror("Error socket: ");
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(portno);
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    int b = bind(sfd2,(struct sockaddr *)&s_addr,sizeof(s_addr));
    listen(sfd2,10);
    while(1){
            struct sockaddr_in c_addr;
            cout<<"Waiting for connection..\n";
            socklen_t cilen = sizeof(c_addr);
            int * nsfd = new int;
            *nsfd = accept(sfd2,(struct sockaddr *)&c_addr,&cilen);

            if(*nsfd < 0)
                perror("Accept: ");

            cout<<"Port no: "<<portno<<" ";
            cout<<"Connection Established with" <<inet_ntoa(c_addr.sin_addr)<<endl;
            pthread_t * service_thread = new pthread_t();
            pthread_create(service_thread,NULL,service2,(void *)nsfd);
    }
}


int main(){
    sfd1 = socket(AF_INET,SOCK_STREAM,0);
    sfd2 = socket(AF_INET,SOCK_STREAM,0);

    pthread_t port1_thread,port2_thread,poll_thread;
    pthread_create(&port1_thread,NULL,port1_listener,NULL);
    pthread_create(&port2_thread,NULL,port2_listener,NULL);

    pthread_join(port1_thread,NULL);
    pthread_join(port2_thread,NULL);
}
