#include "../afconstants.h"
#include "../autoflight.h"
#include "ardrone.h"

#include "atcommands/configcommand.h"
#include "atcommands/configidscommand.h"
#include "atcommands/controlcommand.h"
#include "atcommands/attitudecommand.h"
#include "atcommands/emergencycommand.h"
#include "atcommands/flattrimcommand.h"
#include "atcommands/flipcommand.h"
#include "atcommands/hovercommand.h"
#include "atcommands/landcommand.h"
#include "atcommands/magnetometercalibrationcommand.h"
#include "atcommands/recordonusbcommand.h"
#include "atcommands/resetwatchdogcommand.h"
#include "atcommands/takeoffcommand.h"
#include "atcommands/zapcommand.h"

#include <Gamepad.h>

#include <chrono>
#include <thread>
#include <cmath>

#include <boost/filesystem.hpp>

#ifndef M_PI
	#define M_PI 3.141592653589793238462643383279502884197169399375105820974944592307816406
#endif

using namespace std;

ARDrone::ARDrone() : ARDrone(ardrone::DEFAULT_IP) {}

ARDrone::ARDrone(string ip)
{
	_ip = ip;
	_saveDir = "";

	addNavdataListener(&_ndrecorder);
}

ARDrone::~ARDrone()
{
	if(!_stop_flag)
	{
		stopUpdateLoop();
	}
}

void ARDrone::setIP(string ip)
{
	_ip = ip;
}

void ARDrone::setSaveDirectory(string saveDir)
{
	_saveDir = saveDir;
	if(_saveDir.back() != '/' && _saveDir.back() != '\\')
	{
		_saveDir.append("/");
	}
}

void ARDrone::setSessionRecorder(SessionRecorder *srec)
{
	_srec = srec;
}

int ARDrone::connect()
{
	if(!_connected)
	{
		try
		{
			if(_io_service == nullptr)
			{
				_io_service = new boost::asio::io_service;
			}

			// Initialize communication with AR.Drone
			_cl.init(_ip, *_io_service);
		
			// Needed for the AR.Drone to send full navigation data and accept commands (Somewhat like described in the dev guide in section 7.1.2)
			_cl.setAppID();
			boost::this_thread::sleep_for(boost::chrono::milliseconds(250)); // Wait until the drone has performed its configuration switch (important)
			_cl.sendATCommands(vector<ATCommand>{ConfigIDSCommand(), ConfigCommand("general:navdata_demo", "TRUE"), ControlCommand(0)});
			boost::this_thread::sleep_for(boost::chrono::milliseconds(250));
			_cl.sendATCommands(vector<ATCommand>{ConfigIDSCommand(), ConfigCommand("general:navdata_demo", "FALSE"), ConfigIDSCommand(), ConfigCommand("general:navdata_options", "268435455"), ControlCommand(5)});

			// Wait for the AR.Drone to process the commands
			boost::this_thread::sleep_for(boost::chrono::milliseconds(50));
		
			drone_setConfiguration(ardrone::config::VIDEO_CODEC, ardrone::config::codec::H264_720P);

			// Init navdata manager
			_nm.init(_ip, *_io_service);

			// Wait for navdata packets to be received
			boost::this_thread::sleep_for(boost::chrono::milliseconds(100));

			// Process received packets (if any)
			_nm.update();

			// Check that navdata has indeed been received
			_connected = _nm.isConnected();

			if(_connected)
			{
				_vm.init(_ip, *_io_service);
				_vm.update();

				for(IConnectionStatusListener *cslistener : _cslisteners)
				{
					cslistener->connectionEstablished();
				}

				return ardrone::connection::CONNECTION_ESTABLISHED;
			}
			else
			{
				_cl.close();
				_nm.close();
				return ardrone::connection::CONNECTION_FAILED;
			}
		}
		catch(const boost::system::system_error &e)
		{
			cerr << "Error: " << e.what() << endl;
			if(e.code().value() != 1)
			{
				try
				{
					_cl.close();
				}
				catch(const boost::system::system_error &e)
				{}
			}
			return ardrone::connection::EXCEPTION_OCCURRED;
		}
	}
	else
	{
		return ardrone::connection::ALREADY_CONNECTED;
	}
}

