#ifndef ASMODULES_OPENCV_H_
#define ASMODULES_OPENCV_H_

#include <cmath> // Needed, see http://boost.2283326.n4.nabble.com/Boost-Python-Compile-Error-s-GCC-via-MinGW-w64-td3165793.html#a3166760
                 // and http://bugs.python.org/issue11566

#include <boost/python.hpp>
#include <string>
#include "../../ardrone/ardrone.h"
#include "../../imgprocui/imagevisualizer.h"
#include "../iscriptsimulationui.h"

/* Compile OpenCV with:
 * cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local -D BUILD_opencv_java=OFF -D PYTHON_EXECUTABLE=/usr/bin/python3 -D PYTHON_INCLUDE=/usr/include/python3 -D PYTHON_LIBRARY=/usr/lib/x86_64-linux-gnu/libpython3.3m.so -D PYTHON_PACKAGES_PATH=/usr/local/lib/python3.3/dist-packages/ -D PYTHON_NUMPY_INCLUDE_DIR=/usr/local/lib/python3.3/dist-packages/numpy/core/include/ -D WITH_FFMPEG=OFF ..
 * for support in Python 3 (use latest git version of OpenCV)
 */

class ImgProc
{
	public:
		ImgProc(ARDrone *drone = NULL, ImageVisualizer *iv = NULL, bool simulationMode = false, IScriptSimulationUI *simUI = NULL);

		PyObject *getLatestFrame();
		void showFrame(PyObject *frame);

		bool abortFlag = false;
	private:
		ARDrone *d = NULL;
		bool sim = true;
		IScriptSimulationUI *ssui;
		ImageVisualizer *_iv = NULL;
};

#endif
