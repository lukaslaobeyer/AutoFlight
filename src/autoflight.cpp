#include "autoflight.h"
#include "afconstants.h"

#include <iostream>
#include <QCoreApplication>

using namespace std;

AutoFlight::AutoFlight()
{
	_ase = new ASEngine(&_drone);
}

AutoFlight::~AutoFlight()
{
	delete _ase;
}

string AutoFlight::getProgramDirectory()
{
	string pathToApp = QCoreApplication::applicationDirPath().toStdString();

	if(pathToApp.back() != '/' && pathToApp.back() != '\\')
	{
		pathToApp.append("/");
	}

	return pathToApp;
}

ARDrone *AutoFlight::ardrone()
{
	return &_drone;
}

ASEngine *AutoFlight::asengine()
{
	return _ase;
}

bool AutoFlight::attemptConnectionToDrone()
{
	_drone.setIP("192.168.1.1");
	int connected = _drone.connect();
	switch(connected)
	{
	case ardrone::connection::ALREADY_CONNECTED:
		cout << "Already connected!\n";
		break;
	case ardrone::connection::CONNECTION_FAILED:
		cout << "Error connecting!\n";
		break;
	case ardrone::connection::EXCEPTION_OCCURRED:
		cout << "Exception occurred while connecting!\n";
		break;
	case ardrone::connection::CONNECTION_ESTABLISHED:
		cout << "Connected!\n";
		break;
	}

	try
	{
		_drone.startUpdateLoop();
	}
	catch(NotConnectedException &ex)
	{
		cout << ex.what() << endl;
	}

	if(connected == ardrone::connection::CONNECTION_ESTABLISHED || connected == ardrone::connection::ALREADY_CONNECTED)
	{
		return true;
	}
	else
	{
		return false;
	}
}