bool ARDrone::isConnected()
{
	return _connected;
}

void ARDrone::addNavdataListener(INavdataListener *listener)
{
	_ndlisteners.push_back(listener);
}

void ARDrone::removeNavdataListener(INavdataListener *listener)
{
	_ndlisteners.erase(remove(_ndlisteners.begin(), _ndlisteners.end(), listener), _ndlisteners.end());
}

void ARDrone::addConnectionStatusListener(IConnectionStatusListener *listener)
{
	_cslisteners.push_back(listener);
}

void ARDrone::removeConnectionStatusListener(IConnectionStatusListener *listener)
{
	_cslisteners.erase(remove(_cslisteners.begin(), _cslisteners.end(), listener), _cslisteners.end());
}

void ARDrone::addVideoListener(IVideoListener *listener)
{
	_vlisteners.push_back(listener);
}

void ARDrone::removeVideoListener(IVideoListener *listener)
{
	_vlisteners.erase(remove(_vlisteners.begin(), _vlisteners.end(), listener), _vlisteners.end());
}

void ARDrone::addControllerInputListener(IControllerInputListener *listener)
{
	_ctrllisteners.push_back(listener);
}

void ARDrone::removeControllerInputListener(IControllerInputListener *listener)
{
	_ctrllisteners.erase(remove(_ctrllisteners.begin(), _ctrllisteners.end(), listener), _ctrllisteners.end());
}

void ARDrone::setControllerConfiguration(ControllerConfiguration *config)
{
	_controllerconfig = config;
}

ControllerConfiguration *ARDrone::getControllerConfiguration()
{
	return _controllerconfig;
}

void ARDrone::startUpdateLoop()
{
	if(_updater == NULL)
	{
		if(_connected)
		{
			_stop_flag = false;
			_updater = new boost::thread(&ARDrone::runUpdateLoop, this);
		}
		else
		{
			throw NotConnectedException();
		}
	}
	else if(_connected)
	{
		// When the connection is lost the _updater thread still exists. If it is reestablished, the it needs to be killed.

		stopUpdateLoop();
		_connected = true;
		startUpdateLoop();
	}
}

void ARDrone::stopUpdateLoop()
{
	_stop_flag = true;
	_connected = false;

	if(_updater != NULL)
	{
		try
		{
			_updater->join();
		}
		catch(boost::thread_interrupted &)
		{}

		delete _updater;
		_updater = NULL;
	}
}

void ARDrone::runUpdateLoop()
{
	cout << "Update loop started" << endl;

	int zero_packets_counter = 0; // Counts number of times the io_service.poll() function returns 0
	                              // Used to detect connection loss

	try
	{
		while(!_stop_flag)
		{
			boost::this_thread::sleep_for(boost::chrono::milliseconds(25));

			// Process received packets (if any): Important!
			_io_service->reset();
			int packets = _io_service->poll();

			if(packets == 0)
			{
				zero_packets_counter++;

				if(zero_packets_counter == 15)
				{
					// Definitely lost connection
					cout << "[WARNING] Lost connection to AR.Drone!" << endl;

					// Reset state to unconnected
					_vm.stopRecording();

					_vm.close();
					_nm.close();
					_cl.close();

					_io_service->stop();
					delete _io_service;
					_io_service = nullptr;

					for(IConnectionStatusListener *cslistener : _cslisteners)
					{
						cslistener->connectionLost();
					}

					_stop_flag = true;
					_connected = false;

					break;
				}
			}
			else
			{
				zero_packets_counter = 0;
			}

			// Get newest navdata
			_navdatamutex.lock();
			_nd = _nm.getNavdata();

			if(_nd != NULL)
			{
				for(INavdataListener *i : _ndlisteners)
				{
					i->navdataAvailable(_nd);
				}
			}
			_navdatamutex.unlock();

			processControllerInput();

			// Process command queue
			_commandmutex.lock();
			if((_phi == 0.0f) && (_theta == 0.0f) && (_gaz == 0.0f) && (_yaw == 0.0f))
			{
				_commandqueue.push_back(HoverCommand());
			}
			else
			{
				_commandqueue.push_back(AttitudeCommand(_phi, _theta, _gaz, _yaw));
			}
			_cl.sendATCommands(_commandqueue);
			_commandqueue.clear();
			_commandmutex.unlock();

			// Get newest video frame
			_frame = _vm.getVideoFrame();
			if(!_frame.empty())
			{
				for(IVideoListener *i : _vlisteners)
				{
					i->videoFrameAvailable(_frame);
				}
			}
		}
	}
	catch(const std::exception &e)
	{
		cerr << "[FATAL] Unexpected exception occurred: " << e.what() << endl;
	}

	cout << "Update loop stopped" << endl;
}

