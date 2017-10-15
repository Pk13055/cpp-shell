#include <bits/stdc++.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <pwd.h>
#include <readline/readline.h>
#include <readline/history.h>

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
map<char*,char*> builtin_cmd;

// stores the home dir
char home_dir[PWD_LENGTH / 2];

// checks for background and prints to terminal
void check_bg() {
	if(all_proc.size()) {
		int status;
		for(map<pid_t, Process>::iterator i = 
			all_proc.begin(); i != all_proc.end(); i++) {
				pid_t wpid = waitpid(((*i).first), &status, WNOHANG);
				if (WIFEXITED(status) || WIFSIGNALED(status)) {
					cout<<"- ["<<((*i).second).get_parent()<<"] ended"<<endl;
					all_proc.erase((*i).first);
				}
				((*i).second).set_priority(status);
		}
	}
}

int find_pid(int p)
{
	for(auto x:all_proc)
		if((x.second).get_priority() == p)
			return x.first;	
	return 0;	
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

char* BaseDetails::print_term() {
	Modifier bg_green(BG_GREEN);
	Modifier bg_blue(BG_BLUE);
	Modifier fg_green(FG_GREEN);
	Modifier fg_blue(FG_BLUE);

	Modifier fg_def(FG_DEFAULT);
	Modifier bg_def(BG_DEFAULT);
	Modifier fg_black(FG_BLACK);
	char buff[PWD_LENGTH] = {'\0'};
	stringstream st;
    st.rdbuf()->pubsetbuf(buff, sizeof(buff));
	st<<bg_blue<<fg_black<<get_user()<<"@"<<get_host()<<" "<<
	bg_green<<fg_blue<<"\uE0B0 "<<fg_black<<get_cwd()<<" "<<
	bg_def<<fg_green<<"\uE0B0"<<bg_def<<fg_def<<" ";
	return readline(buff);
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


	pid = fork();
	int _pid = random(); 


	if (pid == 0) {
		if(is_bg)
			setpgid(0,0);	
		/* handling for the single type of command*/
		// case nightswatch
		if(strcmp(cmd[0], "nightswatch") == 0) {
			strcpy(cmd[0], "watch");
			strcpy(tokenized[0],"watch"); 
			one_statement(tokenized,cmd, false);

		}

		// case ls
		else if(strcmp(cmd[0],"ls")==0) ls(tokenized);

		// case echo
		else if(!strcmp(cmd[0], "echo")) echo(tokenized);

		
		else if(!strcmp(cmd[0],"interrupt")){
			FILE* memfile = fopen("/proc/interrupts","r");
			char line[STAT_LENGTH];

			if(memfile == NULL)
				cout<<"Couldn't open /proc/interrupts"<<endl;	
			else
				for(int i=0;fgets(line, sizeof(line),memfile);i++)
					if(i == 0 || i ==2)	{printf("%s",line);}
		}


		// pinfo custom command
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

		else if (strcmp(cmd[0],"jobs") == 0)
		{
			if(all_proc.size() > 0)
			for(auto x : all_proc)
			{		
				printf("[%d] %s %s %s [%d]\n", (x.second).get_priority(),(x.second).get_status(), (x.second).get_name(),(x.second).get_type()?"FG":"BG", x.first);
			}	


			else
				printf("No jobs\n"); 
		}	
		
		else if(strcmp(cmd[0],"kjob") == 0)
		{
			if(tokenized.size() != 3)
			{
				perror("Usage: kjob [Job number] [signal]");
				return 0;
			}

			int job_number = strtol(cmd[1],NULL,10);
			int signal_number = strtol(cmd[2],NULL,10);
			if(job_number == 0)
			{
				perror("Argument2 not a number");
				return 0;
			}

			int process_pid = find_pid(job_number);
			
			if(signal_number == 0 )
			{
				perror("Argument3 should a number");
				return 0;
			}

			kill(process_pid,signal_number);

		}

<<<<<<< HEAD
=======

		// else if(strcmp(cmd[0],"fg")==0)
		// {
		// 	if(tokenized.size() != 2)
		// 	{	perror("Usage: fg [job_number]"); return 0;}
		
		// 	int job_number = strtol(cmd[1],NULL,10);
		// 	if(job_number == 0)
		// 	{
		// 		perror("Argument2 not a number");
		// 		return 0;
		// 	}			
			// int process_pid = find_pid(job_number);



		// }

		else if(strcmp(cmd[0],"bg")==0)
		{
			if(tokenized.size() != 2)
			{	perror("Usage: bg [job_number]"); return 0;}
		
			int job_number = strtol(cmd[1],NULL,10);
			if(job_number == 0)
			{
				perror("Argument2 not a number");
				return 0;
			}			

			int process_pid = find_pid(job_number);
			if(process_pid == 0)
			{
				perror("No process found");
				return 0;
			}

			map<int, Process>::iterator it = all_proc.end(); 
			printf("%s\n", ((*it).second).get_status()	);
			if(strcmp(((*it).second).get_status(),"Zombie") == 0)
			{
				kill((*it).first,SIGCONT);

				printf("Killed process:%s\n",((*it).second).get_name());
			all_proc.erase((*it).first);
			}
	

		}



>>>>>>> 91ec6eb1544794aa27b1c4448f8ed66da5dfe412
		else {
			if (execvp(cmd[0], cmd) == -1) perror("shkell");
		}


		exit(0);

<<<<<<< HEAD

=======
		
>>>>>>> 91ec6eb1544794aa27b1c4448f8ed66da5dfe412

	} 
	else {

		Process p;
		p.set_pid(pid);
		p.set_parent(getpid());
		p.set_job(CHILD);
		p.set_name(cmd[0]);
		p.set_priority(all_proc.size() + 1);
		p.set_status();
<<<<<<< HEAD
		p.set_type(is_bg?0:1);
=======
>>>>>>> 91ec6eb1544794aa27b1c4448f8ed66da5dfe412
		all_proc[pid] = p;

		if(!is_bg){
			do
			wpid = waitpid(pid, &status, WUNTRACED);
			while (!WIFEXITED(status) && !WIFSIGNALED(status));
			all_proc.erase(pid);		
	
		}

		else
			cout<<"+ ["<<all_proc[pid].get_priority() <<"] "<<all_proc[pid].get_name()<<endl;

	}


}

int single_command(char cmd[]) {


	// due to different style of implementations we have to create this string. Can be removed during clean up.
	char cmd2[COMMAND_LENGTH];

	strcpy(cmd2,cmd);

	vector<char*> tokenized;
	char *temp = strtok(cmd2, " ");
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


	// redirect command
	bool is_redirect = false;
	for(auto i: tokenized)
		if(!is_redirect && (strstr(i, "<") || strstr(i, ">")  || strstr(i, "<<")  
			|| strstr(i, ">>") )) { is_redirect = true; break; }
	if(is_redirect) {
		handle_redirect(tokenized);
		return REDIRECT;
	}

	// piped command 
	int is_pipe = 0;
	for(int i =0; i< strlen(cmd);i++) {
		if(cmd[i] == '|') is_pipe ++;
	}
	if(is_pipe && cmd) {
		pipeline(cmd,is_pipe); 
		return PIPED;
	}		
		
	// normal command

	char** args = (char**) malloc((tokenized.size()+1) * sizeof(char*));
	int ic = 0;
	for(auto i: tokenized) args[ic++] = i;
		args[tokenized.size()] = NULL;

	if(strcmp(args[tokenized.size() - 1], "&") == 0) {
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
		
		return 0;
		}
	}

	else if(strcmp(args[0],"overkill") ==0)
	{
		for(auto x:all_proc)
			kill(x.first,SIGTERM);
	
		return 0;

	}

	else if(strcmp(args[0], "setenv") == 0)
		{if(tokenized.size() != 3)
			perror("Usage: setenv var [value]");
		else
			setenv(args[1],args[2],1);

		return 0;

		}

	else if(strcmp(args[0], "getenv") == 0){
		if(tokenized.size() != 2)
			perror("Usage: getenv var ");
		else
			if(getenv(args[1]) < 0) perror("Error");
	
		return 0;

	}	

	else if(strcmp(args[0], "unsetenv") == 0){
		if(tokenized.size() != 2)
			perror("Usage: getsetenv var");
		else
			if(unsetenv(args[1]) < 0) perror("Error");
	
		return 0;

	}

	else if(strcmp(args[0],"fg")==0)
	{
		if(tokenized.size() != 2)
			{	perror("Usage: fg [job_number]"); return 0;}
		
			int job_number = strtol(args[1],NULL,10);
			if(job_number == 0)
			{
				perror("Argument2 not a number");
				return 0;
			}			

			int process_pid = find_pid(job_number);
			if(process_pid == 0)
			{
				perror("No process found");
				return 0;
			}

			printf("Process: [%d] is now running in foreground\n",process_pid);
			 
			kill(process_pid,SIGCONT);
			all_proc[process_pid].set_status();
			all_proc[process_pid].set_type(1);
			waitpid(process_pid,NULL,WUNTRACED);
			all_proc.erase(process_pid);

			return 0;

	}


	else if(strcmp(args[0],"bg")==0)
	{
		if(tokenized.size() != 2)
		{	perror("Usage: bg [job_number]"); return 0;}
	
		int job_number = strtol(args[1],NULL,10);
		if(job_number == 0)
		{
			perror("Argument2 not a number");
			return 0;
		}			

		int process_pid = find_pid(job_number);
		if(process_pid == 0)
		{
			perror("No process found");
			return 0;
		}

		printf("Process: [%d] is now running in background\n",process_pid);
		 
		kill(process_pid,SIGCONT);
		all_proc[process_pid].set_status();
	
		return 0;

	}



	else if(strcmp(args[0], "alias")==0){
	
		if(tokenized.size() ==1)
			{for(map<char*,char*>::iterator iter = builtin_cmd.begin(); iter != builtin_cmd.end(); iter++)
				printf("%s=%s\n",(*iter).first,(*iter).second); return 0;}


		else if(tokenized.size() > 2)
			printf("Too many arguments\nUsage: alias a=\"cmd\" taking only the 1st argument\n");

		char* new_cmd;
		char* old_cmd;
		new_cmd = (char*)malloc(COMMAND_LENGTH*sizeof(char));
		old_cmd = (char*)malloc(COMMAND_LENGTH*sizeof(char));
		
		new_cmd = strtok(tokenized[1],"=");
		old_cmd = strtok(NULL,"=");
		if(old_cmd[0]=='\"')
			old_cmd = old_cmd + 1;

		if(old_cmd[strlen(old_cmd)-1] !='\"')
			{ printf("Correct usage: alias a=\"cmd\" \n"); return 0;}
		else
			old_cmd[strlen(old_cmd)-1]='\0';

		if(strcmp(new_cmd,old_cmd))
			{
			builtin_cmd[new_cmd] = old_cmd;
			printf("%s => %s\n",new_cmd,builtin_cmd[new_cmd]);
			}

			return 0;
	}



	for(map<char*,char*>::iterator iter = builtin_cmd.begin(); iter != builtin_cmd.end(); iter++)
		if(!strcmp((*iter).first,args[0]))
			{return single_command((*iter).second);}

	else
		return one_statement(tokenized,args,false);		

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
		if(!strcmp(cmd,"exit") || !strcmp(cmd,"quit")){
		 write_history("/home/shubh/Assignment/OS/Assignment2/cpp-shell/.shkell_history");
		 exit(0);
		 } // exit shell on quit

		return_type = single_command(cmd); 
	
	}
	
	return return_type; 
}