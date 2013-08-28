#ifndef CONTROLLERCONFIGURATION_H
#define CONTROLLERCONFIGURATION_H

struct ControllerConfiguration
{
	// Used to identify the device after application restart
	int vendorID;
	int productID;

	int deviceID;

	// Index for every control
	// Set to -1 for unset

	int height;
	int yaw;
	int pitch;
	int roll;

	int heightP, heightM; // P for plus / M for minus
	int yawP, yawM;		  // This variables are set when buttons are used instead of an axis
	int pitchP, pitchM;   //
	int rollP, rollM;     //

	int takeoff;
	int land;
	int emergency;
	int recording;
	int photo;
	int flip;
	int zap;
	int slow;
};

inline int *ControllerConfiguration_getControlsArray(ControllerConfiguration *c)
{
	int *array = new int[20];

	array[0] = c->height;
	array[1] = c->yaw;
	array[2] = c->pitch;
	array[3] = c->roll;
	array[4] = c->takeoff;
	array[5] = c->land;
	array[6] = c->emergency;
	array[7] = c->recording;
	array[8] = c->photo;
	array[9] = c->flip;
	array[10] = c->zap;
	array[11] = c->slow;

	array[12] = c->heightP;
	array[13] = c->heightM;
	array[14] = c->yawP;
	array[15] = c->yawM;
	array[16] = c->pitchP;
	array[17] = c->pitchM;
	array[18] = c->rollP;
	array[19] = c->rollM;

	return array;
}

#endif