void ARDrone::processControllerInput()
{
	static const int N_CYCLES = 16;
	static const int N_COUNTERS = 8;
	static int cyclesToWait[] = { // Update loop cycles to wait for each action before accepting new commands:
			0, // 0 Takeoff          Needed to ensure that actions do not trigger multiple times on somewhat longer button presses.
			0, // 1 Land             When an action is triggered, the corresponding cycle counter is set to N_CYCLES and decremented
			0, // 2 Zap              by one every cycle until the counter reaches 0.
			0, // 3 Photo
			0, // 4 Recording
			0, // 5 Flip
			0, // 6 Slow mode
			0, // 7 Emergency
	};

	static ControllerInput *in = new ControllerInput;
	// TODO: Cleanup!

	if(_controllerconfig != NULL)
	{
		*in = {0, 0, 0, 0, false, false, false, false, false, false, false, false};

		Gamepad_device *device = Gamepad_deviceAtIndex(_controllerconfig->deviceID);
		Gamepad_processEvents();
		bool slow = false;

		// Decrement cyclesToWait counters if needed
		for(int i = 0; i < N_COUNTERS; i++)
		{
			if(cyclesToWait[i] > 0)
			{
				cyclesToWait[i]--;
			}
		}

		if(_controllerconfig->takeoff >= 0 && cyclesToWait[0] == 0)
		{
			if(device->buttonStates[_controllerconfig->takeoff])
			{
				drone_takeOff();
				in->takeOff = true;
				cyclesToWait[0] = N_CYCLES;
			}
		}
		if(_controllerconfig->land >= 0 && cyclesToWait[1] == 0)
		{
			if(device->buttonStates[_controllerconfig->land])
			{
				drone_land();
				in->land = true;
				cyclesToWait[1] = N_CYCLES;
			}
		}
		if(_controllerconfig->zap >= 0 && cyclesToWait[2] == 0)
		{
			if(device->buttonStates[_controllerconfig->zap])
			{
				drone_toggleView();
				in->toggleView = true;
				cyclesToWait[2] = N_CYCLES;
			}
		}
		if(_controllerconfig->photo >= 0 && cyclesToWait[3] == 0)
		{
			if(device->buttonStates[_controllerconfig->photo])
			{
				drone_takePicture();
				in->takePicture = true;
				cyclesToWait[3] = N_CYCLES;
			}
		}
		if(_controllerconfig->recording >= 0 && cyclesToWait[4] == 0)
		{
			if(device->buttonStates[_controllerconfig->recording])
			{
				drone_toggleRecording();
				in->toggleRecording = true;
				cyclesToWait[4] = N_CYCLES;
			}
		}
		if(_controllerconfig->flip >= 0 && cyclesToWait[5] == 0)
		{
			if(device->buttonStates[_controllerconfig->flip])
			{
				drone_flip(ardrone::flip::LEFT);
				in->flip = true;
				cyclesToWait[5] = N_CYCLES;
			}
		}
		if(_controllerconfig->slow >= 0 && cyclesToWait[6] == 0)
		{
			if(device->buttonStates[_controllerconfig->slow])
			{
				slow = true;
				in->slowMode = true;
				cyclesToWait[6] = N_CYCLES;
			}
		}
		if(_controllerconfig->emergency >= 0 && cyclesToWait[7] == 0)
		{
			if(device->buttonStates[_controllerconfig->emergency])
			{
				drone_emergency();
				in->emergency = true;
				cyclesToWait[7] = N_CYCLES;
			}
		}

		float phi = 0, theta = 0, gaz = 0, yaw = 0;

		if(_controllerconfig->rollM >= 0 && _controllerconfig->rollP >= 0)
		{
			phi += device->buttonStates[_controllerconfig->rollP];
			phi -= device->buttonStates[_controllerconfig->rollM];
		}
		else if(_controllerconfig->roll >= 0)
		{
			phi = device->axisStates[_controllerconfig->roll];
		}

		if(_controllerconfig->pitchM >= 0 && _controllerconfig->pitchP >= 0)
		{
			theta += device->buttonStates[_controllerconfig->pitchP];
			theta -= device->buttonStates[_controllerconfig->pitchM];
		}
		else if(_controllerconfig->pitch >= 0)
		{
			theta = device->axisStates[_controllerconfig->pitch];
		}

		if(_controllerconfig->heightM >= 0 && _controllerconfig->heightP >= 0)
		{
			gaz += device->buttonStates[_controllerconfig->heightP];
			gaz -= device->buttonStates[_controllerconfig->heightM];
		}
		else if(_controllerconfig->height >= 0)
		{
			gaz = -device->axisStates[_controllerconfig->height];
		}

		if(_controllerconfig->yawM >= 0 && _controllerconfig->yawP >= 0)
		{
			yaw += device->buttonStates[_controllerconfig->yawP];
			yaw -= device->buttonStates[_controllerconfig->yawM];
		}
		else if(_controllerconfig->yaw >= 0)
		{
			yaw = device->axisStates[_controllerconfig->yaw];
		}

		if(slow)
		{
			phi *= 0.3f;
			theta *= 0.3f;
			gaz *= 0.5f;
			yaw *= 0.5f;
		}

		// Ignore very small values
		if(abs(phi) < 0.1f)
		{
			phi = 0;
		}
		if(abs(theta) < 0.1f)
		{
			theta = 0;
		}
		if(abs(gaz) < 0.1f)
		{
			gaz = 0;
		}
		if(abs(yaw) < 0.1f)
		{
			yaw = 0;
		}


		drone_move(phi, theta, gaz, yaw);

		in->altitude = gaz;
		in->yaw = yaw;
		in->pitch = theta;
		in->roll = phi;

		for(IControllerInputListener *l : _ctrllisteners)
		{
			l->controllerInputAvailable(in);
		}
	}
}

