#ifndef MAGNETOMETERCALIBRATIONCOMMAND_H
#define MAGNETOMETERCALIBRATIONCOMMAND_H

#include "atcommand.h"

class MagnetometerCalibrationCommand : public ATCommand
{
	public:
		explicit MagnetometerCalibrationCommand() : ATCommand("CALIB", std::vector<std::string>{ATCommand::_int(0)}) {}
};

#endif
