#ifndef ISCRIPTSIMULATIONUI_H
#define ISCRIPTSIMULATIONUI_H

#include <string>

class IScriptSimulationUI
{
	public:
		virtual ~IScriptSimulationUI() {}
		virtual void printAction(std::string action) = 0;
	    virtual bool getSimulatedBoolInput(std::string text) = 0;
	    virtual float getSimulatedFloatInput(std::string text, std::string unit) = 0;
};

#endif
