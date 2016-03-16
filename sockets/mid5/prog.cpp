#include<iostream>
#include<unistd.h>
using namespace std;

int main(){

    int x = 0;
    while(x != 5){
        x++;
        cout<<"\nMessage from service\n";
    }

}
