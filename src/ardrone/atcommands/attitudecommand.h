#ifndef ATTITUDECOMMAND_H
#define ATTITUDECOMMAND_H

#include "atcommand.h"

class AttitudeCommand : public ATCommand
{
	public:
		explicit AttitudeCommand(float phi, float theta, float gaz, float yaw) : ATCommand("PCMD", std::vector<std::string>{"1", ATCommand::_float(phi), ATCommand::_float(theta), ATCommand::_float(gaz), ATCommand::_float(yaw)}) {}
};

#endif
