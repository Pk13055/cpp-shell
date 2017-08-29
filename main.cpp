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
		
		cmd = getInput();
		cmd = remove_padding(cmd); // removing the leading and trailing spaces
		
		if(strstr(cmd,"exit") || strstr(cmd,"quit")) { exit(0); break; } // exit shell on quit
		else if(strcmp(cmd, "pinfo") == 0) {
			cout<<"PID\t"<<"Name"<<endl;
			for(auto i: all_proc)
				cout<<i.first<<"\t"<<i.second.get_name()<<endl;
			continue;
		}
		
		/* EXECUTION OF PROCESS */
		exe_cmds(cmd);

	} while(true);
}
