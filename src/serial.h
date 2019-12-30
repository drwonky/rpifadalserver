/*
 * serial.h
 *
 *  Created on: Dec 28, 2019
 *      Author: pedward
 */

#ifndef SERIAL_H_
#define SERIAL_H_

#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>
#include <streambuf>
#include <iostream>

namespace apsoft {

class Serial : public std::streambuf, public std::iostream
{
public:
	enum Parity { N = 0, E = 1, O = 2 };

public:
	Serial();
	~Serial();
	bool Open(std::string path = "", speed_t baud = B115200, int data_bits = 8, Parity parity = N, int stop_bits = 1, bool xon_xoff = false);
	bool Open(int fd, speed_t baud = B115200, int data_bits = 8, Parity parity = N, int stop_bits = 1, bool xon_xoff = false);
	void Close();
	bool SetSpeed(std::string baud);
	bool SetSpeed(speed_t baud);
	bool SetDataBits(int bits);
	bool SetParity(int parity);
	bool SetStopBits(int bits);
	bool GetMyTTY();
	int sync();

private:
	speed_t ParseBaud(std::string baud);

protected:
	std::streambuf::int_type underflow();
//	std::char_traits::int_type uflow();
	std::streambuf::int_type overflow(std::streambuf::int_type c);

private:
	int				fd_;
	std::string		tty_name_;
	struct termios	tio_old_;
	struct termios	tio_;
	speed_t			baud_;
	int				data_bits_;
	int				stop_bits_;
	Parity			parity_;
	int				last_errno_;
	char			gbuf_[1024];
	char			pbuf_[1024];

};

}

#endif /* SERIAL_H_ */
