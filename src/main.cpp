#ifdef __MINGW32__
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
	#include <windows.h>
#endif

#include "autoflight.h"
#include "afmainwindow.h"
#include <QApplication>
#include <iostream>
#include <Gamepad.h>

int main(int argc, char *argv[])
{
#ifdef __MINGW32__
	SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
#endif

	std::cout << "Starting AutoFlight...\n";
	
	Gamepad_init();

	AutoFlight af;

	QApplication gui(argc, argv);

	AFMainWindow w(&af);
	w.show();

	gui.exec();

	Gamepad_shutdown();

	std::cout << "Closing...\n";

	return 0;
}
