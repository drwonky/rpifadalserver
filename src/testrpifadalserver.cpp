/*
 ============================================================================
 Name        : rpifadalserver.cpp
 Author      : Perry Harrington
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C++,
 ============================================================================
 */

#include <iostream>
#include "serial.h"

using namespace std;

int main(int argc, char *argv[]) {
	apsoft::Serial tty;

	cout << "!!!Hello World!!!" << endl; /* prints !!!Hello World!!! */

	cout << "argc " << argc << endl;
	if (argc > 1) {
		tty.Open(argv[1]);
	} else {
		tty.Open();
	}

	cout<<"writing"<<endl;
	tty.write("test\r\n",6);
	cout<<"operator test output"<<endl;
	tty << "hello world test\r" << endl;
	cout << "send tty to cout"<<endl;
	sleep(5);
	try {
		tty << "disconnect test\r"<<endl;
	} catch (const std::exception& e) {
		std::cerr << "Caught exception "<< e.what() << endl;
	}
	cout << "past the disconnect test"<<endl;
	cout << "doing input test"<<endl;
	string input;
	try {
	tty >> input;
	cout << input << endl;
	} catch (const std::exception& e) {
		std::cerr << "Caught exception "<< e.what() << endl;
	}
	tty << input << "\r"<< endl;
	tty.sync();
	tty << "input was " << input << "\r"<< endl;
	//tty.GetMyTTY();

	return 0;
}
