shkell :  classdef.h funcdef.cpp ls_cmd.cpp redirect.cpp echo.cpp pinfo_cmd.cpp handle_piping.cpp main.cpp signal_handlers.cpp
		g++ funcdef.cpp ls_cmd.cpp echo.cpp pinfo_cmd.cpp redirect.cpp handle_piping.cpp main.cpp signal_handlers.cpp -std=c++14 -lreadline -w -o shkell

