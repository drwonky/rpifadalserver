/*
 ============================================================================
 Name        : rpifadalserver.cpp
 Author      : Perry Harrington
 Version     : 0.1a
 Copyright   : Copyright 2019 Perry Harrington
 Description : Raspberry Pi based file server for Fadal CNC control
 ============================================================================
 */

#include <iostream>
#include <algorithm>
#include "serial.h"
#include "CommandProcessor.h"

using namespace std;

int main(int argc, char *argv[]) {
	apsoft::Serial tty;

	if (argc > 1) {
		tty.Open(argv[1]);
	} else {
		tty.Open();
	}

	string input;
	string token;
	string args;
	size_t pos;

	apsoft::CommandProcessor command(tty);

	tty.setf(ios::left);
	while(std::getline(tty, input, '+')) {
		std::transform(input.begin(), input.end(),input.begin(), ::toupper);
//		cout << "Read command '" << input << "'" << apsoft::Serial::crlf;

		if (input == "EXIT") break;

		pos = input.find_first_of(",");

		if (pos != std::string::npos) {
			token = input.substr(0,pos);
			args = input.substr(pos+1,std::string::npos);
		} else {
			token = input;
			args.clear();
		}

//		cout << "Read token '"<<token<<"'"<< apsoft::Serial::crlf;
//		cout << "Process" << apsoft::Serial::crlf;

		command.Process(token,args);

	}

	return 0;
}
