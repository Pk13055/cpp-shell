#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>
#include <string>
#include <ctype.h>

#include "classdef.h"
#include "colormod.h"
using namespace Color;
using namespace std;

// stores the details of all the processes
map<pid_t, Process> all_proc;
// stores the home dir
char home_dir[PWD_LENGTH / 2];

// stores the pwd aswell as host and username
BaseDetails::BaseDetails() {

	register struct passwd *pw;
	register uid_t uid;
	uid = geteuid();
	pw = getpwuid(uid);
	if (pw)  strcpy(user_name, pw->pw_name);

	// stores the home dir
	/*
	// uncomment this if you want the home_dir to be PC home_dir
	if ((home_dir = getenv("HOME")) == NULL) 
		home_dir = getpwuid(getuid())->pw_dir;
	*/
	// comment this if you want home dir to be PC home_dir
	getcwd(home_dir, PWD_LENGTH / 2);

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
	bool is_same = (strlen(pwd1) >= strlen(home_dir)) && 
		(!strncmp(pwd1, home_dir, strlen(home_dir)));
	
	if(is_same && strlen(pwd1) > strlen(home_dir)) {
    	// - 1 because of ~ repr
		int repl = strlen(home_dir), act = strlen(pwd1);
		for(int i = 1; i <= act - repl; i++) 
			pwd1[i] = pwd1[repl + i];
		pwd1[0] = '~';
		for(int i = act - repl; i < act; pwd1[i++] = '\0');
	}
	else if(is_same && strlen(pwd1) == strlen(home_dir))
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
		if(is_init && isspace(cmd[i]))
			continue;
		else if(is_init && !isspace(cmd[i])) {
			is_init = false;
			sequence.push_back(cmd[i]);
		}
		else if(!is_init && !space_taken && isspace(cmd[i])) {
			space_taken = true;
			sequence.push_back(' ');
		}
		else if(!is_init && space_taken && !isspace(cmd[i])) {
			sequence.push_back(cmd[i]);
			space_taken = false;
		}
		else if(!is_init && !space_taken && !isspace(cmd[i])) {
			sequence.push_back(cmd[i]);
		}
	}	
	
	char *new_cmd = (char*) malloc(sizeof(char) * sequence.size());
	int j = 0;
	if(sequence.back() == ' ') sequence.pop_back();
	for(auto i: sequence) new_cmd[j++] = i;

	return new_cmd;
}

int single_command(char cmd[]) {
	Modifier red(FG_RED);
	Modifier def(FG_DEFAULT);

	vector<char*> tokenized;
	char *temp = strtok(cmd, " ");
	tokenized.push_back(temp);
	while(tokenized.back()) {
		temp = strtok(NULL, " ");
		if(!temp) break;
		tokenized.push_back(temp);
	}
	if(!tokenized.back()) tokenized.pop_back();

	char** args = (char**) malloc(tokenized.size() * sizeof(char*));
	int ic = 0;
	for(auto i: tokenized) args[ic++] = i;
	
	/* handling for the sigle type of command*/
	// handling of pipe cd etc should be taken care of here

	if(strcmp(args[0], "cd") == 0) {
		// cd to the home directory set
		if(tokenized.size() == 1)
			// chdir(getpwuid(getuid())->pw_dir);
			chdir(home_dir);
		// cd to the other dir
		else if((ic = chdir(args[1])) < 0)
			cout<<red<<" Error 'cd'-ing into dir "<<args[1]<<def<<endl;
	}

	else if(strcmp(args[tokenized.size() - 1], "&") == 0) {
		cout<<"Background process";
		args[tokenized.size() - 1] = NULL;
		if((ic = execvp(args[0], args)) < 0) {
			cout<<red<<"ERROR "<<ic<<def<<endl;
			exit(1);
		}
		return BACKGROUND;
	}
	else {
		bool is_pipe = false, is_redirect = false;
		for(auto i: tokenized) {
			if(!is_pipe && strcmp(i, "|") == 0) is_pipe = true;
			else if(!is_redirect && (strcmp(i, "<") == 0 
				|| strcmp(i, ">") == 0)) is_redirect = true;
			if(is_pipe || is_redirect) break;
		}
		if(is_pipe) {
			cout<<"Piped command";
			// tokenize further and handle here
			// add a function here
			return PIPED;
		}
		else if(is_redirect) {
			cout<<"Redirection";
			// add a function here 	
			return REDIRECT;
		}
		// at the end check for normal command
		else if((ic = execvp(args[0], args)) < 0) {
			cout<<red<<"ERROR "<<ic<<def<<endl;
			exit(1);
		}
	}

	return CHILD;
}

int exe_cmds(char cmd[]) {

	vector<char*> init_args;
	char *temp = strtok(cmd, ";");
	init_args.push_back(remove_padding(temp));
	while(init_args.back()) {
		temp = strtok(NULL, ";");
		if(!temp) break;
		init_args.push_back(remove_padding(temp));
	}
	if(!init_args.back()) init_args.pop_back();
	bool return_type = CHILD;
	int ic = 0;
	pid_t pid;
	int status;
	for(auto i: init_args) {
		Process p;
		if((pid = fork()) == 0) {
			p.set_pid(pid);
			p.set_name(i);
			p.set_job(CHILD);
			all_proc[pid] = p;
			p.set_job(single_command(i));
		}
		else {
			if(p.get_type() != BACKGROUND) {
				while(waitpid(-1, &status, WIFEXITED(status)) != pid);
				// while(wait(&status) != pid); // wait for the process to finish
				if(kill(pid, SIGTERM)) kill(pid, 9); // try to kill the process gracefully
				all_proc.erase(pid);
			}
			else return_type = BACKGROUND;
		}
	}
	return return_type;
}