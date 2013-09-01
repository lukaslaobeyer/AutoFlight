#include "navdatarecorder.h"
#include "../../autoflight.h"
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

		string timestamp;
		const boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
		timestamp = boost::posix_time::to_iso_extended_string(now);

		recordFile << timestamp << "; " << nd->vbatpercentage << "; " << nd->wifipercentage << "; " << nd->altitude << "; "
				   << nd->psi << "; " << nd->theta << "; " << nd->phi << "; " << nd->vx << "; " << nd->vy << "; " << nd->vz
				   << "; " << nd->ax << "; " << nd->ay << "; " << nd->az << endl;
	}
}
