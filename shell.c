#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include<sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <pwd.h> 
#include <string.h>
char* argv[256];//to recieve the variable(-a , -alh ...) command

void type_prompt() {
	char hostname[256] = { '\0' };
	gethostname(hostname, sizeof(hostname));

	char curpath[256] = { '\0' };
	getcwd(curpath, 256);

	char prompt = '$';
	if (geteuid() == 0) {
		prompt = '#';
	}

	printf("%s@%s:%s%c", getpwuid(getuid())->pw_name, hostname, curpath, prompt);
}

void read_command(char *str,char** cmd) {
	int index=0;
	char *retPtr;//to spilit cmd
	retPtr=strtok(str," \n");
	while(retPtr!=NULL){
		argv[index++]=retPtr;
		if(index==1){
			*cmd=retPtr;
		}
		retPtr=strtok(NULL," \n");
	}
	argv[index]=NULL;
}

int count(char cmd[1024] , char**cc){
	char *s;
	s=strtok(cmd,"|\n");
	int num = 0;
	while(s!=NULL){
		cc[num++]=s;
		s=strtok(NULL," |\n");
	}
	cc[num]=NULL;
	return num;
}


int main(){
	pid_t pid;
	pid_t pid2;
	int cmd_len;
	while (1){
		char cmd[1024];
		char *cc[1024];
		char *exeName[1024];
		type_prompt();//to print the information of user
		fgets(cmd,1024,stdin);//to recieve the command
		cmd_len=count(cmd,&cc);// see how many pipe we have

		//if the cmd don't have pipe
		if(cmd_len==1){
			read_command(cmd,&exeName);//to spilit the command
			pid=fork();//call child process 
			if (pid == 0) {
				execvp(*exeName,argv);//if process is child then change the child prrocess to run command
			}
			else if(pid>0){
				waitpid(-1, NULL, 0);//parent process have to wait child process end
			}
			else{
				perror("fork");//the fork is wrong
			}
		}

		//if cmd have pipe
		else if(cmd_len>1){
			read_command(cmd,&exeName);//to spilit the command
			int pipefd[2];
			pipe(pipefd);
			pid=fork();//call child1 process 
			if (pid == 0) {
				close(1);//close main's input
				dup(pipefd[1]);//connect main's input to child1's input
				close(pipefd[0]);//close child1's output to pipe
				close(pipefd[1]);//close child1's input to pipe
				execlp(cc[0],cc[0],NULL);
			}
			if(pid2==0){
				close(0);//close main's output
				dup(pipefd[0]);//connect main's output to child2's output
				close(pipefd[0]);//close child2's output to pipe
				close(pipefd[1]);//close child2's input to pipe
				execlp(cc[1],cc[1],NULL);
			}
			else if(pid>0){
				pid2=fork();//call child2 process
			}
			if(pid2>0){
				close(pipefd[0]);// close main's input to pipe
				close(pipefd[1]);// close main's output to pipe
				waitpid(-1,NULL,0);
				waitpid(-1,NULL,0);
			}
			
			else{
				perror("fork");//the fork is wrong
			}
		}
		else{
			perror("input");
		}
	}
	return 0;
}