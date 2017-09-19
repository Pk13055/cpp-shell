// handles file redirection
#include <bits/stdc++.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <vector>
#include <utility> 

#include "colormod.h"
#include "classdef.h"

using namespace std;
using namespace Color;

#define FORWARD_SINGLE 62
#define FORWARD_APPEND 124
#define BACKWARD_SINGLE 60
#define BACKWARD_APPEND 120

/*
	Feture Set

	- Does not support & operator for multiple redirection
	- Does not support redirection on the small end of the arrow (add this part)

*/


// get the type of pipe, using simple hashing 
int get_type(char *s) {
	int c = 0;
	for(int i = 0; s[i] != '\0'; c += s[i++]);
		return c;
}


// handles the separation of the tokens and values
pair< vector< vector<char*> >, vector< pair<int, int> > >
	get_separated(vector<char*> tokens) {
	
	vector< vector<char*> > separated;
	vector< pair<int, int> > codes;
	vector<char*> cur_arg;

	// seperates it into tokens taking care of error codes
	for(auto i: tokens) {
		if (strstr((i), "<") || strstr((i), ">")  || strstr((i), "<<")  || strstr((i), ">>")) {
			if(strcmp((i), "<") == 0 || strcmp((i), ">") == 0  
				|| strcmp((i), "<<") == 0  || strcmp((i), ">>") == 0)
				// change part to include both side numbering in later release
				codes.push_back(make_pair(1, get_type(i)));
			else codes.push_back(make_pair(i[0] - 48, get_type(i + 1)));
			if(cur_arg.size())
				separated.push_back(cur_arg);
			cur_arg.clear();
		}
		else cur_arg.push_back(i);
	}

	// pushing the last command
	separated.push_back(cur_arg);

	return make_pair(separated, codes);
} 


// checks for proper redirection
bool check_redirection(
	vector< vector<char*> > separated, 
	vector< pair<int, int> > codes) {
	if(separated.size() != codes.size() + 1) return true;
	for(auto i: codes)
		if((i.first != 0 && i.first != 1 && i.first != 2)
			|| (i.second != FORWARD_SINGLE && i.second != FORWARD_APPEND
				&& i.second != BACKWARD_SINGLE && i.second != BACKWARD_APPEND)) 
			return true;
		return false;
}


// convert char* vector to string
char* get_command(vector<char*> separate) {
	char command[COMMAND_LENGTH];
	bool is_first = true;
	// for(vector<char*>::iterator i = separate.begin(); i != separate.end(); i++) {
	for(auto i: separate) {
			if(is_first)
				strcpy(command, i), is_first = false;
			else
				strcat(command, " "), strcat(command, i);
		}
	strcat(command, "\0");
	cout<<command<<endl;
	return command;
}

char** prepare_one(char cmd[]) {
	vector<char*> tokenized;
	char *temp = strtok(cmd, " ");
	tokenized.push_back(temp);
	while(tokenized.back()) {
		temp = strtok(NULL, " ");
		if(!temp) break;
		tokenized.push_back(temp);
	}
	if(!tokenized.back()) tokenized.pop_back();

	char** args = (char**) malloc((tokenized.size()+1) * sizeof(char*));
	int ic = 0;
	for(auto i: tokenized) args[ic++] = i;
		args[tokenized.size()] = NULL;
	return args;
}

// the main function that handles the redirection
int handle_redirect(vector<char*> tokens) {
	Modifier red(FG_RED);
	Modifier def(FG_DEFAULT);

	auto i = get_separated(tokens);
	vector< vector<char*> > separated = i.first;
	// codes[i].first === stdin/out/err codes[i].second === direction
	vector< pair<int, int> > codes = i.second;

	if(check_redirection(separated, codes)) {
		cout<<red<<"Invalid redirection"<<def<<endl;
		return false;
	}

	// execute and redirect here here //

	// handle only single redirection for now
	if(codes.size() == 1) {
		vector<char*> cmd1 = separated[0], cmd2 = separated[1];
		// flags for the opening of the outputfile
		int status,
		fg = ((codes[0].second == FORWARD_SINGLE || codes[0].second == BACKWARD_SINGLE)? 0 : O_APPEND) |
		O_WRONLY | O_TRUNC | O_CREAT;
		char *file_, *cmd;
		// remove input redirection for now
		if (codes[0].first == 0) codes[0].first = 1;
		cout<<codes[0].first<<endl;
		if(codes[0].second == FORWARD_SINGLE || codes[0].second == FORWARD_APPEND) 
			file_ = get_command(cmd2), cmd = get_command(cmd1);
		else file_ = get_command(cmd1), cmd = get_command(cmd2);
		
		// open the file as output storer
		int fd = open(file_, fg, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
		dup2(fd, codes[0].first); // replace the stdout/er with fd
		close(fd); // close unused fd
		status = single_command(cmd);
		// status = one_statement(prepare_one(cmd), false);
	}
	// add multiple redirection and types later
	else {
		cout<<red<<"Multiple redirection feature coming up in next release"<<def<<endl;
		/*
		// first tells the stdinouterr, and second the direction
		auto c = codes.begin();

		for(vector< vector<char*> >::iterator i = separated.begin(), j = separated.begin() + 1
			; i != separated.end() && j != separated.end(); i++, j++) {
				vector<char*> cmd1, cmd2;
			// in this situation execute the first and send to second
			if(c.second == FORWARD_APPEND || c.second == FORWARD_SINGLE) cmd1 = *i, cmd2 = *j;
			// in this case execute the second and send to first
			else cmd1 = *j, cmd2 = *i;
		}*/
	}
	return true;
}	