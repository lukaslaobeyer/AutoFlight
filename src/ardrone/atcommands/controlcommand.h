#ifndef CONTROLCOMMAND_H
#define CONTROLCOMMAND_H

#include "atcommand.h"

class ControlCommand : public ATCommand
{
	public:
		explicit ControlCommand(int value) : ATCommand("CTRL", std::vector<std::string>{ATCommand::_int(value), ATCommand::_int(0)}) {}
};

#endif
