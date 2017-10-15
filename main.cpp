#include <bits/stdc++.h>
#include <readline/history.h>
#include <sys/wait.h>

#include "classdef.h"
using namespace std;


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
