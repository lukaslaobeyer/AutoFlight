#ifndef EMERGENCYCOMMAND_H
#define EMERGENCYCOMMAND_H

#include "atcommand.h"

class EmergencyCommand : public ATCommand
{
	public:
		explicit EmergencyCommand(bool set) : ATCommand("REF", std::vector<std::string>{})
		{
			if(set)
			{
				setParameters(std::vector<std::string>{ATCommand::_int(0 | (1 << 8))});
			}
			else
			{
				setParameters(std::vector<std::string>{ATCommand::_int(0 & ~(1<<8))});
			}
			
		}
};

#endif
