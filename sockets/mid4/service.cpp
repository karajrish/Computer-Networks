#include<iostream>
#include<pthread.h>
#include<vector>
#include <fcntl.h>
#include<poll.h>
#include<unistd.h>
#include<sys/socket.h>
#include<string.h>
#include <sys/types.h>
#include<signal.h>
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



int main(){

        int fd;
        while((fd = read_fd(0)) != -1){
            char buff[20];
            while(read(fd,buff,20)!=0){
                cout<<"SERVICE-->Message received: "<<buff<<endl;
            }
            kill(getppid(),SIGUSR1);
        }
        cout<<"Service Dead..\n";
}
