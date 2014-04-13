#ifndef ASENGINE_H
#define ASENGINE_H

#include <cmath> // Needed, see http://boost.2283326.n4.nabble.com/Boost-Python-Compile-Error-s-GCC-via-MinGW-w64-td3165793.html#a3166760

#include <string>
#include <vector>
#include <functional>
#include <boost/python.hpp>
#include "../ardrone/ardrone.h"
#include "../imgprocui/imagevisualizer.h"
#include "aserror.h"
#include "iscriptsimulationui.h"
#include "asmodules.h"
#include "opencv/asmodules_opencv.h"

#include <Python.h>


class ASEngine
{
	public:
		ASEngine(ARDrone *drone);
		~ASEngine();

		ARDrone *ardrone();

		std::vector<std::string> getAvailableFunctions();

		bool runScript(std::string script, bool simulate, IScriptSimulationUI *ssui, ImageVisualizer *iv, ASError *e, std::function<void(const std::string &)> outputCallback);
		void stopRunningScript();
		std::string getPythonVersion();
	private:
		ASError getLatestExceptionMessage();

		ARDrone *_drone;

		Sensors *_sensors = NULL;
		Control *_control = NULL;
		Util *_util = NULL;
		ImgProc *_imgproc = NULL;
		HWExt *_hwext = NULL;
};

#endif
