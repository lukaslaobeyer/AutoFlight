#include "atcommand.h"
#include <string.h>

using namespace std;

ATCommand::ATCommand(string cmd, vector<string> params)
{
	_cmd = cmd;
	_params = params;
}

string ATCommand::getCommand()
{
	return _cmd;
}

vector<string> ATCommand::getParameters()
{
	return _params;
}

void ATCommand::setCommand(string cmd)
{
	_cmd = cmd;
}

void ATCommand::setParameters(vector<string> params)
{
	_params = params;
}

string ATCommand::_float(float value)
{
	return to_string(floatToIntBits(value));
}

string ATCommand::_int(int value)
{
	return to_string(value);
}

string ATCommand::_string(string value)
{
	return '"' + value + '"';
}

unsigned int ATCommand::floatToIntBits(float f)
{
   static_assert(sizeof(float) == sizeof(unsigned int), "sizeof(float) must be equal to sizeof(unsigned int)!");
   unsigned int ret;
   memcpy(&ret, &f, sizeof(float));
   return ret;
}