// AR.Drone navdata getting functions

ardrone::orientation ARDrone::drone_getOrientation()
{
	if(!isConnected())
	{
		ardrone::orientation error;
		error.yaw = -1;
		error.pitch = -1;
		error.roll = -1;
		return error;
	}

	ardrone::orientation o;

	_navdatamutex.lock();
	o.yaw = _nd->psi;
	o.pitch = _nd->theta;
	o.roll = _nd->phi;
	_navdatamutex.unlock();

	return o;
}

float ARDrone::drone_getRelativeHeading(bool clockwise)
{
	ardrone::orientation o = drone_getOrientation();
	float heading = o.yaw;

	if(clockwise)
	{
		if(heading < 0)
		{
			heading += 360;
		}
	}
	else
	{
		if(heading >= 0)
		{
			heading = 360 - heading;
		}
		else
		{
			heading *= -1;
		}
	}

	return heading;
}

ardrone::linearvelocity ARDrone::drone_getLinearVelocity()
{
	if(!isConnected())
	{
		ardrone::linearvelocity error;
		error.vx = -1;
		error.vy = -1;
		error.vz = -1;
		return error;
	}

	ardrone::linearvelocity l;

	_navdatamutex.lock();
	l.vx = _nd->vx;
	l.vy = _nd->vy;
	l.vz = _nd->vz;
	_navdatamutex.unlock();

	return l;
}

