#include <bits/stdc++.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

#include "classdef.h"
#include "colormod.h"

using namespace std;
using namespace Color;

signed main() {
	BaseDetails b;
	char cmd[COMMAND_LENGTH];
	Modifier red(FG_RED);
	Modifier def(FG_DEFAULT);
	
	// main process execution loop
	do {
		
		// updates the static params
		b.update();
		
		b.print_term();
		
		// this creates a parent child fork and the new command is run on the child process
		pid_t pid;
		int status;

		cin>>cmd;
		
		// pid is 0 for the child process
		if((pid = fork()) == 0) {
			int ic;
			if((ic = execlp(cmd, cmd, (char*) NULL)) < 0) {
				cout<<red<<"Error Occured "<<ic<<def<<endl;
				exit(1);
			}
		}

		// targets non-zero, ie, parent process
		// parent process should wait until the child process is done
		else while(wait(&status) != pid);
	
	} while(true);
}
