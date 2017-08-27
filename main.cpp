#include <bits/stdc++.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string>

#include "classdef.h"
#include "colormod.h"
using namespace std;
using namespace Color;

signed main() {
	
	BaseDetails b;
	char *cmd = (char*) malloc(COMMAND_LENGTH * sizeof(char));
	
	// main process execution loop
	do {
		
		
		b.update();
		b.print_term();
		
		cin.getline(cmd, COMMAND_LENGTH);
		cmd = remove_padding(cmd); // removing the leading and trailing spaces
		
		if((strcmp(cmd,"exit") == 0) || (strcmp(cmd,"quit") == 0)) { exit(0); break; } // exit shell on quit
		else if(strcmp(cmd, "pinfo") == 0) {
			cout<<"PID\t"<<"Name"<<endl;
			for(auto i: all_proc)
				cout<<i.first<<"\t"<<i.second.get_name()<<endl;
			continue;
		}

		
		/* EXECUTION OF PROCESS */

		pid_t pid;
		int status;
		Process p;

		// pid is 0 for the child process
		if((pid = fork()) == 0) {
			
			p.set_pid(pid);
			p.set_name(cmd);
			p.set_job(0);
			all_proc[pid] = p;

			exe_cmds(cmd);
			
		}

		// targets non-zero, ie, parent process
		// parent process should wait until the child process is done
		else {
			while(wait(&status) != pid);
			if(kill(pid, SIGTERM)) kill(pid, SIGKILL); // try to kill the process gracefully
			all_proc.erase(pid);
		}

	} while(true);
}
