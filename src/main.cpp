#ifdef __MINGW32__
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
	#include <windows.h>
#endif

#include "autoflight.h"
#include "afmainwindow.h"
#include <boost/program_options.hpp>
#include <QApplication>
#include <iostream>
#include <string>
#include <Gamepad.h>

int main(int argc, char *argv[])
{
#ifdef __MINGW32__
	SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
#endif
	
	// Parse command line options with boost::program_options
	boost::program_options::options_description desc("Available options");
	desc.add_options()
			("help", "show help message")
			("ip-address", boost::program_options::value<std::string>(), "an alternative IP address for the AR.Drone (default is 192.168.1.1)")
			("stream-resolution", boost::program_options::value<std::string>(), "resolution for the live video stream (360P default, can be set to 720P)")
	;

	boost::program_options::variables_map vm;
	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
	boost::program_options::notify(vm);

	if(vm.count("help"))
	{
	    std::cout << desc << std::endl;
	    return 1;
	}

	std::string ip_address = ardrone::DEFAULT_IP;
	std::string stream_res = "360P";

	if(vm.count("ip-address"))
	{
		ip_address = vm["ip-address"].as<std::string>();
		std::cout << "AR.Drone IP address manually set to " << ip_address << std::endl;
	}
	else
	{
		std::cout << "Using default AR.Drone IP address (192.168.1.1). Use the --ip-address option to choose a different address." << std::endl;
	}

	if(vm.count("stream-resolution"))
	{
		stream_res = vm["stream-resolution"].as<std::string>();
	}

	std::cout << "Starting AutoFlight...\n";

	Gamepad_init();
	Gamepad_detectDevices();

	AutoFlight af(ip_address);

	if(stream_res == "720P")
	{
		af.ardrone()->setDefaultLiveStreamCodec(ardrone::config::codec::H264_720P);
		std::cout << "AR.Drone live video stream resolution manually set to " << stream_res << std::endl;
	}

	QApplication gui(argc, argv);

	AFMainWindow w(&af);
	w.show();

	gui.exec();

	Gamepad_shutdown();

	std::cout << "Closing...\n";

	return 0;
}
