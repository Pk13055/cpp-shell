#include <bits/stdc++.h>
#include <sys/wait.h>
#include <unistd.h>
#include "classdef.h"

void sig_handler_c(int){
	for(auto x: all_proc)
		{if((x.second).get_type() ==1)
			kill(x.first,SIGINT);
			for (auto y : all_proc)
			if((y.second).get_priority() >= (x.second).get_priority())
				(y.second).set_priority((y.second).get_priority()-1);
		}

	printf("\n");
}

void sig_handler_z(int signal_number){
	for(auto x: all_proc)
		if((x.second).get_type() ==1)
			kill(x.first,SIGTSTP);

	printf("\n");

}
void sig_handler_child(int signal_number){

	int pid;
	int status;

	while((pid =waitpid(-1,&status,(WNOHANG | WUNTRACED) ))>0){

		Process proc = all_proc[pid];

		for (auto x : all_proc)
			if((x.second).get_priority() >= proc.get_priority())
				(x.second).set_priority((x.second).get_priority()-1);

		all_proc.erase(pid);

		if(WIFSTOPPED(status)) {
				printf("%d stopped\n",pid);
				all_proc[pid] = proc;
			}
	
		else if(WIFSIGNALED(status)){
				printf("%d terminated\n", pid);
		}
	

	}
}


