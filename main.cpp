#include <bits/stdc++.h>
#include <sys/wait.h>
#include "classdef.h"
#include <unistd.h>
#include <stdlib.h>

signed main() {
	BaseDetails b;
	char cmd[COMMAND_LENGTH];
	while(1) {
		
		b.update();
		
		// this creates a parent child fork and the new command is run on the child process
		pid_t pid;
		int status;


		// pid is 0 for the child process
		if((pid = fork()) == 0) {
			cout<<b.get_user()<<"@"<<b.get_host()<<" "<<b.get_cwd()<<"$ ";
			cin>>cmd;
			if(execlp(cmd, cmd, (char*) NULL) < 0) {
				cout<<"Error Occured";
				exit(1);
			}
		}
		// targets non-zero, ie, parent process
		else 
			while(wait(&status) != pid);
			// parent process should wait until the child process is done
	}
}
