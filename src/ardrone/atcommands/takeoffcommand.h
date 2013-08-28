#ifndef TAKEOFFCOMMAND_H
#define TAKEOFFCOMMAND_H

#include "atcommand.h"

class TakeOffCommand : public ATCommand
{
	public:
		explicit TakeOffCommand() : ATCommand("REF", std::vector<std::string>{ATCommand::_int((1 << 9) | 0)}) {}
};

#endif
