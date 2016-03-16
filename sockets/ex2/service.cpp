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


int main(){
    char buff[20];
    cout<<"From child, pid: "<<getpid()<<endl;
    while(read(0,buff,20) != 0){
        write(0,buff,20);
    }
}
