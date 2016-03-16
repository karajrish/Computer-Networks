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

struct Message{
    int op1;
    int op2;
    int opr;
    int rslt;
};


int main(){
    struct Message msg;
    while(read(0,&msg,sizeof(struct Message)) != 0){
        msg.rslt = msg.op1 + msg.op2;
        int s = sizeof(msg);
        cout<<"Bytes Sent: "<<write(0,&msg,s)<<endl;
    }
    return 0;
}
