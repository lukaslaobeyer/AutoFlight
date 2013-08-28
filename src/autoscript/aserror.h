#ifndef ASERROR_H
#define ASERROR_H

#include <string>

struct ASError
{
	std::string message;

	bool internalError;

	long linenumber;
	std::string filename;
	std::string funcname;
};

#endif
