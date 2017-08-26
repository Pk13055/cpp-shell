#include <bits/stdc++.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <map>
#include "classdef.h"
#include "colormod.h"

using namespace std;
using namespace Color;

signed main() {
	BaseDetails b;
	char *cmd = (char*) malloc(COMMAND_LENGTH);
	Modifier red(FG_RED);
	Modifier def(FG_DEFAULT);
	
	map<pid_t, Process> all_proc;

	// main process execution loop
	do {
		
		// updates the static params
		b.update();
		
		b.print_term();
		
		// this creates a parent child fork and the new command is run on the child process
		Process p;

		cin>>cmd;
		// removing the leading and trailing spaces
		/* ADD REMOVE CODE HERE*/

		if(strlen(cmd) == 1)
			continue;


		// exit shell on quit
		if(!strcmp(cmd,"exit") || !strcmp(cmd,"quit"))
			break;

		pid_t pid;
		int status;

		// pid is 0 for the child process
		if((pid = fork()) == 0) {
			int ic;
			
			p.set_pid(pid);
			p.set_name(cmd);
			p.set_job(1);
			all_proc[pid] = p;

			if((ic = execlp(cmd, cmd, (char*) NULL)) < 0) {
				cout<<red<<"Error Occured "<<ic<<def<<endl;
				exit(1);
			}
		}

		// targets non-zero, ie, parent process
		// parent process should wait until the child process is done
		else {
			while(wait(&status) != pid);
			all_proc.erase(pid);
		}

	
	} while(true);
}
