#include "asioredirector.h"

void ASIORedirector::addOutputListener(std::function<void(const std::string &)> callback)
{
	callbacks.push_back(callback);
}

void ASIORedirector::removeOutputListeners()
{
	callbacks.clear();
}

void ASIORedirector::write(const std::string &str) // Catches the output of the scripts
{
	for(unsigned int i = 0; i < callbacks.size(); i++)
	{
		if(callbacks[i] != NULL)
		{
			callbacks[i](str);
		}
	}
}
