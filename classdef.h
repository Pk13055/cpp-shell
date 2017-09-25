#include <vector>
using namespace std;

// all the constants 
#define USER_NAME_LENGTH 200
#define SYSTEM_NAME_LENGTH 200
#define COMMAND_LENGTH 500
#define PWD_LENGTH 2048
#define JOB_NAME 200
#define FILE_NAME 200
#define STAT_LENGTH 1024
#define PID_LENGTH 10

// JOB TYPES
#define PARENT 0
#define CHILD 1
#define BACKGROUND 2
#define PIPED 3
#define REDIRECT 4


extern char home_dir[PWD_LENGTH / 2];

// stores the PS1 details and other stuff
class BaseDetails {
	char user_name[USER_NAME_LENGTH];
	char host_name[SYSTEM_NAME_LENGTH];
	char pwd1[PWD_LENGTH];
	void sub_home();
	char* get_host();
	char* get_user();
public:
	char* print_term();
	char* get_cwd();
	BaseDetails();
	void update();
};

// each Process is of this type
class Process {
	char name[JOB_NAME],status[10];
	int job_type,priority;
	pid_t pid;
	pid_t parent;
public:
	pid_t 	get_pid() 	{ return pid; 		}
	pid_t 	get_parent(){ return parent; 	}
	int 	get_type() 	{ return job_type; 	}
	int 	get_priority() { return priority; }
	char* 	get_name() 	{ return name; 		}
	char* 	get_status() { 
		if(strcmp(status,"R") == 0)
			return "Running";

		else if(strcmp(status,"S") == 0)
			return "Sleep";
		
		else if(strcmp(status,"Z") == 0)
			return "Zombie";

		else if(strcmp(status,"T") == 0)
			return "Stopped";

		else if(strcmp(status,"D") == 0)
			return "DEAD";

		else if(strcmp(status,"B") == 0)
			return "Build In";

		else
			return "Undefined";
	}
	
	void set_pid(pid_t t) 	{ pid = t; 			}
	void set_parent(pid_t t){ parent = t; 		}
	void set_job(int type) 	{ job_type = type; 	}
	void set_name(char s[]) { 
		if(strlen(s) < JOB_NAME) 
			strcpy(name, s);
		else
			strncpy(name, s, (size_t) JOB_NAME);
	}
	void set_priority(int x) {priority = x; }
	
	void set_status(){ 
		
	char stat_file[FILE_NAME]="/proc/";
	char line[STAT_LENGTH];

	char* buf = (char*)malloc(PID_LENGTH);
	sprintf(buf,"%d",pid);

	strcat(stat_file,buf);
	strcat(stat_file,"/stat");

	FILE* inputFile = fopen(stat_file,"r");

	if(inputFile == NULL)
		{ strcpy(status,"B"); }  	

	else
	{
		
		fgets(line, sizeof(line),inputFile);

		char* stat_param[100];
		stat_param[0] = strtok(line," ");
		for(int i=0; i<5 ;i++)
			stat_param[i+1] = strtok(NULL," ");

		strcpy(status,stat_param[2]);
	}

	}

};

extern map<pid_t, Process> all_proc; // stores all the processess

int find_pid(int p);

// checks for background and prints to terminal
void check_bg(); 

// removes the padding and intermediate spaces
char* remove_padding(char s[]);

// exe_cmds makes multiple calls to single_command
int exe_cmds(char cmd[]);

// executes single command with piping etc
int single_command(char cmd[]);

// executes the basic one command
int one_statement(vector<char*> tokenized,char* cmd[], bool);

// gets the input command from the user
char* getInput();

// implementation of ls
void ls(vector<char*>);

// implementation of echo
bool echo(vector<char*>);

// handling of redirect commands
int handle_redirect(vector<char*>);

void pinfo(vector<char*>);

int pipeline(char*, int);