/*
 * rpi.h
 *
 *  Created on: Jan 1, 2020
 *      Author: pedward
 */

#ifndef RPI_H_
#define RPI_H_

#include <string>
#include <string.h>
#include <vector>
#include <fstream>

namespace apsoft
{

class Rpi
{
public:
	typedef std::vector<std::string> dirlist_t;
	static const std::string mediaPath;

public:
	Rpi();
	virtual ~Rpi();
	bool OpenPath(std::string path = mediaPath);
	std::ifstream OpenFile(std::string path);
	dirlist_t GetAvailablePaths(std::string path = mediaPath);
	dirlist_t GetDirectoryListing(std::string path = "");
	std::string GetError(void) {
		std::string err(256,'\0');

		strerror_r(last_errno_,&err[0],err.capacity());

		return err;
	};


private:
	std::string		path_;
	int				last_errno_;

};

} /* namespace apsoft */

#endif /* RPI_H_ */