float ARDrone::drone_getAltitude()
{
	if(!isConnected())
	{
		return -1;
	}

	float a;

	_navdatamutex.lock();
	a = _nd->altitude;
	_navdatamutex.unlock();

	return a;
}

ardrone::position ARDrone::drone_getPosition() //TODO: Implement this!
{
	if(!isConnected())
	{
		ardrone::position error;
		error.x = -1;
		error.y = -1;
		return error;
	}

	ardrone::position p;

	_navdatamutex.lock();

	_navdatamutex.unlock();

	return p;
}

ardrone::acceleration ARDrone::drone_getAcceleration()
{
	if(!isConnected())
	{
		ardrone::acceleration error;
		error.ax = -1;
		error.ay = -1;
		error.az = -1;
		return error;
	}

	ardrone::acceleration a;

	_navdatamutex.lock();
	a.ax = _nd->ax;
	a.ay = _nd->ay;
	a.az = _nd->az;
	_navdatamutex.unlock();

	return a;
}

float ARDrone::drone_getBatteryStatus()
{
	if(!isConnected())
	{
		return -1;
	}

	float b;

	_navdatamutex.lock();
	b = _nd->vbatpercentage;
	_navdatamutex.unlock();

	return b;
}

bool ARDrone::drone_isFlying()
{
	if(!isConnected())
	{
		return false;
	}

	return _flying;
}

bool ARDrone::drone_isRecording()
{
	if(!isConnected())
	{
		return false;
	}

	return _isRecording;
}

// AR.Drone control functions

bool ARDrone::drone_calibmagneto()
{
	if(!isConnected())
	{
		return false;
	}

	_commandmutex.lock();
	_commandqueue.push_back(MagnetometerCalibrationCommand());
	_commandmutex.unlock();

	return true;
}

bool ARDrone::drone_changeView(int view)
{
	if(!isConnected())
	{
		return false;
	}

	_commandmutex.lock();
	_commandqueue.push_back(ConfigIDSCommand());
	_commandqueue.push_back(ZapCommand(view == ardrone::camera::FRONT? true : false));
	_commandmutex.unlock();

	_currentView = view;

	return true;
}

bool ARDrone::drone_emergency()
{
	if(!isConnected())
	{
		return false;
	}

	_commandmutex.lock();
	_commandqueue.push_back(EmergencyCommand(true));
	_commandmutex.unlock();

	_flying = false;
	_srec->addEvent("Emergency");

	return true;
}

bool ARDrone::drone_flattrim()
{
	if(!isConnected())
	{
		return false;
	}

	_commandmutex.lock();
	_commandqueue.push_back(FlatTrimCommand());
	_commandmutex.unlock();

	return true;
}

bool ARDrone::drone_flip(int direction)
{
	if(!isConnected())
	{
		return false;
	}

	_commandmutex.lock();
	_commandqueue.push_back(ConfigIDSCommand());
	_commandqueue.push_back(FlipCommand(direction));
	_commandmutex.unlock();

	return true;
}

bool ARDrone::drone_hover()
{
	if(!isConnected())
	{
		return false;
	}

	_phi = 0.0f;
	_theta = 0.0f;
	_gaz = 0.0f;
	_yaw = 0.0f;

	return true;
}

bool ARDrone::drone_land()
{
	if(!isConnected())
	{
		return false;
	}

	_commandmutex.lock();
	_commandqueue.push_back(LandCommand());
	_commandmutex.unlock();

	_flying = false;
	_srec->addEvent("Land");

	return true;
}

