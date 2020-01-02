/*
 * CommandProcessor.cpp
 *
 *  Created on: Dec 31, 2019
 *      Author: pedward
 */

#include "CommandProcessor.h"
#include "serial.h"
#include <iomanip>

namespace apsoft
{

CommandProcessor::CommandProcessor(std::iostream &stream)
: stream_(stream)
{
	// TODO Auto-generated constructor stub
	RegisterCallbacks();
}

CommandProcessor::~CommandProcessor()
{
	// TODO Auto-generated destructor stub
}

void CommandProcessor::Process(std::string token, std::string arguments)
{
	if (callbacks_.count(token)) {
		callback_t func = callbacks_.at(token);
		arglist_t args = Split(arguments);
		func(args);
	}
}

CommandProcessor::arglist_t CommandProcessor::Split(std::string args)
{
	arglist_t arglist;
	int pos,last_pos=0;

	while((pos = args.find_first_of(",",last_pos)) != std::string::npos) {
		arglist.push_back(args.substr(last_pos,pos));
		last_pos=pos+1;
	}

	if (arglist.size() == 0) {
		arglist.push_back(args);
	} else if (args.length() > last_pos) {
		arglist.push_back(args.substr(last_pos,std::string::npos));
	}

	return arglist;
}

void CommandProcessor::LogError(std::string text)
{
	stream_ << "ERROR: " << text << Serial::crlf;
}

void CommandProcessor::Response(std::string text)
{
	stream_ << std::setw(40) << text << Serial::crlf;
}

void CommandProcessor::Bye()
{
	stream_ << "BYE" << Serial::crlf;
}

void CommandProcessor::RegisterCallbacks()
{
	using namespace std::placeholders;

	RegisterCallback("LV",std::bind(&CommandProcessor::Command_lv, this, _1));
	RegisterCallback("OP",std::bind(&CommandProcessor::Command_op, this, _1));
	RegisterCallback("LS",std::bind(&CommandProcessor::Command_ls, this, _1));
	RegisterCallback("RM",std::bind(&CommandProcessor::Command_rm, this, _1));
	RegisterCallback("SF",std::bind(&CommandProcessor::Command_sf, this, _1));
	RegisterCallback("HELP",std::bind(&CommandProcessor::Command_help, this, _1));

	RegisterHelp("LV","LV           LIST AVAILABLE VOLUMES");
	RegisterHelp("LS","LS           LIST FILES");
	RegisterHelp("OP","OP,VOL       OPEN DISK VOLUME");
	RegisterHelp("RM","RM,FILE      REMOVE FILE");
	RegisterHelp("SF","SF,FILE      SEND FILE");
}

void CommandProcessor::Command_lv(arglist_t args)
{
	Rpi::dirlist_t dentries;

	try {
		rpi_.OpenPath();

		dentries = rpi_.GetAvailablePaths();
	} catch (int errnum) {
		std::string error_text = rpi_.GetError();

		LogError("LISTING VOLUMES: " + error_text);
		return;
	}

	Response("AVAILABLE VOLUMES:");

	for(auto a:dentries) {
		Response(a);
	}
}

void CommandProcessor::Command_op(arglist_t args)
{
	if (args.capacity() == 0) {
		LogError("VOLUME NOT GIVEN");
		Response(help_.at("OP"));
	}

	try {
		rpi_.OpenPath(Rpi::mediaPath + args[0]);
	} catch (int errnum) {
		std::string error_text = rpi_.GetError();

		LogError("OPENING VOLUME: " + error_text);
		return;
	}

	Response("VOLUME " + args[0] + " OPENED");
}

void CommandProcessor::Command_ls(arglist_t args)
{
	Rpi::dirlist_t dentries;

	try {
		dentries = rpi_.GetDirectoryListing();
	} catch (int errnum) {
		std::string error_text = rpi_.GetError();

		LogError("LISTING DIRECTORY: " + error_text);
		return;
	}

	for(auto a:dentries) {
		Response(a);
	}
}

void CommandProcessor::Command_rm(arglist_t args)
{
	stream_ << "rm called: "<< Serial::crlf;

	for(auto a:args) {
		stream_ << "Parsed arg :" << a << Serial::crlf;
	}
}

void CommandProcessor::Command_sf(arglist_t args)
{
	try {
		std::ifstream file;

		file = rpi_.OpenFile(args[0]);

		stream_ << (char)0x12;

		std::string line;

		int checksum = 0;

		while(std::getline(file, line)) {
			for(auto c:line) {
				checksum += c;
				if (checksum >= 9999) checksum -= 9999;
			}
			stream_ << line << std::endl;
		}

		stream_ << checksum << Serial::crlf;

		file.close();
	} catch (int errnum) {
		std::string error_text = rpi_.GetError();

		LogError("SENDING FILE " + args[0] + ":" + error_text);
		return;
	}

	Bye();

}

void CommandProcessor::Command_help(arglist_t args)
{
	Response("RPI FADAL FILE SERVER V0.1a");
	Response("");

	for(auto a:help_) {
		Response(a.second);
	}
}

} /* namespace apsoft */
