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
#include<signal.h>
using namespace std;

static int send_fd(int socket,int fd_to_send) /* File descriptor to be passed, could be another socket */
{
 struct msghdr message;
 struct iovec iov[1];
 struct cmsghdr *control_message = NULL;
 char ctrl_buf[CMSG_SPACE(sizeof(int))];
 char data[1];

 memset(&message, 0, sizeof(struct msghdr));
 memset(ctrl_buf, 0, CMSG_SPACE(sizeof(int)));

 /* We are passing at least one byte of data so that recvmsg() will not return 0 */
 data[0] = ' ';
 iov[0].iov_base = data;
 iov[0].iov_len = sizeof(data);

 message.msg_name = NULL;
 message.msg_namelen = 0;
 message.msg_iov = iov;
 message.msg_iovlen = 1;
 message.msg_controllen =  CMSG_SPACE(sizeof(int));
 message.msg_control = ctrl_buf;

 control_message = CMSG_FIRSTHDR(&message);
 control_message->cmsg_level = SOL_SOCKET;
 control_message->cmsg_type = SCM_RIGHTS;
 control_message->cmsg_len = CMSG_LEN(sizeof(int));

 *((int *) CMSG_DATA(control_message)) = fd_to_send;

 return sendmsg(socket, &message, 0);
}

map<int,bool> state;
int sfd1,sfd2,sfd3;
int my_pipe1[2];
int my_pipe2[2];
int pid1,pid2,pid3;

void * port1_listener(void * arg){
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
            map<int,bool>::iterator it;
            int flag = 1;
            for(it = state.begin();it!=state.end();it++){
                if(it->second == true){
                    it->second = false;
                    if(it->first == pid1){
                        send_fd(my_pipe1[0],nsfd);
                        cout<<"Platform 1\n";
                        flag = 0;
                        break;
                    }
                    else if(it->first == pid2){
                        send_fd(my_pipe2[0],nsfd);
                        cout<<"Platform 2\n";
                        flag = 0;
                        break;
                    }
                }
            }

            if(flag == 1){
                cout<<"All platform occupied..\n";
            }

            close(nsfd);
        }
}




void handler(int signo,siginfo_t * info,void * vp){
    cout<<"Platform Free..\n";
    state.find(info->si_pid)->second = true;
}

int main(){
    struct sigaction action;
    action.sa_flags = SA_SIGINFO;
    action.sa_sigaction = handler;

     if(sigaction(SIGUSR1,&action,NULL) < 0)
        perror("Signal : ");

    sfd1 = socket(AF_INET,SOCK_STREAM,0);
    sfd2 = socket(AF_INET,SOCK_STREAM,0);
    sfd3 = socket(AF_INET,SOCK_STREAM,0);


    socketpair(AF_UNIX,SOCK_STREAM,0,my_pipe1);
    pid1 = fork();
    if(pid1 == 0){
        close(my_pipe1[0]);
        dup2(my_pipe1[1],0);
        execl("service","service",NULL,NULL);
    }
    else{
        close(my_pipe1[1]);
        socketpair(AF_UNIX,SOCK_STREAM,0,my_pipe2);
        pid2 = fork();
        if(pid2 == 0){
            close(my_pipe2[0]);
            dup2(my_pipe2[1],0);
            execl("service","service",NULL,NULL);
        }
        else{
            close(my_pipe2[1]);
            pthread_t port1_thread;
            pthread_create(&port1_thread,NULL,port1_listener,NULL);

            state.insert(pair<int,bool>(pid1,true));
            state.insert(pair<int,bool>(pid2,true));

            pthread_join(port1_thread,NULL);
        }
    }

}
