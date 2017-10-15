#include <bits/stdc++.h>
#include <readline/history.h>
#include <sys/wait.h>

#include "classdef.h"
using namespace std;

void sig_handler(int signal_number){
	if(all_proc.size())
	{	
	map<int, Process>::iterator it = all_proc.end(); 
	((*it).second).set_status();
	printf("%s\n",((*it).second).get_status());
	if(strcmp(((*it).second).get_status(),"Running") == 0)
		{printf("Killed process:%s\n",((*it).second).get_name());
		all_proc.erase((*it).first);
		}
	}
}


void sig_handler_z(int signal_number){

	for(auto x: all_proc){
		kill(x.first,SIGTSTP);
	}

	printf("\n");
}

void sig_handler_child(int signal_number){

	int pid;
	int status;
	
	while((pid = waitpid(-1,&status,WNOHANG)) > 0)
	{
		all_proc[pid].set_status();
		if(strcmp(all_proc[pid].get_status(),"DEAD") == 0)		
			{cout<<endl<<"+ ["<<all_proc[pid].get_priority() <<"] "<<all_proc[pid].get_name()<<" ended"<<endl;		
			all_proc.erase(pid);
			}
	}	
}



signed main(int argc, char** argv) {
	
	BaseDetails b;
	char *cmd = (char*) malloc(COMMAND_LENGTH * sizeof(char));
	char shell_prompt[10];

	read_history("/home/shubh/Assignment/OS/Assignment2/cpp-shell/.shkell_history");

	signal(SIGINT, sig_handler_c);
	
	signal(SIGTSTP,sig_handler_z);
	signal(SIGCHLD,sig_handler_child);

	// main process execution loop
	do {
		
		
		b.update(); // updates to the pwd
		// check_bg(); // checks for background ends

        cmd = b.print_term();
		cmd = remove_padding(cmd); // removing the leading and trailing spaces
		
		/* EXECUTION OF PROCESS */
		exe_cmds(cmd);

		if(strlen(cmd) > 0)
    		add_history(cmd);
        free(cmd);

	} while(true);
}
