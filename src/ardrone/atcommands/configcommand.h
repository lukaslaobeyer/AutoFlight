#ifndef CONFIGCOMMAND_H
#define CONFIGCOMMAND_H

#include "atcommand.h"
#include <string>

class ConfigCommand : public ATCommand
{
	public:
		explicit ConfigCommand(std::string key, std::string value) : ATCommand("CONFIG", std::vector<std::string>{ATCommand::_string(key), ATCommand::_string(value)}) {}
};

#endif
