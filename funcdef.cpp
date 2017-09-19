#include <bits/stdc++.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <pwd.h>

#include "classdef.h"
#include "colormod.h"
using namespace Color;
using namespace std;

char* getInput() {
	char *buffer = NULL;
	size_t buff_size = COMMAND_LENGTH;
	getline(&buffer, &buff_size, stdin);
	return buffer;
}

// stores the details of all the processes
map<pid_t, Process> all_proc;
// stores the home dir
char home_dir[PWD_LENGTH / 2];

// checks for background and prints to terminal
void check_bg() {
	if(all_proc.size()) {
		int status;
		for(map<pid_t, Process>::iterator i = 
			all_proc.begin(); i != all_proc.end(); i++) {
				pid_t wpid = waitpid(((*i).first), &status, WUNTRACED);
				if (WIFEXITED(status) || WIFSIGNALED(status)) {
					cout<<"- ["<<((*i).second).get_parent()<<"] ended"<<endl;
					all_proc.erase((*i).first);
				}
		}
	}
}

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
	bg_green<<fg_blue<<"\uE0B0 "<<fg_black<<get_cwd()<<" "<<
	bg_def<<fg_green<<"\uE0B0"<<bg_def<<fg_def;
}


char* remove_padding(char cmd[]) {

	if(cmd == NULL)
		return NULL;

	vector<char> sequence;
	bool is_init = true, space_taken = false;
	for(int i = 0; i < strlen(cmd); i++) {
		if(is_init && isspace(cmd[i]))
			continue;
		else if (cmd[i]!=' ' && isspace(cmd[i]))
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
	if(sequence.size()>0)
	if(sequence.back() == ' ') sequence.pop_back();
	for(auto i: sequence) new_cmd[j++] = i;
		new_cmd[sequence.size()] = '\0';
		return new_cmd;
}


int one_statement(vector<char*> tokenized,char* cmd[], bool is_bg) {
	pid_t pid, wpid;
	int status;
	Process p;

	pid = fork();
	int _pid = random(); 

	if (pid == 0) {
		p.set_pid(_pid);
		p.set_parent(getppid());
		p.set_job(CHILD);
		p.set_name(cmd[0]);

		/* handling for the single type of command*/

		// cd command block

		// case nightswatch
		if(strcmp(cmd[0], "nightswatch") == 0) {
			strcpy(cmd[0], "watch");
			strcpy(tokenized[0],"wacth"); 
			one_statement(tokenized,cmd, false);

		}

		// case ls
		else if(strcmp(cmd[0],"ls")==0) ls(tokenized);

		// case echo
		else if(!strcmp(cmd[0], "echo")) echo(tokenized);

		// pinfo custom command
		
		else if(!strcmp(cmd[0],"interrupt")){
			FILE* memfile = fopen("/proc/interrupts","r");
			char line[STAT_LENGTH];

			if(memfile == NULL)
				cout<<"Couldn't open /proc/interrupts"<<endl;	
			else
				for(int i=0;fgets(line, sizeof(line),memfile);i++)
					if(i == 0 || i ==2)	{printf("%s",line);}
		}


		else if(strcmp(cmd[0], "pinfo") == 0)
			pinfo(tokenized);
			

		else if(strcmp(cmd[0],"dirty") == 0){

			FILE* memfile = fopen("/proc/meminfo","r");
			char line[STAT_LENGTH];

			if(memfile == NULL)
				cout<<"Couldn't open /proc/meminfo"<<endl;	
			else
				for(int i=0;fgets(line, sizeof(line),memfile);i++)
					if(i == 16)	{printf("%s",line); break;}
		}

		else 
			if (execvp(cmd[0], cmd) == -1) perror("shkell");

			else {
				all_proc[pid] = p;
				if(!is_bg) _exit(status);
			}
	

		exit(0);	

	} 
	else if(!is_bg) {
		do 
			wpid = waitpid(pid, &status, WUNTRACED);
		while (!WIFEXITED(status) && !WIFSIGNALED(status));
		all_proc.erase(_pid);
	}


}

int single_command(char cmd[]) {

	// bool is_redirect = false;
	// for(int i =0; i< strlen(cmd);i++) {
	// 	if(!is_redirect && (strstr(cmd[i], "<") || strstr(cmd[i], ">")  || strstr(cmd[i], "<<")  
	// 		|| strstr(cmd[i], ">>") )) is_redirect = true
	// }
	
	// redirect command
	// if(is_redirect)
	// 	handle_redirect(pipe_commands);

	int is_pipe = 0;
	for(int i =0; i< strlen(cmd);i++) {
		if(cmd[i] == '|') is_pipe ++;
	}

	if(is_pipe && cmd) 
		{pipeline(cmd,is_pipe); return PIPED;}		
		
	// normal command

	vector<char*> tokenized;
	char *temp = strtok(cmd, " ");
	while(temp!=NULL) {

		temp = remove_padding(temp);

		if(temp[0]=='\"' && temp[strlen(temp)-1]!='\"')
		{
			char *temp2 = strtok(NULL,"\"");
		
			strcat(temp," ");

			if(temp2 != NULL)
				strcat(temp,temp2);

			if(temp[strlen(temp)-1]!=temp[0])
				{
				temp[strlen(temp)]=temp[0];
				temp[strlen(temp)+1] = '\0';
				}

		}


		if(strlen(temp)) 
			tokenized.push_back(temp);
		temp = strtok(NULL, " ");
	}


	char** args = (char**) malloc((tokenized.size()+1) * sizeof(char*));
	int ic = 0;
	for(auto i: tokenized) args[ic++] = i;
		args[tokenized.size()] = NULL;

	if(strcmp(args[tokenized.size() - 1], "&") == 0) {
		cout<<"+ ["<<all_proc.size() + 1<<"] "<<args[0]<<endl;
		args[tokenized.size() - 1] = NULL;
		tokenized.pop_back();
		one_statement(tokenized,args, true);
		return BACKGROUND;
	}
		
		// checks for piping and redirection type commands

	if(strcmp(args[0], "cd") == 0) {
		Modifier red(FG_RED);
		Modifier def(FG_DEFAULT);

		if(tokenized.size() == 1)
			chdir(home_dir); // chdir(getpwuid(getuid())->pw_dir);

		// cd to the other dir
		else if((ic = chdir(args[1])) < 0) {
			cout<<red<<" Error 'cd'-ing into dir "<<args[1]<<def<<endl;
			perror("shkell");
		
		}
	}

	else
		return one_statement(tokenized,args,false);		

	return CHILD;
}

// converts multiple commands and runs them one by one
int exe_cmds(char cmd[]) {

	vector<char*> init_args;
	char* temp =strtok(cmd, ";");
	while(temp!=NULL) {
		temp = remove_padding(temp);
		if(strlen(temp))
			init_args.push_back(temp);
		temp = strtok(NULL,";");
	}

	int return_type = CHILD;
	int ic = 0;
	pid_t pid, wpid;
	int status;
	for(auto cmd: init_args) {
		if(!strcmp(cmd,"exit") || !strcmp(cmd,"quit")) exit(0); // exit shell on quit

		return_type = single_command(cmd); 
	
	}
	
	return return_type; 
}