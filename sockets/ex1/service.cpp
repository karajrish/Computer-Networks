#include<iostream>
#include<pthread.h>
#include<vector>
#include <fcntl.h>
#include<poll.h>
#include<unistd.h>
#include<sys/socket.h>
#include<string.h>
#include <sys/types.h>
using namespace std;

vector<struct pollfd> fdList;

static int read_fd(int socket) /* Socket from which the file descriptor is read */
{
 int sent_fd;
 struct msghdr message;
 struct iovec iov[1];
 struct cmsghdr *control_message = NULL;
 char ctrl_buf[CMSG_SPACE(sizeof(int))];
 char data[1];
 int res;

 memset(&message, 0, sizeof(struct msghdr));
 memset(ctrl_buf, 0, CMSG_SPACE(sizeof(int)));

 /* For the dummy data */
 iov[0].iov_base = data;
 iov[0].iov_len = sizeof(data);

 message.msg_name = NULL;
 message.msg_namelen = 0;
 message.msg_control = ctrl_buf;
 message.msg_controllen = CMSG_SPACE(sizeof(int));
 message.msg_iov = iov;
 message.msg_iovlen = 1;

 if((res = recvmsg(socket, &message, 0)) <= 0)
  return res;

 /* Iterate through header to find if there is a file descriptor */
 for(control_message = CMSG_FIRSTHDR(&message);
     control_message != NULL;
     control_message = CMSG_NXTHDR(&message,
                                   control_message))
 {
  if( (control_message->cmsg_level == SOL_SOCKET) &&
      (control_message->cmsg_type == SCM_RIGHTS) )
  {
   return *((int *) CMSG_DATA(control_message));
  }
 }

 return -1;
}


void * poll_func(void * arg){
    char buff[20];
    while(1){
        if(fdList.size() == 0){
            sleep(2);
        }
        else{
            for(int i = 0;i<fdList.size();i++){
                int r = poll(&fdList[i],1,100);
                if(r > 0){
                    int x;
                    if((x=read(fdList[i].fd,buff,20)) == 0){
                        cout<<"One client removed..\n";
                        fdList.erase(fdList.begin() + i);
                    }
                    else if(x > 0){
                        cout<<"Message Received: "<<buff<<endl;
                        if(write(fdList[i].fd,buff,strlen(buff)) == 0)
                            perror("Write error: ");
                    }
                    else{
                        perror("reading: ");
                        cout<<fdList[i].fd<<endl;
                    }
                }
            }
            sleep(2);
        }
    }
}

int main(){
        pthread_t poll_thread;
        pthread_create(&poll_thread,NULL,poll_func,NULL);
        int fd;
        while((fd = read_fd(0)) != -1){
            struct pollfd pfd;
            pfd.fd = fd;
            pfd.events = POLLIN | POLLHUP | POLLRDNORM;
            pfd.revents = 0;
            fdList.push_back(pfd);
            cout<<"fd: "<<fd<<endl;

        }
        cout<<"Service Dead..\n";
}
