#include "asengine.h"
#include "asmodules.h"
#include "opencv/asmodules_opencv.h"
#include <string>
#include <boost/filesystem.hpp>
#include "asioredirector.h"

using namespace std;
namespace py = boost::python;

BOOST_PYTHON_MODULE(autoscript)
{
	py::class_<Control>("Control")
				.def("takeOff", &Control::takeOff)
				.def("land", &Control::land)
				.def("move", &Control::move)
				.def("move_time", &Control::move_time)
				.def("move_distance", &Control::move_distance)
				.def("forward", &Control::forward)
				.def("forward_time", &Control::forward_time)
				.def("forward_distance", &Control::forward_distance)
				.def("left", &Control::left)
				.def("left_time", &Control::left_time)
				.def("left_distance", &Control::left_distance)
				.def("right", &Control::left)
				.def("right_time", &Control::right_time)
				.def("right_distance", &Control::right_distance)
				.def("up", &Control::up)
				.def("up_time", &Control::up_time)
				.def("up_distance", &Control::up_distance)
				.def("down", &Control::down)
				.def("down_time", &Control::down_time)
				.def("down_distance", &Control::down_distance)
				.def("backward", &Control::backward)
				.def("backward_time", &Control::backward_time)
				.def("backward_distance", &Control::backward_distance)
				.def("rotate", &Control::rotate)
				.def("hover", &Control::hover)
				.def("flip", &Control::flip);

	py::class_<Sensors>("Sensors")
				.def("getAltitude", &Sensors::getAltitude)
				.def("getOrientation", &Sensors::getOrientation)
				.def("getOrientation360", &Sensors::getOrientation360)
				.def("getAcceleration", &Sensors::getAcceleration)
				.def("getLinearVelocity", &Sensors::getLinearVelocity)
				.def("getBatteryLevel", &Sensors::getBatteryLevel);

	py::class_<Util>("Util")
				.def("isConnected", &Util::isConnected)
				.def("isFlying", &Util::isFlying)
				.def("stopRecording", &Util::stopRecording)
				.def("startRecording", &Util::startRecording)
				.def("flatTrim", &Util::flatTrim)
				.def("calibrateMagnetometer", &Util::calibrateMagnetometer);

	py::class_<ImgProc>("ImgProc")
				.def("getLatestFrame", &ImgProc::getLatestFrame)
				.def("showFrame", &ImgProc::showFrame);

	//bpy::class_<HWExt>("HWExt");
}

BOOST_PYTHON_MODULE(autoscriptioredirector)
{
	py::class_<ASIORedirector>("autoscriptioredirector", py::init<>())
			    .def("write", &ASIORedirector::write);
}

ASEngine::ASEngine(ARDrone *drone)
{
	PyImport_AppendInittab("autoscript", &PyInit_autoscript);
	PyImport_AppendInittab("autoscriptioredirector", &PyInit_autoscriptioredirector);

	if(Py_IsInitialized() == 0)
	{
		Py_SetProgramName((wchar_t *)"AutoFlight");
		Py_Initialize();
	}

	_drone = drone;
}

ASEngine::~ASEngine()
{

}

ARDrone *ASEngine::ardrone()
{
	return _drone;
}

vector<string> ASEngine::getAvailableFunctions()
{
	vector<string> funcs = {
			"control.takeOff()",
			"control.land()",
			"control.move(phi, theta, gaz, yaw)",
			"control.move_time(phi, theta, gaz, yaw, milliseconds)",
			"control.move_distance(phi, theta, gaz, yaw, centimeters)",
			"control.forward(speed)",
			"control.forward_time(speed, milliseconds)",
			"control.forward_distance(speed, centimeters)",
			"control.backward(speed)",
			"control.backward_time(speed, milliseconds)",
			"control.backward_distance(speed, centimeters)",
			"control.left(speed)",
			"control.left_time(speed, milliseconds)",
			"control.left_distance(speed, centimeters)",
			"control.right(speed)",
			"control.right_time(speed, milliseconds)",
			"control.right_distance(speed, centimeters)",
			"control.up(speed)",
			"control.up_time(speed, milliseconds)",
			"control.up_distance(speed, centimeters)",
			"control.down(speed)",
			"control.down_time(speed, milliseconds)",
			"control.down_distance(speed, centimeters)",
			"control.rotate(speed, degrees, clockwise)",
			"control.hover()",
			"control.flip(direction)",

			"sensors.getAltitude()",
			"sensors.getOrientation(axis)",
			"sensors.getOrientation360(axis)",
			"sensors.getAcceleration(axis)",
			"sensors.getBatteryLevel()",
			"sensors.getLinearVelocity(axis)",

			"util.isConnected()",
			"util.isFlying()",
			"util.startRecording()",
			"util.stopRecording()",
			"util.flatTrim()",
			"util.calibrateMagnetometer()",
			"util.changeView(camera)",

			"imgproc.getLatestFrame()",
			"imgproc.showFrame(frame)"
	};

	return funcs;
}

