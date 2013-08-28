#ifndef RECORDONUSBCOMMAND_H
#define RECORDONUSBCOMMAND_H

#include "atcommand.h"

class RecordOnUSBCommand : public ATCommand
{
	public:
		explicit RecordOnUSBCommand(bool record) : ATCommand("CONFIG", std::vector<std::string>{ATCommand::_string("video:video_on_usb"), ATCommand::_string(record? "TRUE" : "FALSE")}) {}
};

#endif
