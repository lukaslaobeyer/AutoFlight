#ifndef ASIOREDIRECTOR_H
#define ASIOREDIRECTOR_H

#include <string>
#include <vector>
#include <functional>

class ASIORedirector
{
	public:
		void addOutputListener(std::function<void(const std::string &)> callback);
		void removeOutputListeners();
		void write(const std::string &str);
	private:
		std::vector<std::function<void(const std::string &)>> callbacks;
};

#endif
