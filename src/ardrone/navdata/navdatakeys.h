#ifndef NAVDATAKEYS_H
#define NAVDATAKEYS_H

#include <cstdint>

struct AFNavdata
{
	uint32_t n; // The sequence number
	
	uint32_t ctrlstate; // AR.Drone control state
	
	int vbatpercentage; // Battery percentage
	int wifipercentage; // WiFi connection quality
	
	float altitude; // Altitude in cm
	
	float theta; // Pitch in degrees
	float phi; // Roll in degrees
	float psi; // Yaw in degrees
	
	float vx; // Linear velocity in m/s, x axis
	float vy; // Linear velocity in m/s, y axis
	float vz; // Linear velocity in m/s, z axis
	
	float ax; // Acceleration, x axis
	float ay; // Acceleration, y axis
	float az; // Acceleration, z axis
	
	float mx; // Magnetometer, x axis
	float my; // Magnetometer, y axis
	float mz; // Magnetometer, z axis
	
	float p; // Pressure
};

#endif
