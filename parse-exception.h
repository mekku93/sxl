#ifndef __PARSE_EXCEPTION_H__
#define __PARSE_EXCEPTION_H__

#include <exception>
#include <string>
#include <sstream>
#include "token.h"

class ParseException : public exception {
	public:
		ParseException(string msg) {
			stringstream ss;
			ss << "ParseError: " << msg;
			this->msg = ss.str();
		}
		virtual const char* what() const throw() {
			return this->msg.c_str();
		}
	private:
		string msg;
};


#endif