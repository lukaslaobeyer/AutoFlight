#ifndef LANDCOMMAND_H
#define LANDCOMMAND_H

#include "atcommand.h"

class LandCommand : public ATCommand
{
	public:
		explicit LandCommand() : ATCommand("REF", std::vector<std::string>{ATCommand::_int(0 | 0)}) {}
};

#endif
