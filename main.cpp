#include <bits/stdc++.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <map>
#include "classdef.h"
#include "colormod.h"

using namespace std;
using namespace Color;

void single_command(char cmd[])
{
	Modifier red(FG_RED);
	Modifier def(FG_DEFAULT);

	char* arg[100];

	arg[0] = strtok(cmd," ");

	for(int i=0;arg[i] != NULL;i++)
		arg[i+1] = strtok(NULL," ");

	int ic;
	if((ic = execvp(arg[0],arg)) < 0) {
		cout<<red<<"Error Occured "<<ic<<def<<endl;
		exit(1);}

}

void execute_command(char cmd[]){

	char* arg[100];

	arg[0] = strtok(cmd,";");

	for(int i=0; arg[i]!= NULL;i++)
		arg[i+1] = strtok(NULL,";");	

	for(int i=0;arg[i]!=NULL;i++)
		{
			int pid = fork();
			if(pid == 0)
				single_command(arg[i]);
			else
				wait(NULL);
		}

}

signed main() {
	BaseDetails b;
	char *cmd = (char*) malloc(COMMAND_LENGTH);
	
	// main process execution loop
	do {
		
		// updates the static params
		b.update();
		
		b.print_term();
		
		// this creates a parent child fork and the new command is run on the child process
		Process p;
		// cin.getline(cmd, COMMAND_LENGTH);
		fgets(cmd,COMMAND_LENGTH,stdin);
		cmd = strtok(cmd,"\n");
		// removing the leading and trailing spaces
		strcpy(cmd, remove_padding(cmd));
		// exit shell on quit
		if(!strcmp(cmd,"exit") || !strcmp(cmd,"quit"))
			break;

		else if(!strcmp(cmd, "pwd")) {
			cout<<b.get_cwd()<<endl;
			continue;
		}

		pid_t pid;
		int status;

		// pid is 0 for the child process
		if((pid = fork()) == 0) {
			
			p.set_pid(pid);
			p.set_name(cmd);
			p.set_job(1);
			all_proc[pid] = p;

			execute_command(cmd);
			
		}

		// targets non-zero, ie, parent process
		// parent process should wait until the child process is done
		else {
			while(wait(&status) != pid);
			all_proc.erase(pid);
		}

	
	} while(true);
}