string ASEngine::getPythonVersion()
{
	string v = Py_GetVersion();

	return v.substr(0, v.find_first_of(' '));
}

bool ASEngine::runScript(string script, bool simulate, IScriptSimulationUI *ssui, ImageVisualizer *iv, ASError *e, function<void(const string &)> outputCallback)
{
	if(simulate && ssui == NULL)
	{
		// Make sure nothing is trying to simulate without providing a IScriptSimulationUI
		return false;
	}

	cout << "Hi" << endl;
	PyGILState_STATE state = PyGILState_Ensure();
	cout << "ok" << endl;

	_control = new Control(_drone, simulate, ssui);
	_sensors = new Sensors(_drone, simulate, ssui);
	_util = new Util(_drone, simulate, ssui);
	_imgproc = new ImgProc(_drone, iv, simulate, ssui);
	_hwext = new HWExt(_drone, simulate, ssui);

	bool initialized = false;
	bool error = false;

	try
	{
		// Initialize namespaces and modules
		py::object main_module = py::import("__main__");
		py::object main_namespace = main_module.attr("__dict__");

		py::dict global_namespace;
		py::dict local_namespace;

		global_namespace["__builtins__"] = main_namespace["__builtins__"];

		// Set up the standard output redirector
		py::object redirector_module((py::handle<>(PyImport_ImportModule("autoscriptioredirector"))));
		global_namespace["autoscriptioredirector"] = redirector_module;

		ASIORedirector redirector;
		redirector.addOutputListener(outputCallback);
		boost::python::import("sys").attr("stderr") = redirector;
		boost::python::import("sys").attr("stdout") = redirector;

		// Import AR.Drone control functions
		py::object autoscript_module((py::handle<>(PyImport_ImportModule("autoscript"))));
		global_namespace["autoscript"] = autoscript_module;

		global_namespace["control"] = py::ptr(_control);
		global_namespace["sensors"] = py::ptr(_sensors);
		global_namespace["util"] = py::ptr(_util);
		global_namespace["imgproc"] = py::ptr(_imgproc);
		//global_namespace["hwext"] = py::ptr(_hwext);

		initialized = true;

		py::exec(py::str(script), global_namespace, local_namespace);

		_drone->drone_hover();

		error = false;
	}
	catch(const py::error_already_set &ex)
	{
		_drone->drone_hover();

		if(e != NULL)
		{
			*e = getLatestExceptionMessage();

			if(!initialized)
			{
				e->internalError = true;
			}
			else
			{
				e->internalError = false;
			}
		}
		else
		{
			cout << getLatestExceptionMessage().message << endl;
		}

		error = true;
	}

	delete _control;
	delete _sensors;
	delete _util;
	delete _hwext;
	delete _imgproc;
	_control = NULL;
	_sensors = NULL;
	_util = NULL;
	_hwext = NULL;
	_imgproc = NULL;

	PyGILState_Release(state);

	return !error;
}

int pyQuit(void *) // Gets injected into the script by stopRunningScript
{
	PyErr_SetInterrupt();
    return -1;
}

void ASEngine::stopRunningScript()
{
	if(_control != NULL)
	{
		_control->abortFlag = true;
	}

	if(_sensors != NULL)
	{
		_sensors->abortFlag = true;
	}

	if(_util != NULL)
	{
		_util->abortFlag = true;
	}

	if(_imgproc != NULL)
	{
		_imgproc->abortFlag = true;
	}

	if(_hwext != NULL)
	{
		_hwext->abortFlag = true;
	}

	Py_AddPendingCall(&pyQuit, NULL);
}

ASError ASEngine::getLatestExceptionMessage()
{
	ASError e;
	e.linenumber = -1;

	stringstream ss;

	if(PyErr_Occurred())
	{
		PyObject *ptype, *pvalue, *ptraceback;
		PyErr_Fetch(&ptype, &pvalue, &ptraceback);
		PyErr_NormalizeException(&ptype, &pvalue, &ptraceback);

		if(ptype != NULL)
		{
			py::object type(py::handle<>(py::allow_null(ptype)));
			py::object value(py::handle<>(py::allow_null(pvalue)));

			string strErrorMessage = "No error message available";

			if(pvalue != NULL)
			{
				strErrorMessage = py::extract<string>(PyObject_Str(pvalue));
			}

			if(ptraceback != NULL)
			{
				py::object traceback(py::handle<>(py::allow_null(ptraceback)));

				long lineno = py::extract<long>(traceback.attr("tb_lineno"));
				string filename = py::extract<string>(traceback.attr("tb_frame").attr("f_code").attr("co_filename"));
				string funcname = py::extract<string>(traceback.attr("tb_frame").attr("f_code").attr("co_name"));

				e.linenumber = lineno;
				e.filename = filename;
				e.funcname = funcname;

				ss << "Exception occurred in line " << lineno << " (" << funcname << ", " << filename << "):\n" << strErrorMessage << endl;
			}
			else
			{
				ss << "Exception occured:\n" << strErrorMessage << endl;
			}
		}
	}

	e.message = ss.str();

	return e;
}
