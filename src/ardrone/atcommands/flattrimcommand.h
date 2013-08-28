#ifndef FLATTRIMCOMMAND_H
#define FLATTRIMCOMMAND_H

#include "atcommand.h"

class FlatTrimCommand : public ATCommand
{
	public:
		explicit FlatTrimCommand() : ATCommand("FTRIM", std::vector<std::string>{}) {}
};

#endif
