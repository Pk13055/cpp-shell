#include <bits/stdc++.h>

#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>


#include "classdef.h"
#include "colormod.h"
using namespace std;
using namespace Color;

signed main() {
	
	BaseDetails b;
	char *cmd = (char*) malloc(COMMAND_LENGTH * sizeof(char));
	char shell_prompt[10];


	// main process execution loop
	do {
		
		
		b.update(); // updates to the pwd
		b.print_term(); // prints the PS1
		check_bg(); // checks for background ends

		// cmd = getInput();
        cmd = readline(" ");
        
		cmd = remove_padding(cmd); // removing the leading and trailing spaces
		
		/* EXECUTION OF PROCESS */
		exe_cmds(cmd);
		
    	add_history(cmd);
        
        free(cmd);

	} while(true);
}
