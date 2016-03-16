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
#include<map>
#include<semaphore.h>
#include<signal.h>
using namespace std;

int sfd1,sfd2;
map<int,int> service_map;
sem_t limit_proc1;
sem_t limit_proc2;
void * port1_listener(void * arg){
    bool flag = false;

    sem_init(&limit_proc1,0,2);
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
            int nsfd = accept(sfd1,(struct sockaddr *)&c_addr,&cilen);
            cout<<"Port no: "<<portno<<" ";
            cout<<"Connection Established with" <<inet_ntoa(c_addr.sin_addr)<<endl;
            sem_wait(&limit_proc1);
            int pid = fork();
            if(pid == 0){
                dup2(nsfd,0);
                cout<<"child created..\n";
                execl("service","service",NULL,NULL);
            }
            else{
                service_map.insert(pair<int,int>(pid,1));
            }
        }
}

void * port2_listener(void * arg){
    int portno = 2556;
    struct sockaddr_in s_addr;
    //int sfd = socket(AF_INET,SOCK_STREAM,0);

    sem_init(&limit_proc2,0,3);
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
        int nsfd = accept(sfd2,(struct sockaddr *)&c_addr,&cilen);
        cout<<"Port no: "<<portno<<" ";
        cout<<"Connection Established with" <<inet_ntoa(c_addr.sin_addr)<<endl;
        //write(pipew2[1],(void*)&nsfd,sizeof(int));
        int pid = fork();
        if(pid == 0){
            cout<<"child created..\n";
            dup2(nsfd,0);
            execl("service","service",NULL,NULL);
        }
        else{
            service_map.insert(pair<int,int>(pid,2));
        }
    }
}

void handler(int signo,siginfo_t * info,void * vp){
    cout<<"Sent by: "<<info->si_pid<<endl;
    if(service_map.find(info->si_pid) != service_map.end()){
        int service_no = service_map.find(info->si_pid)->second;

        if(service_no == 1){
            sem_post(&limit_proc1);
            cout<<"Service 1 freed..\n";
        }
        else{
            sem_post(&limit_proc2);
            cout<<"Service 2 freed..\n";
        }
    }
}

int main(){
     struct sigaction action;
    action.sa_flags = SA_SIGINFO;
    action.sa_sigaction = handler;
    if(sigaction(SIGCHLD,&action,NULL) < 0)
        perror("Singal : ");

    sfd1 = socket(AF_INET,SOCK_STREAM,0);
    sfd2 = socket(AF_INET,SOCK_STREAM,0);

    pthread_t port1_thread,port2_thread,poll_thread;
    pthread_create(&port1_thread,NULL,port1_listener,NULL);
    pthread_create(&port2_thread,NULL,port2_listener,NULL);
    pthread_join(port1_thread,NULL);


}
