#ifndef CONFIGIDSCOMMAND_H
#define CONFIGIDSCOMMAND_H

#include "atcommand.h"
#include "../../afconstants.h"

class ConfigIDSCommand : public ATCommand
{
	public:
		explicit ConfigIDSCommand() : ATCommand("CONFIG_IDS", std::vector<std::string>{ATCommand::_string(ardrone::app_id::SESSION_ID), ATCommand::_string(ardrone::app_id::PROFILE_ID), ATCommand::_string(ardrone::app_id::APPLICATION_ID)}) {}
};

#endif
