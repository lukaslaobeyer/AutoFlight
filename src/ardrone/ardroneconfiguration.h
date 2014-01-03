#ifndef ARDRONECONFIGURATION_H
#define ARDRONECONFIGURATION_H

struct ARDroneConfiguration
{
	float pitch_roll_max;     // Maximum pitch/roll angle in degrees. Should be between 0 and 30.
	float altitude_max;       // Maximum drone altitude in m.
	float vertical_speed_max; // Maximum vertical speed in m/s. Recommended value: 200 - 2000.
	float yaw_speed_max;      // Maximum yaw speed in deg/s. Recommended value: 40 deg/s - 350 deg/s.
	bool outdoor_flight;      // Should be set to true if flying outdoors. (Enables the AR.Drone's wind calculator.)
	bool no_hull;             // Should be set to true if flying without hull / with the outdoor hull.
};

#endif
