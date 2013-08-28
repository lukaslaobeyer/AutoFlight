#ifndef HOVERCOMMAND_H
#define HOVERCOMMAND_H

#include "atcommand.h"

class HoverCommand : public ATCommand
{
	public:
		explicit HoverCommand() : ATCommand("PCMD", std::vector<std::string>{"0", "0", "0", "0", "0"}) {}
};

#endif
