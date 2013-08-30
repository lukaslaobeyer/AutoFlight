#include "navdatarecorder.h"
#include <iostream>

using namespace std;

NavdataRecorder::NavdataRecorder()
{

}

NavdataRecorder::~NavdataRecorder()
{
	stopRecording();
}

bool NavdataRecorder::startRecording(string file)
{
	if(!_record)
	{
		try
		{
			recordFile.open(file, ios::out);
		}
		catch(ofstream::failure &ex)
		{
			cout << "Could not open file " << file << " :" << ex.what() << endl;
			return false;
		}

		if(recordFile.is_open())
		{
			_headerWritten = false;
			_record = true;
		}

		return _record;
	}
	else
	{
		return true;
	}
}

bool NavdataRecorder::stopRecording()
{
	if(_record && recordFile.is_open())
	{
		_record = false;

		recordFile.close();
		recordFile.clear();

		return !_record;
	}
	else
	{
		return false;
	}
}

void NavdataRecorder::navdataAvailable(AFNavdata *nd)
{
	if(_record && recordFile.is_open())
	{
		if(!_headerWritten)
		{
			recordFile << "Timestamp; Battery Charge Level; WiFi Link Quality; Altitude; Yaw; Pitch; Roll; Vx; Vy; Vz; Ax; Ay; Az" << endl;
			_headerWritten = true;
		}

		const boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
		string timestamp = to_iso_timestamp(now);

		recordFile << timestamp << "; " << nd->vbatpercentage << "; " << nd->wifipercentage << "; " << nd->altitude << "; "
				   << nd->psi << "; " << nd->theta << "; " << nd->phi << "; " << nd->vx << "; " << nd->vy << "; " << nd->vz
				   << "; " << nd->ax << "; " << nd->ay << "; " << nd->az << endl;
	}
}

string NavdataRecorder::to_iso_timestamp(boost::posix_time::ptime time)
{
	stringstream timestamp;
	timestamp << setw(4) << setfill('0') << time.date().year() << setw(2) << time.date().month() << setw(2) << time.date().day();
	timestamp << "T";
	timestamp << setw(2) << time.time_of_day().hours() << setw(2) << time.time_of_day().minutes() << setw(2) << time.time_of_day().seconds();

	return timestamp.str();
}
