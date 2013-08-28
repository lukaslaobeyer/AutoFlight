#ifndef ARDRONE_H
#define ARDRONE_H

#include <string>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <stdexcept>
#include <opencv2/opencv.hpp>
#include "../afconstants.h"
#include "controllink.h"
#include "navdata/navdatamanager.h"
#include "navdata/inavdatalistener.h"
#include "video/videomanager.h"
#include "video/ivideolistener.h"
#include "atcommands/atcommand.h"
#include "input/controllerconfiguration.h"

class NotConnectedException : public std::runtime_error
{
	public:
		NotConnectedException() : std::runtime_error("Not connected to AR.Drone!") { }
};


class ARDrone
{		
	public:
		explicit ARDrone(std::string ip);
		ARDrone();
		~ARDrone();
		
		void setIP(std::string ip);

		int connect(); // Returns a code defined in afconstants.h
		bool isConnected();
		void addNavdataListener(INavdataListener *listener);
		void removeNavdataListener(INavdataListener *listener);
		void addVideoListener(IVideoListener *listener);
		void removeVideoListener(IVideoListener *listener);
		void startUpdateLoop();
		void stopUpdateLoop();
		void setControllerConfiguration(ControllerConfiguration *config);

		ardrone::orientation drone_getOrientation();
		ardrone::linearvelocity drone_getLinearVelocity();
		float drone_getAltitude();
		ardrone::position drone_getPosition();
		ardrone::acceleration drone_getAcceleration();
		float drone_getBatteryStatus();
		bool drone_isRecordingOnUSB();
		bool drone_isRecording();
		bool drone_isFlying();

		bool drone_takePicture();
		bool drone_startRecording();
		bool drone_stopRecording();
		bool drone_toggleRecording();

		bool drone_move(float phi, float theta, float gaz, float yaw);
		bool drone_setPhi(float phi);
		bool drone_setTheta(float theta);
		bool drone_setGaz(float gaz);
		bool drone_setYaw(float yaw);
		bool drone_hover();
		bool drone_takeOff();
		bool drone_land();
		bool drone_emergency();
		bool drone_flattrim();
		bool drone_calibmagneto();
		bool drone_flip(int direction); // Direction defined in afconstants.h
		bool drone_pair();
		bool drone_pair(std::string mac_address);
		bool drone_unpair();
		bool drone_startRecordingOnUSB();
		bool drone_stopRecordingOnUSB();
		bool drone_toggleRecordingOnUSB();
		bool drone_changeView(int view); // View defined in afconstants.h
		bool drone_toggleView();
		bool drone_setConfiguration(std::string field, float value);       // Field defined in afconstants.h
		bool drone_setConfiguration(std::string field, std::string value);
	private:
		void runUpdateLoop();
		void processControllerInput();
		std::string to_iso_timestamp(boost::posix_time::ptime);

		std::string _ip;
		boost::asio::io_service _io_service;
		ControlLink _cl;
		NavdataManager _nm;
		VideoManager _vm;
		std::vector<INavdataListener *> _ndlisteners;
		std::vector<IVideoListener *> _vlisteners;
		ControllerConfiguration *_controllerconfig = NULL;
		int _currentView;
		bool _isRecording = false;
		bool _stop_flag = false;
		bool _connected = false;
		AFNavdata *_nd = NULL;
		bool _flying = false;
		cv::Mat _frame;
		boost::thread *_updater = NULL;
		boost::mutex _navdatamutex;
		boost::mutex _commandmutex;
		std::vector<ATCommand> _commandqueue;
		bool _configCmdInQueue = false;

		float _phi = 0.0f, _theta = 0.0f, _gaz = 0.0f, _yaw = 0.0f;
};

#endif
