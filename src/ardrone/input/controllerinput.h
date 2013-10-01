#ifndef CONTROLLERINPUT_H
#define CONTROLLERINPUT_H

struct ControllerInput
{
	float yaw;
	float pitch;
	float roll;
	float altitude;
	bool takeOff;
	bool land;
	bool toggleView;
	bool toggleRecording;
	bool emergency;
	bool flip;
	bool takePicture;
	bool slowMode;
};

#endif