bool ARDrone::drone_move(float phi, float theta, float gaz, float yaw)
{
	if(!isConnected())
	{
		return false;
	}

	_phi = phi;
	_theta = theta;
	_gaz = gaz;
	_yaw = yaw;

	return true;
}

bool ARDrone::drone_setPhi(float phi)
{
	if(!isConnected())
	{
		return false;
	}

	_phi = phi;

	return true;
}

bool ARDrone::drone_setTheta(float theta)
{
	if(!isConnected())
	{
		return false;
	}

	_theta = theta;

	return true;
}

bool ARDrone::drone_setGaz(float gaz)
{
	if(!isConnected())
	{
		return false;
	}

	_gaz = gaz;

	return true;
}

bool ARDrone::drone_setYaw(float yaw)
{
	if(!isConnected())
	{
		return false;
	}

	_yaw = yaw;

	return true;
}

bool ARDrone::drone_pair(string mac_address)
{
	if(!isConnected())
	{
		return false;
	}

	_commandmutex.lock();
	_commandqueue.push_back(ConfigIDSCommand());
	_commandqueue.push_back(ConfigCommand("network:owner_mac", mac_address));
	_commandmutex.unlock();

	return true;
}

bool ARDrone::drone_pair()
{
	if(!isConnected())
	{
		return false;
	}

	//TODO: get MAC address of wifi adapter

	return true;
}

bool ARDrone::drone_setConfiguration(string field, float value)
{
	if(!isConnected())
	{
		return false;
	}

	if((field == ardrone::config::YAW_SPEED_MAX) || (field == ardrone::config::TILT_MAX))
	{
		value = value * M_PI / 180.0; // Convert degree to radian
	}
	else
	{
		if(field == ardrone::config::ALTITUDE_MAX)
		{
			value = value * 10;
		}
	}

	_commandmutex.lock();
	_commandqueue.push_back(ConfigIDSCommand());
	_commandqueue.push_back(ConfigCommand(field, to_string(value)));
	_commandmutex.unlock();

	return true;
}

bool ARDrone::drone_setConfiguration(string field, string value)
{
	if(!isConnected())
	{
		return false;
	}

	_commandmutex.lock();
	_commandqueue.push_back(ConfigIDSCommand());
	_commandqueue.push_back(ConfigCommand(field, value));
	_commandmutex.unlock();

	return true;
}

bool ARDrone::drone_startRecordingOnUSB()
{
	if(!isConnected())
	{
		return false;
	}

	_commandmutex.lock();
	_commandqueue.push_back(ConfigIDSCommand());
	_commandqueue.push_back(RecordOnUSBCommand(true));
	_commandmutex.unlock();

	return true;
}

bool ARDrone::drone_stopRecordingOnUSB()
{
	if(!isConnected())
	{
		return false;
	}

	_commandmutex.lock();
	_commandqueue.push_back(ConfigIDSCommand());
	_commandqueue.push_back(RecordOnUSBCommand(false));
	_commandmutex.unlock();

	return true;
}

bool ARDrone::drone_startRecordingNavdata()
{
	if(!isConnected())
	{
		return false;
	}

	string nddirectory = _saveDir;
	nddirectory.append("SensorData/");

	// Create the dirextory if it doesn't exist
	boost::filesystem::create_directories(nddirectory);

	string timestamp = AutoFlight::af_timestamp();

	string filename = "Sen_";
	filename.append(timestamp);
	filename.append(".csv");

	_isRecordingNavdata = _ndrecorder.startRecording(nddirectory + filename);

	if(_isRecordingNavdata && _srec != NULL)
	{
		_srec->addEvent("NavdataRecordingStart", nddirectory + filename);
	}

	return _isRecordingNavdata;
}

