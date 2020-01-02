/*
 * rpi.cpp
 *
 *  Created on: Jan 1, 2020
 *      Author: pedward
 */

#include "rpi.h"
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <iostream>
#include <fstream>

namespace apsoft
{

const std::string Rpi::mediaPath = "/media/pi/";

Rpi::Rpi()
{
	// TODO Auto-generated constructor stub
	path_ = Rpi::mediaPath;
	last_errno_ = 0;
}

Rpi::~Rpi()
{
	// TODO Auto-generated destructor stub
}

std::vector<std::string> Rpi::GetAvailablePaths(std::string path)
{
	return GetDirectoryListing(path);
}

std::vector<std::string> Rpi::GetDirectoryListing(std::string path)
{

	if (path.empty()) {
		path = path_;
	}

	DIR	*dir;
	struct dirent *dentry;
	std::vector<std::string> dentries;

	dir = opendir(path.c_str());

	if (dir == NULL) {
		last_errno_ = errno;
		throw last_errno_;
	}

	errno = 0;
	while((dentry = readdir(dir)) != NULL) {
		if (dentry->d_name[0] != '.') {
			dentries.push_back(dentry->d_name);
		}
	}

	if (errno != 0) {
		last_errno_ = errno;
		throw last_errno_;
	}

	return dentries;
}

std::ifstream Rpi::OpenFile(std::string path)
{
//	std::cerr << "opening "<<path<<std::endl;
	std::string filepath = path_ + path;

	if (access(filepath.c_str(), R_OK | X_OK | F_OK) == 0) {

		std::ifstream file(filepath);

		if (file.good()) {
			return file;
		} else {
			last_errno_ = errno;
			throw last_errno_;
		}
	} else {
//		std::cerr << "access failed" << std::endl;
		last_errno_ = errno;
		throw last_errno_;
	}
}

bool Rpi::OpenPath(std::string path)
{
	if (access(path.c_str(), R_OK | X_OK | F_OK) == 0) {
		path_ = path;

		if (path_.back() != '/') path_+="/";
		return true;
	} else {
		last_errno_ = errno;
		throw last_errno_;
	}
}

} /* namespace apsoft */
