#include "autoflight.h"
#include "afconstants.h"

#include <iostream>
#include <boost/filesystem.hpp>
#include <QCoreApplication>
#include <QDir>

using namespace std;

AutoFlight::AutoFlight()
{
	_drone = new ARDrone();
	_ase = new ASEngine(_drone);

	_drone->setIP(ardrone::DEFAULT_IP);
	_drone->setSaveDirectory(getHomeDirectory().append("AutoFlightSaves/"));

	_srec = new SessionRecorder(_sessionRecDoc);
}

AutoFlight::~AutoFlight()
{
	delete _ase;
	delete _drone;
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

string AutoFlight::getHomeDirectory()
{
	string home = QDir::homePath().toStdString();

	if(home.back() != '/' && home.back() != '\\')
	{
		home.append("/");
	}

	return home;
}

ARDrone *AutoFlight::ardrone()
{
	return _drone;
}

ASEngine *AutoFlight::asengine()
{
	return _ase;
}

SessionRecorder *AutoFlight::sessionrecorder()
{
	return _srec;
}

bool AutoFlight::attemptConnectionToDrone()
{
	int connected = _drone->connect();
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
		_drone->startUpdateLoop();
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

void AutoFlight::saveSession()
{
	string filename = "AF_";
	filename.append(AutoFlight::af_timestamp());
	filename.append(".xml");

	string sessiondir = getHomeDirectory();
	sessiondir.append("AutoFlightSaves/Sessions/");

	boost::filesystem::create_directories(sessiondir);

	cout << "Saving session under " << (sessiondir + filename) << endl;

	_sessionRecDoc.save_file((sessiondir + filename).c_str());
}

string AutoFlight::af_timestamp()
{
	const boost::posix_time::ptime time = boost::posix_time::second_clock::local_time();

	stringstream timestamp;
	timestamp << setw(4) << setfill('0') << time.date().year() << setw(2) << time.date().month() << setw(2) << time.date().day();
	timestamp << "T";
	timestamp << setw(2) << time.time_of_day().hours() << setw(2) << time.time_of_day().minutes() << setw(2) << time.time_of_day().seconds();

	return timestamp.str();
}
