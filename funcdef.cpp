#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <string>
#include <ctype.h>

#include "classdef.h"
#include "colormod.h"
using namespace Color;
using namespace std;

// stores the details of all the processes
map<pid_t, Process> all_proc;

// stores the pwd aswell as host and username
BaseDetails::BaseDetails() {

	register struct passwd *pw;
	register uid_t uid;
	uid = geteuid();
	pw = getpwuid(uid);
	if (pw)  strcpy(user_name, pw->pw_name);

	getcwd(pwd1, PWD_LENGTH);
	gethostname(host_name, SYSTEM_NAME_LENGTH);
}

// updates the pwd everytime
void BaseDetails::update() { 
	getcwd(pwd1, PWD_LENGTH);
	sub_home();
}

// replaces the home path with '~'
void  BaseDetails::sub_home() {
	const char *homedir;
	if ((homedir = getenv("HOME")) == NULL) 
		homedir = getpwuid(getuid())->pw_dir;
	bool is_same = (strlen(pwd1) >= strlen(homedir)) && 
		(!strncmp(pwd1, homedir, strlen(homedir)));
	
	if(is_same && strlen(pwd1) > strlen(homedir)) {
    	// - 1 because of ~ repr
		int repl = strlen(homedir), act = strlen(pwd1);
		for(int i = 1; i < act - repl; i++) 
			pwd1[i] = pwd1[repl + i - 1];
		pwd1[0] = '~';
		for(int i = act - repl; i < act; pwd1[i++] = '\0');
	}
	else if(is_same && strlen(pwd1) == strlen(homedir))
		strcpy(pwd1, "~");

}
char* BaseDetails::get_user() { return user_name; }
char* BaseDetails::get_host() { return host_name; }
char* BaseDetails::get_cwd()  { return pwd1; 	  }

void BaseDetails::print_term() {
	Modifier bg_green(BG_GREEN);
	Modifier bg_blue(BG_BLUE);
	Modifier fg_green(FG_GREEN);
	Modifier fg_blue(FG_BLUE);

	Modifier fg_def(FG_DEFAULT);
	Modifier bg_def(BG_DEFAULT);
	Modifier fg_black(FG_BLACK);
	cout<<bg_blue<<fg_black<<get_user()<<"@"<<get_host()<<" "<<
	bg_green<<fg_blue<<"▶ "<<fg_black<<get_cwd()<<" "<<
	bg_def<<fg_green<<"▶ "<<bg_def<<fg_def;
}



char* remove_padding(char cmd[]) {
	vector<char> sequence;
	bool is_init = true, space_taken = false;
	
	for(int i = 0; i < strlen(cmd); i++) {
		if(is_init && (cmd[i] == ' ' || cmd[i] == '\t'))
			continue;
		else if(is_init && !(cmd[i] == ' ' || cmd[i] == '\t')) {
			is_init = false;
			sequence.push_back(cmd[i]);
		}
		else if(!is_init && !space_taken && (cmd[i] == ' ' || cmd[i] == '\t')) {
			space_taken = true;
			sequence.push_back(' ');
		}
		else if(!is_init && space_taken && !(cmd[i] == ' ' || cmd[i] == '\t')) {
			sequence.push_back(cmd[i]);
			space_taken = false;
		}
		else if(!is_init && !space_taken && !(cmd[i] == ' ' || cmd[i] == '\t')) {
			sequence.push_back(cmd[i]);
		}
	}
	
	char *new_cmd = (char*) malloc(sizeof(char) * sequence.size());
	int j = 0;
	if(sequence.back() == ' ') sequence.pop_back();
	for(vector<char>::iterator i = sequence.begin(); i != sequence.end(); ++i) new_cmd[j++] = *i;

	return new_cmd;
}