bool ARDrone::drone_stopRecordingNavdata()
{
	if(!isConnected())
	{
		return false;
	}

	_isRecordingNavdata = _ndrecorder.stopRecording();

	if(!_isRecordingNavdata && _srec != NULL)
	{
		_srec->addEvent("NavdataRecordingStop");
	}

	return !_isRecordingNavdata;
}

bool ARDrone::drone_toggleRecordingNavdata()
{
	if(!isConnected())
	{
		return false;
	}

	if(_isRecordingNavdata)
	{
		return drone_stopRecordingNavdata();
	}
	else
	{
		return drone_startRecordingNavdata();
	}
}

bool ARDrone::drone_isRecordingNavdata()
{
	return _isRecordingNavdata;
}

bool ARDrone::drone_takeOff()
{
	if(!isConnected())
	{
		return false;
	}

	_commandmutex.lock();
	_commandqueue.push_back(TakeOffCommand());
	_commandmutex.unlock();

	_flying = true;
	_srec->addEvent("TakeOff");

	return true;
}

bool ARDrone::drone_toggleView()
{
	if(!isConnected())
	{
		return false;
	}

	if(_currentView == ardrone::camera::BOTTOM)
	{
		drone_changeView(ardrone::camera::FRONT);
	}
	else
	{
		drone_changeView(ardrone::camera::BOTTOM);
	}

	return true;
}

bool ARDrone::drone_unpair()
{
	if(!isConnected())
	{
		return false;
	}

	drone_pair("00:00:00:00:00:00");

	return true;
}

bool ARDrone::drone_takePicture()
{
	if(!isConnected())
	{
		return false;
	}

	string picdirectory = _saveDir;
	picdirectory.append("Pictures/");

	// Create the dirextory if it doesn't exist
	boost::filesystem::create_directories(picdirectory);

	string timestamp = AutoFlight::af_timestamp();

	string filename = "Pic_";
	filename.append(timestamp);
	filename.append(".jpg");

	bool picTaken = _vm.takePicture(picdirectory + filename);

	if(picTaken && _srec != NULL)
	{
		_srec->addEvent("PictureTaken", picdirectory + filename);
	}

	return picTaken;
}

bool ARDrone::drone_startRecording()
{
	if(!isConnected())
	{
		return false;
	}

	// Tell the drone to start sending the record stream
	drone_setConfiguration(ardrone::config::VIDEO_CODEC, ardrone::config::codec::MP4_360P_H264_720P);

	string videodirectory = _saveDir;
	videodirectory.append("Videos/");

	// Create the dirextory if it doesn't exist
	boost::filesystem::create_directories(videodirectory);

	string timestamp = AutoFlight::af_timestamp();

	string filename = "Vid_";
	filename.append(timestamp);
	filename.append(".mp4");

	try
	{
		if(_vm.startRecording(videodirectory + filename))
		{
			_isRecording = true;
		}
	}
	catch(runtime_error &e)
	{
		cerr << "Error: " << e.what() << endl;
	}

	if(_isRecording && _srec != NULL)
	{
		_srec->addEvent("VideoRecordingStart", videodirectory + filename);
	}

	return true;
}

bool ARDrone::drone_stopRecording()
{
	if(!isConnected())
	{
		return false;
	}

	try
	{
		if(_vm.stopRecording())
		{
			_isRecording = false;
		}
	}
	catch(runtime_error &e)
	{
		cout << "Error: " << e.what() << endl;
	}

	if(!_isRecording && _srec != NULL)
	{
		_srec->addEvent("VideoRecordingStop");
	}

	// Tell the drone to stop sending the record stream
	drone_setConfiguration(ardrone::config::VIDEO_CODEC, ardrone::config::codec::MP4_360P);

	return true;
}

bool ARDrone::drone_toggleRecording()
{
	if(!isConnected())
	{
		return false;
	}

	if(_isRecording)
	{
		drone_stopRecording();
	}
	else
	{
		drone_startRecording();
	}

	return true;
}
