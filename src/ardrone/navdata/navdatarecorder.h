#ifndef NAVDATARECORDER_H
#define NAVDATARECORDER_H

#include "inavdatalistener.h"
#include "navdatakeys.h"
#include <string>
#include <fstream>
#include <boost/date_time.hpp>

class NavdataRecorder : public INavdataListener
{
	public:
		NavdataRecorder();
		~NavdataRecorder();

		bool startRecording(std::string file);
		bool stopRecording();

		void navdataAvailable(AFNavdata *nd);

	private:
		std::ofstream recordFile;
		bool _record = false;
		bool _headerWritten = false;

		std::string to_iso_timestamp(boost::posix_time::ptime time);
};

#endif
