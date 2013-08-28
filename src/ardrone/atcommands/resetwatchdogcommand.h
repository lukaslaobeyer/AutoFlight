#ifndef RESETWATCHDOGCOMMAND_H
#define RESETWATCHDOGCOMMAND_H

#include "atcommand.h"

class ResetWatchdogCommand : public ATCommand
{
	public:
		explicit ResetWatchdogCommand() : ATCommand("COMWDG", std::vector<std::string>{}) {}
};

#endif
