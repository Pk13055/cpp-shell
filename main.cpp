#include <bits/stdc++.h>
#include <readline/history.h>

#include "classdef.h"
using namespace std;

signed main(int argc, char** argv) {
	
	BaseDetails b;
	char *cmd = (char*) malloc(COMMAND_LENGTH * sizeof(char));
	char shell_prompt[10];

	read_history("/home/shubh/Assignment/OS/Assignment2/cpp-shell/.shkell_history");


	// main process execution loop
	do {
		
		
		b.update(); // updates to the pwd
		// check_bg(); // checks for background ends

        cmd = b.print_term();
		cmd = remove_padding(cmd); // removing the leading and trailing spaces
		
		/* EXECUTION OF PROCESS */
		exe_cmds(cmd);

    	add_history(cmd);
        free(cmd);

	} while(true);
}
