#include <iostream>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
using namespace std;
int main(){
    int status;
    int count;
    pid_t PID = fork();
        if(PID==-1) {
            perror("fork()");
            exit(-1);
        }

        else if(PID==0){//this is child process
            cin>>count;
            if(count==100){
                cout<<"you are right"<<endl;
            }
            else{
                cout<<"you are wrong"<<endl;
            }
            cout<<"I'm Child process"<<getpid()<<endl;
            cout<<"[child] enter my status : ";
            scanf("%d",&status);
            //printf("Child's PID is \n",getpid());
        }
        else{ //this is parent process
            cout<<"what value is 10*10 : "<<endl;
            waitpid(-1,&status,0);//wait child process end
            cout<<"I'm Parent process"<<getpid()<<endl;
            cout<<"[Parent]child exit status is "<<WEXITSTATUS(status)<<endl;
            
        }
    return 0;
}
