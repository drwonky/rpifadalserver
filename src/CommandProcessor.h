/*
 * CommandProcessor.h
 *
 *  Created on: Dec 31, 2019
 *      Author: pedward
 */

#ifndef COMMANDPROCESSOR_H_
#define COMMANDPROCESSOR_H_

#include <iostream>
#include <string>
#include <unordered_map>
#include <map>
#include <functional>
#include <utility>
#include <vector>
#include "rpi.h"

namespace apsoft
{

class CommandProcessor
{
private:
	typedef std::vector<std::string> arglist_t;
	typedef std::function<void(arglist_t)> callback_t;

public:
	virtual ~CommandProcessor();
	CommandProcessor(std::iostream &stream);
	void Process(std::string token, std::string arguments);
	void RegisterCallback(const std::string &cmd, callback_t func) {
		callbacks_.insert(std::make_pair(cmd,func));
	};
	void RegisterHelp(const std::string &cmd, const std::string &help) {
		help_.insert(std::make_pair(cmd,help));
	};
	void RegisterCallbacks();
	arglist_t Split(std::string args);
	void LogError(std::string text);
	void Response(std::string text);
	void Bye();
	void Command_lv(arglist_t args);
	void Command_op(arglist_t args);
	void Command_ls(arglist_t args);
	void Command_rm(arglist_t args);
	void Command_sf(arglist_t args);
	void Command_help(arglist_t args);

private:
	std::iostream&								stream_;
	std::unordered_map<std::string, callback_t> callbacks_;
	std::map<std::string, std::string> 			help_;
	Rpi											rpi_;
};

} /* namespace apsoft */

#endif /* COMMANDPROCESSOR_H_ */
