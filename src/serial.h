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
	static const std::string crlf;

public:
	Serial();
	~Serial();
	bool Open(std::string path = "", speed_t baud = B115200, int data_bits = 8, Parity parity = N, int stop_bits = 1, bool xon_xoff = false);
	bool Open(int fd, speed_t baud = B115200, int data_bits = 8, Parity parity = N, int stop_bits = 1, bool xon_xoff = false);
	void Close();
	void  SetSpeed(std::string baud);
	void  SetSpeed(speed_t baud){
		baud_ = baud;
		setTermios();
	};
	void  SetDataBits(int data_bits) {
		data_bits_ = data_bits;
		setTermios();
	};
	void  SetParity(Parity parity) {
		parity_ = parity;
		setTermios();
	};
	void  SetStopBits(int stop_bits) {
		stop_bits_ = stop_bits;
		setTermios();
	};
	void SetFlow(bool xon_xoff) {
		xon_xoff_ = xon_xoff;
		setTermios();
	};
	bool GetTTY();
	int sync();
	bool GetLine(std::istream& stream, std::string& string, char delimiter);

private:
	bool setTermios();

protected:
	std::streambuf::int_type underflow();
	std::streambuf::int_type overflow(std::streambuf::int_type c);

private:
	int				fd_;
	std::string		tty_name_;
	struct termios	tio_old_;
	struct termios	tio_;
	speed_t			baud_;
	int				data_bits_;
	int				stop_bits_;
	bool			xon_xoff_;
	bool			flow_flag_;
	Parity			parity_;
	int				last_errno_;
	char			gbuf_[1024];
};

}

#endif /* SERIAL_H_ */
