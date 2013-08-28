#ifndef ATCOMMAND_H
#define ATCOMMAND_H

#include <string>
#include <vector>

class ATCommand
{
	public:
		explicit ATCommand(std::string cmd, std::vector<std::string> params);
		//~ATCommand();
		
		std::string getCommand();
		std::vector<std::string> getParameters();
		
		void setCommand(std::string cmd);
		void setParameters(std::vector<std::string> params);
		
		static std::string _float(float value);
		static std::string _int(int value);
		static std::string _string(std::string value);
	private:
		std::string _cmd;
		std::vector<std::string> _params;
		static unsigned int floatToIntBits(float f);
};

#endif
