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
#include<vector>
#include<fcntl.h>
#include<map>
#include<signal.h>
#include<error.h>
using namespace std;

vector<struct pollfd> fdList;
vector<int> cList;
map<int,int> pList;
vector<int> outSourced;
void broadcast(char * buff,int size){
    for(int i = 0;i<cList.size();i++){
        write(cList[i],buff,size);
    }
}

void handler(int signo,siginfo_t * info,void * vp){
    if(signo == SIGUSR1){
        cout<<"Signal Received..\n";
        int client = cList[0];
        cList.erase(cList.begin());
        outSourced.push_back(client);
        int pid = fork();
        if(pid == 0){
            cout<<"Dispatched a client to echo server..\n";
            dup2(client,0);
            execl("echo","echo",NULL,NULL);
        }
        else{
            pList.insert(pair<int,int>(pid,client));
        }
    }
    else if(signo == SIGCHLD){
        cout<<"Serving client after echo server has finished its work..\n";
        int child_pid = info->si_pid;
        int client = pList.find(child_pid)->second;
        cList.push_back(client);
    }
}

int main(){

    struct sigaction action;
    action.sa_flags = SA_SIGINFO;
    action.sa_sigaction = handler;
    if(sigaction(SIGCHLD,&action,NULL) < 0)
        perror("Singal : ");
     if(sigaction(SIGUSR1,&action,NULL) < 0)
        perror("Singal : ");
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(6000);
    addr.sin_addr.s_addr = INADDR_ANY;

    FILE * pfd = popen("./prog","r");
    FILE * ffd = popen("./prog","r");
    FILE * fd = popen("./prog","r");
    if(pfd == NULL || ffd == NULL || fd == NULL)
        perror("FD:");

   // cout<<buff;

    struct pollfd ppfd;
    ppfd.fd = fileno(pfd);
    ppfd.events = POLLIN | POLLHUP | POLLRDNORM;
    ppfd.revents = 0;
    fdList.push_back(ppfd);

     struct pollfd ppfd1;
    ppfd1.fd = fileno(ffd);
    ppfd1.events = POLLIN | POLLHUP | POLLRDNORM;
    ppfd1.revents = 0;
    fdList.push_back(ppfd1);

    struct pollfd ppfd2;
    ppfd2.fd = fileno(fd);
    ppfd2.events = POLLIN | POLLHUP | POLLRDNORM;
    ppfd2.revents = 0;
    fdList.push_back(ppfd2);

    int sfd = socket(AF_INET,SOCK_STREAM,0);
    int b = bind(sfd,(sockaddr *)&addr,sizeof(addr));

    if(b < 0)
        perror("Bind error:");

    listen(sfd,10);
    int flag = fcntl(sfd,F_GETFL,0);
    fcntl(sfd,F_SETFL,flag|O_NONBLOCK);
    struct sockaddr_in c_addr;
    socklen_t cilen = sizeof(c_addr);

    while(1){
        for(int i = 0;i<fdList.size();i++){
            int r = poll(&fdList[i],1,100);
            if(r > 0){
                if(fdList[i].revents & POLLIN){
                    char buff[20];
                    read(fdList[i].fd,buff,20);
                    broadcast(buff,20);
                }
            }
        }

        int nsfd = accept(sfd,(struct sockaddr *)&c_addr,&cilen);
        if(nsfd > 0){
            cout<<"Client Connected..\n";
            cout<<"NSFD: "<<nsfd<<endl;
            cList.push_back(nsfd);
        }

        sleep(2);
    }

    return 0;
}
