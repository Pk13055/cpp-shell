#include <bits/stdc++.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <pwd.h>

#include "classdef.h"
#include "colormod.h"
using namespace Color;
using namespace std;

char* int_to_string(int pid){

	char* buf = (char*)malloc(PID_LENGTH);
	sprintf(buf,"%d",pid);
	return buf;
}


char* getInput() {
	int position = 0, len = COMMAND_LENGTH;
	char *buffer = (char*) malloc(sizeof(char) * len);
	int c;

	if (!buffer) {
		fprintf(stderr, "shkell: allocation error\n");
		exit(EXIT_FAILURE);
	}

	while (1) {
	// Read a character 
		c = getchar();

	// If we hit EOF, replace it with a null character and return.
		if (c == EOF || c == '\n') {
			buffer[position] = '\0';
			return buffer;
		} 
		else 
			buffer[position] = c;
		position++;

	// If we have exceeded the buffer, reallocate.
		if (position >= len) {
			len += COMMAND_LENGTH;
			realloc(buffer, len);
			if (!buffer) {
				fprintf(stderr, "shkell: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}
	}
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


int one_statement(char* cmd[], bool is_bg) {
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
		if (execvp(cmd[0], cmd) == -1) perror("shkell");
		else {
			all_proc[pid] = p;
			if(!is_bg) _exit(status);
		}
	} 
	else if(!is_bg) {
		do 
			wpid = waitpid(pid, &status, WUNTRACED);
		while (!WIFEXITED(status) && !WIFSIGNALED(status));
		all_proc.erase(_pid);
	}
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

	char** args = (char**) malloc((tokenized.size()+1) * sizeof(char*));
	int ic = 0;
	for(auto i: tokenized) args[ic++] = i;
		args[tokenized.size()] = NULL;

	/* handling for the single type of command*/

		// cd command block
		if(strcmp(args[0], "cd") == 0) {

			if(tokenized.size() == 1)
				chdir(home_dir); // chdir(getpwuid(getuid())->pw_dir);
	
			// cd to the other dir
			else if((ic = chdir(args[1])) < 0) {
				cout<<red<<" Error 'cd'-ing into dir "<<args[1]<<def<<endl;
				perror("shkell");
			}
		}

		// case nightswatch
		if(strcmp(args[0], "nightswatch") == 0) {
			strcpy(args[0], "watch");
			one_statement(args, false);
		}

		// case ls
		else if(strcmp(args[0],"ls")==0) ls(tokenized);

		// case echo
		else if(!strcmp(args[0], "echo")) echo(tokenized);

		// pinfo custom command
		
		else if(!strcmp(args[0],"interrupt")){
			FILE* memfile = fopen("/proc/interrupts","r");
			char line[STAT_LENGTH];

			if(memfile == NULL)
				cout<<"Couldn't open /proc/interrupts"<<endl;	
			else
				for(int i=0;fgets(line, sizeof(line),memfile);i++)
					if(i == 0 || i ==2)	{printf("%s",line);}
		}


		else if(strcmp(args[0], "pinfo") == 0) {

			char stat_file[FILE_NAME],exe_file[FILE_NAME];
			char line[STAT_LENGTH],execute_path[PWD_LENGTH];

			strcpy(stat_file,"/proc/");
			if (tokenized.size() == 1)
				strcat(stat_file,int_to_string(getpid()));
			else if (tokenized.size() ==2)
				strcat(stat_file,args[1]);
			
			strcat(exe_file,stat_file);
			strcat(stat_file,"/stat");

			FILE* inputFile = fopen(stat_file,"r");
			if(inputFile == NULL)
				cout<<"Process doesnt exist"<<endl;	
			else
			{
				
				fgets(line, sizeof(line),inputFile);

				char* stat_param[100];
				stat_param[0] = strtok(line," ");
				for(int i=0;stat_param[i] != NULL;i++)
					stat_param[i+1] = strtok(NULL," ");

				cout<<"PID:"<<stat_param[0]<<endl;
				cout<<"Name:"<<stat_param[1]<<endl;
				cout<<"State:"<<stat_param[2]<<endl;
				cout<<"Virtual Memory:"<<stat_param[22]<<endl;
			
				strcat(exe_file,"/exe");
				if(readlink(exe_file, execute_path, PWD_LENGTH)< 0)
					cout<<"Unable to locate the executable path"<<endl;
				else
					cout<<"Executable Path:"<<execute_path<<endl;	
				exe_file[0] = '\0';

			}
				
		}

		else if(strcmp(args[0],"dirty") == 0){

			FILE* memfile = fopen("/proc/meminfo","r");
			char line[STAT_LENGTH];

			if(memfile == NULL)
				cout<<"Couldn't open /proc/meminfo"<<endl;	
			else
				for(int i=0;fgets(line, sizeof(line),memfile);i++)
					if(i == 16)	{printf("%s",line); break;}
		}


		else if(strcmp(args[tokenized.size() - 1], "&") == 0) {
			cout<<"+ ["<<all_proc.size() + 1<<"] "<<args[0]<<endl;
			args[tokenized.size() - 1] = NULL;
			one_statement(args, true);
			return BACKGROUND;
		}
		
		// checks for piping and redirection type commands
		else {
			bool is_pipe = false, is_redirect = false;
			for(auto i: tokenized) {
				if(!is_pipe && strcmp(i, "|") == 0) is_pipe = true;
				else if(!is_redirect && (strstr(i, "<") || strstr(i, ">")  || strstr(i, "<<")  
					|| strstr(i, ">>") )) is_redirect = true;
					if(is_pipe || is_redirect) break;
			}

			// piped commands
			if(is_pipe) {
				cout<<"Piped command"<<endl;
			// tokenize further and handle here
			// add a function here
				return PIPED;
			}

			// redirect command
			else if(is_redirect) {
				handle_redirect(tokenized);
				return REDIRECT;
			}
		
			// normal command
			else one_statement(args, false);
		}

	return CHILD;
}

// converts multiple commands and runs them one by one
int exe_cmds(char cmd[]) {

	vector<char*> init_args;
	while(cmd) {
		char* temp = strsep(&cmd, ";");
		temp = remove_padding(temp);
		if(strlen(temp))
			init_args.push_back(temp);
	}

	bool return_type = CHILD;
	int ic = 0;
	pid_t pid, wpid;
	int status;
	for(auto cmd: init_args) {

		if(!strcmp(cmd,"exit") || !strcmp(cmd,"quit")) exit(0); // exit shell on quit
		return_type = single_command(cmd); 
	}
	
	return return_type; 
}