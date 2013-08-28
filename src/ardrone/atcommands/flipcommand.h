#ifndef FLIPCOMMAND_H
#define FLIPCOMMAND_H

#include "atcommand.h"

class FlipCommand : public ATCommand
{
	public:
		explicit FlipCommand(int direction) : ATCommand("CONFIG", std::vector<std::string>{ATCommand::_string("control:flight_anim"), ATCommand::_string(std::to_string(direction) + ",15")}) {}
};

#endif
