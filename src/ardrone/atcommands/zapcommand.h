#ifndef ZAPCOMMAND_H
#define ZAPCOMMAND_H

#include "atcommand.h"

class ZapCommand : public ATCommand
{
	public:
		explicit ZapCommand(bool frontCamera) : ATCommand("CONFIG", std::vector<std::string>{ATCommand::_string("video:video_channel"), ATCommand::_string(frontCamera? "0" : "1")}) {}
};

#endif
