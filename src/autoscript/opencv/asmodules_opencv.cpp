#include "asmodules_opencv.h"
#include <opencv2/core.hpp>
#include "CVBoostConverter.hpp"

ImgProc::ImgProc(ARDrone *drone, ImageVisualizer *iv, bool simulationMode, IScriptSimulationUI *simUI)
{
	d = drone;
	sim = simulationMode;
	ssui = simUI;

	_iv = iv;

	import_array1();
}

PyObject *ImgProc::getLatestFrame()
{
	cv::Mat mat = d->getLatestFrame();

	PyObject *ret = bcvt::fromMatToNDArray(mat);

	return ret;
}

void ImgProc::showFrame(PyObject *frame)
{
	cv::Mat mat = bcvt::fromNDArrayToMat(frame);
	_iv->showImage(mat);
}
