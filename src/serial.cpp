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

namespace apsoft {

const std::string Serial::crlf = "\r\n";

void Serial::SetSpeed(std::string baud)
{
	int s;

	switch (stoi(baud)) {
		case 460800:
			s =  B460800;
			break;
		case 230400:
			s =  B230400;
			break;
		case 115200:
			s =  B115200;
			break;
		case 57600:
			s =  B57600;
			break;
		case 38400:
			s =  B38400;
			break;
		case 19200:
			s =  B19200;
			break;
		case 9600:
			s =  B9600;
			break;
		case 4800:
			s =  B4800;
			break;
		case 2400:
			s =  B2400;
			break;
		case 1800:
			s =  B1800;
			break;
		case 1200:
			s =  B1200;
			break;
		case 600:
			s =  B600;
			break;
		case 300:
			s =  B300;
			break;
		case 200:
			s =  B200;
			break;
		case 150:
			s =  B150;
			break;
		case 134:
			s =  B134;
			break;
		case 110:
			s =  B110;
			break;
		case 75:
			s =  B75;
			break;
		case 50:
			s =  B50;
			break;
		default:
			s = B0;
	}

	SetSpeed(s);
}

bool Serial::GetTTY()
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
		if (!GetTTY())
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

	data_bits_ = data_bits;
	baud_ = baud;
	parity_ = parity;
	stop_bits_ = stop_bits;
	xon_xoff_ = xon_xoff;

	tcgetattr(fd,&tio_old_); /* save current serial port settings */
	memcpy(&tio_, &tio_old_, sizeof(tio_));

	fd_ = fd;

	setTermios();

	char *p = gbuf_+sizeof(gbuf_);

	// setup buffer, tell it how big it is, and set the current pointer to the end so it knows to fetch data
	setg(gbuf_,p,p);

	return true;
}

bool Serial::setTermios()
{

	if (fd_ == -1) return false;

	auto dbToSym = [] (int data_bits) {
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

	auto pbToSym = [] (Parity parity) {
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

	auto sbToSym = [] (int stop_bits) {
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

	/*
	  BAUDRATE: Set bps rate. You could also use cfsetispeed and cfsetospeed.
	  CRTSCTS : output hardware flow control (only used if the cable has
		    all necessary lines. See sect. 7 of Serial-HOWTO)
	  CS8     : 8n1 (8bit,no parity,1 stopbit)
	  CLOCAL  : local connection, no modem contol
	  CREAD   : enable receiving characters
	*/
	tio_.c_cflag = baud_ | dbToSym(data_bits_) | pbToSym(parity_) | sbToSym(stop_bits_) | CLOCAL | CREAD;


	/*
	 Raw output.
	*/
	tio_.c_oflag = 0;

	// What cfmakeraw does, but gives us control over CSIZE:

    tio_.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tio_.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);

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
	tcflush(fd_, TCIFLUSH);
	tcsetattr(fd_,TCSANOW,&tio_);

	return true;
}

std::streambuf::int_type Serial::underflow()
{
	if (gptr() < egptr()) {
		return std::streambuf::traits_type::to_int_type(*gptr());
	}

	size_t buflen = gbuf_+sizeof(gbuf_)-eback();

restart:
	ssize_t res = ::read(fd_, gbuf_, buflen);

//	printf("read 0x%X input res %d\r\n",*gbuf_, res);

	if (res == -1) {
		last_errno_ = errno;
		return std::streambuf::traits_type::eof();
	} else if (res == 1) {
		if (xon_xoff_) { // Eat and process flow control characters
			switch (*gbuf_) {
			case 0x13:
				flow_flag_=true;
				return underflow();
				break;
			case 0x11:
				flow_flag_=false;
				return 0;
				break;
			}
		}
	}

	setg(gbuf_,gbuf_,gbuf_ + res);

//	std::cerr << "underflow (" << (int)*gptr()<<") " << gptr() << std::endl;
	return std::streambuf::traits_type::to_int_type(*gptr());
}

std::streambuf::int_type Serial::overflow(std::streambuf::int_type c)
{
//	std::cerr << "overflow (" << c << ") "<< (char)c << std::endl;

	if (xon_xoff_ == true && flow_flag_ == true) {
		while(flow_flag_ == true) underflow();
	}

	if (c != std::streambuf::traits_type::eof()) {
		if (::write(fd_,&c,1) == -1) {
			last_errno_ = errno;
			return std::streambuf::traits_type::eof();
		}
	} else {
		tcdrain(fd_);
	}

	return c;
}

bool Serial::GetLine(std::istream& stream, std::string& string, char delimiter)
{
	int	input;

	string.erase();
	while(1) {
		input = stream.get();

//		printf("got 0x%X\n",input);

		if (input == std::istream::traits_type::eof()) return input;

		switch (input) {
		case '\b':	// handle backspace
		case '\177':	// handle DEL
			string.pop_back();
			break;
		default:
			if (input == delimiter) return true;
			string += input;
		}
	}
}

int Serial::sync()
{
	overflow(std::streambuf::traits_type::eof());
	return 0;
}

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
	xon_xoff_=false;
	flow_flag_ = false;
}

Serial::~Serial()
{
	if (fd_ >= 0) {
		tcflush(fd_, TCIFLUSH);
		tcsetattr(fd_,TCSANOW,&tio_old_);
		Close();
	}
}

}
