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
	void print_term();
	char* get_cwd();
	BaseDetails();
	void update();
};

// each Process is of this type
class Process {
	char name[JOB_NAME];
	int job_type, status;
	pid_t pid;
	pid_t parent;
public:
	pid_t 	get_pid() 	{ return pid; 		}
	pid_t 	get_parent(){ return parent; 	}
	int 	get_type() 	{ return job_type; 	}
	char* 	get_name() 	{ return name; 		}

	void print_p() {
		cout<<"PID : "<<get_pid()<<"\tPARENT : "<<get_parent()<<endl
			<<"Job type : "<<job_type<<"\tName : "<<get_name()<<endl;
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
};

extern map<pid_t, Process> all_proc; // stores all the processess

// checks for background and prints to terminal
void check_bg(); 

// removes the padding and intermediate spaces
char* remove_padding(char s[]);

// exe_cmds makes multiple calls to single_command
int exe_cmds(char cmd[]);

// executes single command with piping etc
int single_command(char cmd[]);

// executes the basic one command
int one_statement(char* cmd[], bool);

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