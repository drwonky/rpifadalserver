/*
 * serial.cpp
 *
 *  Created on: Dec 28, 2019
 *      Author: pedward
 *
 *      C++ class for talking to and setting up serial ttys
 *
 */

#include "serial.h"

using namespace apsoft;

speed_t Serial::ParseBaud(std::string baud)
{
	switch (stoi(baud)) {
		case 460800:
			return B460800;
		case 230400:
			return B230400;
		case 115200:
			return B115200;
		case 57600:
			return B57600;
		case 38400:
			return B38400;
		case 19200:
			return B19200;
		case 9600:
			return B9600;
		case 4800:
			return B4800;
		case 2400:
			return B2400;
		case 1800:
			return B1800;
		case 1200:
			return B1200;
		case 600:
			return B600;
		case 300:
			return B300;
		case 200:
			return B200;
		case 150:
			return B150;
		case 134:
			return B134;
		case 110:
			return B110;
		case 75:
			return B75;
		case 50:
			return B50;
		default:
			return B0;
	}
}

bool Serial::GetMyTTY()
{
	if (!isatty(0)) {
		last_errno_ = errno;
		return false;
	}

	char pathname[255];
	int ret;

	if ((ret = ttyname_r(0,pathname,sizeof(pathname))) != 0) {
		last_errno_ = ret;
		return false;
	}

	tty_name_ = pathname;

	std::cerr << "TTY name is " << tty_name_ << std::endl;

	return true;
}

bool Serial::Open(std::string path, speed_t baud, int data_bits, Parity parity, int stop_bits, bool xon_xoff)
{
	if (path.empty()) {
		if (!GetMyTTY())
			return false;

		fd_ = ::open(tty_name_.c_str(), O_RDWR | O_NOCTTY );
	} else {
		fd_ = ::open(path.c_str(), O_RDWR | O_NOCTTY );
	}

	if (fd_ == -1) {
		last_errno_ = errno;
		return false;
	}

	return Open(fd_, baud, data_bits, parity, stop_bits, xon_xoff);
}

bool Serial::Open(int fd, speed_t baud, int data_bits, Parity parity, int stop_bits, bool xon_xoff)
{
	auto dbToSym = [&] {
		switch (data_bits) {
		default:
		case 8:
				return CS8;
				break;
		case 7:
				return CS7;
				break;
		case 6:
				return CS6;
				break;
		case 5:
				return CS5;
				break;
		}
	};

	auto pbToSym = [&] {
		switch (parity) {
		default:
		case N:
				return 0;
				break;
		case E:
				return PARENB;
				break;
		case O:
				return PARENB | PARODD;
				break;
		}
	};

	auto sbToSym = [&] {
		switch (stop_bits) {
		default:
		case 1:
			return 0;
			break;
		case 2:
			return CSTOPB;
			break;
		}
	};

	tcgetattr(fd,&tio_old_); /* save current serial port settings */
	memcpy(&tio_, &tio_old_, sizeof(tio_));

	/*
	  BAUDRATE: Set bps rate. You could also use cfsetispeed and cfsetospeed.
	  CRTSCTS : output hardware flow control (only used if the cable has
		    all necessary lines. See sect. 7 of Serial-HOWTO)
	  CS8     : 8n1 (8bit,no parity,1 stopbit)
	  CLOCAL  : local connection, no modem contol
	  CREAD   : enable receiving characters
	*/
	tio_.c_cflag = baud | dbToSym() | pbToSym() | sbToSym() | CLOCAL | CREAD;

	/*
	  IGNPAR  : ignore bytes with parity errors
	  ICRNL   : map CR to NL (otherwise a CR input on the other computer
		    will not terminate input)
	  otherwise make device raw (no other input processing)
	*/
	 //tio_.c_iflag = IGNPAR | IGNBRK;
	tio_.c_iflag = xon_xoff ? IXON : 0;

	/*
	 Raw output.
	*/
	tio_.c_oflag = 0;

	/*
	  ICANON  : enable canonical input
	  disable all echo functionality, and don't send signals to calling program
	*/
	 //tio_.c_lflag = ICANON;
	tio_.c_lflag = 0;

	cfmakeraw(&tio_);

	/*
	  initialize all control characters
	  default values can be found in /usr/include/termios.h, and are given
	  in the comments, but we don't need them here
	*/
	tio_.c_cc[VINTR]    = 0;     /* Ctrl-c */
	tio_.c_cc[VQUIT]    = 0;     /* Ctrl-\ */
	tio_.c_cc[VERASE]   = 0;     /* del */
	tio_.c_cc[VKILL]    = 0;     /* @ */
	tio_.c_cc[VEOF]     = 0;     /* Ctrl-d */
	tio_.c_cc[VTIME]    = 0;     /* inter-character timer unused */
	tio_.c_cc[VMIN]     = 1;     /* blocking read until 1 character arrives */
	tio_.c_cc[VSWTC]    = 0;     /* '\0' */
	tio_.c_cc[VSTART]   = 0;     /* Ctrl-q */
	tio_.c_cc[VSTOP]    = 0;     /* Ctrl-s */
	tio_.c_cc[VSUSP]    = 0;     /* Ctrl-z */
	tio_.c_cc[VEOL]     = 0;     /* '\0' */
	tio_.c_cc[VREPRINT] = 0;     /* Ctrl-r */
	tio_.c_cc[VDISCARD] = 0;     /* Ctrl-u */
	tio_.c_cc[VWERASE]  = 0;     /* Ctrl-w */
	tio_.c_cc[VLNEXT]   = 0;     /* Ctrl-v */
	tio_.c_cc[VEOL2]    = 0;     /* '\0' */

	/*
	  now clean the modem line and activate the settings for the port
	*/
	tcflush(fd, TCIFLUSH);
	tcsetattr(fd,TCSANOW,&tio_);

	fd_ = fd;

	char *p = gbuf_+sizeof(gbuf_);

	setg(gbuf_,p,p);
	setp(pbuf_, pbuf_ + sizeof(pbuf_));

	return true;
}

std::streambuf::int_type Serial::underflow()
{
	if (gptr() < egptr()) {
		return std::streambuf::traits_type::to_int_type(*gptr());
	}

	size_t buflen = gbuf_+sizeof(gbuf_)-eback();

	ssize_t res = ::read(fd_, eback(), buflen);

	setg(eback(),eback(),eback() + res);

	return std::streambuf::traits_type::to_int_type(*gptr());
}

std::streambuf::int_type Serial::overflow(std::streambuf::int_type c)
{
	if (c != std::streambuf::traits_type::eof()) {
		::write(fd_,&c,1);
	}

	return c;
}

int Serial::sync()
{
	overflow(EOF);
	setg(gbuf_, gbuf_ + sizeof(gbuf_), gbuf_ + sizeof(gbuf_));
	return 0;
}

//std::char_traits::int_type Serial::uflow() {}


void Serial::Close()
{
	if (fd_ >= 0) {
		close(fd_);
	}

	fd_ = -1;
}

Serial::Serial() :
std::iostream((std::streambuf *)this)
{
	fd_ = -1;
	last_errno_ = 0;
	baud_ = B115200;
	data_bits_ = 8;
	stop_bits_ = 1;
	parity_ = N;
}

Serial::~Serial()
{
	if (fd_ >= 0) {
		tcflush(fd_, TCIFLUSH);
		tcsetattr(fd_,TCSANOW,&tio_old_);
		Close();
	}
}
