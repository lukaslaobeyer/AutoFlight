#ifndef AFCONSTANTS_H
#define AFCONSTANTS_H

#include <string>

namespace autoflight
{
	const std::string SOFTWARE_VERSION = "Alpha 0.2.0";
	const std::string BUILD_NUMBER     = "a428004";
}

namespace ardrone
{
	const std::string DEFAULT_IP   = "192.168.1.1";
	const int NAVDATA_PORT         = 5554;
	const int VIDEO_PORT           = 5555;
	const int VIDEO_RECORDING_PORT = 5553;
	const int CONTROL_PORT         = 5556;

	namespace navdata_keys
	{
		const int NAVDATA_DEMO_TAG = 0;
		const int NAVDATA_TIME_TAG = 1;
		const int NAVDATA_RAW_MEASURES_TAG = 2;
		const int NAVDATA_PHYS_MEASURES_TAG = 3;
		const int NAVDATA_GYROS_OFFSETS_TAG = 4;
		const int NAVDATA_EULER_ANGLES_TAG = 5;
		const int NAVDATA_REFERENCES_TAG = 6;
		const int NAVDATA_TRIMS_TAG = 7;
		const int NAVDATA_RC_REFERENCES_TAG = 8;
		const int NAVDATA_PWM_TAG = 9;
		const int NAVDATA_ALTITUDE_TAG = 10;
		const int NAVDATA_VISION_RAW_TAG = 11;
		const int NAVDATA_MAGNETO_TAG = 22;
		const int NAVDATA_WIFI_TAG = 26;
		const int NAVDATA_CKS_TAG = 0xFFFF;
	}

	namespace video
	{
		namespace frame_type
		{
			const int UNKNOWN = 0;
			const int IDR = 1;
			const int I = 2;
			const int P = 3;
			const int HEADERS = 4;
		}

		namespace codec
		{
			const int UNKNOWN = 0;
			const int VLIB = 1;
			const int P264 = 2;
			const int MPEG4_VISUAL = 3;
			const int MPEG4_AVC = 4;
		}
	}

	namespace app_id
	{
		const std::string SESSION_ID     = "af2af2af";
		const std::string PROFILE_ID     = "af1af1af";
		const std::string APPLICATION_ID = "af0af0af";
	}

	namespace connection
	{
		const int CONNECTION_ESTABLISHED = 1000;
		const int CONNECTION_FAILED      = -1000;
		const int ALREADY_CONNECTED      = -500;
		const int EXCEPTION_OCCURRED     = -1500;
	}

	namespace flip
	{
		const int LEFT   = 18;
		const int RIGHT  = 19;
		const int AHEAD  = 16;
		const int BEHIND = 17;
	}

	namespace camera
	{
		const int FRONT  = 1;
		const int BOTTOM = 0;
	}

	namespace config
	{
		const std::string ALTITUDE_MAX         = "control:altitude_max";
		const std::string I_ALTITUDE_MAX       = "control:indoor_altitude_max";
		const std::string O_ALTITUDE_MAX       = "control:outdoor_altitude_max";
		const std::string OUTDOOR_SHELL        = "control:flight_without_shell";
		const std::string OUTDOOR_FLIGHT       = "control:outdoor";
		const std::string TILT_MAX             = "control:euler_angle_max";
		const std::string I_TILT_MAX           = "control:indoor_euler_angle_max";
		const std::string O_TILT_MAX           = "control:outdoor_euler_angle_max";
		const std::string VERTICAL_SPEED_MAX   = "control:control_vz_max";
		const std::string I_VERTICAL_SPEED_MAX = "control:indoor_control_vz_max";
		const std::string O_VERTICAL_SPEED_MAX = "control:outdoor_control_vz_max";
		const std::string YAW_SPEED_MAX        = "control:control_yaw";
		const std::string I_YAW_SPEED_MAX      = "control:indoor_control_yaw";
		const std::string O_YAW_SPEED_MAX      = "control:outdoor_control_yaw";

		const std::string VIDEO_CODEC          = "video:video_codec";
		const std::string MAX_BITRATE		   = "video:max_bitrate";

		namespace codec
		{
			const int MP4_360P = 0x80;
			const int H264_360P = 0x81;
			const int MP4_360P_H264_720P = 0x82;
			const int H264_720P = 0x83;
			const int MP4_360P_SLRS = 0x84;
			const int H264_360P_SLRS = 0x85;
			const int H264_720P_SLRS = 0x86;
			const int H264_AUTO_RESIZE = 0x87;
			const int MP4_360P_H264_360P = 0x88;
		}
	}

	typedef struct
	{
		float yaw;
		float pitch;
		float roll;
	} orientation;

	typedef struct
	{
		float vx;
		float vy;
		float vz;
	} linearvelocity;

	typedef struct
	{
		float x;
		float y;
	} position;

	typedef struct
	{
		float ax;
		float ay;
		float az;
	} acceleration;


}

namespace error
{
	const int CONNECTION_FAILED   = -2;
	const int CONNECTION_LOST     = -3;
	const int NAVDATA_PARSE_ERROR = -4;
}

#endif
