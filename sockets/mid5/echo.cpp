#include<iostream>
#include<unistd.h>
using namespace std;

int main(){

   char buff[] = "Echo server";
   int x = 0;
   while(1){
    write(0,buff,20);
    x++;
    sleep(1);
    if(x == 4)
        exit(0);
   }
}
