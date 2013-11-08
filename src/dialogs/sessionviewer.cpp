#include "sessionviewer.h"
#include "../autoflight.h"
#include <QtWidgets>

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <locale>
#include <boost/algorithm/string.hpp>
#include <glm/glm.hpp>

using namespace std;

SessionViewer::SessionViewer(QWidget *parent) : QMainWindow(parent)
{
	setWindowTitle(tr("Session Viewer"));

	tabs = new QTabWidget();

	QWidget *chooserw = new QWidget();
	QVBoxLayout *layout = new QVBoxLayout();
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);
	chooserw->setLayout(layout);

	tabs->addTab(chooserw, tr("Session Chooser"));

	setCentralWidget(tabs);

	QLabel *l = new QLabel(tr("Choose the session to open"));
	l->setAlignment(Qt::AlignCenter);
	l->setStyleSheet("font-size: 25px; padding: 10px;");
	layout->addWidget(l);

	// Get the time data of the saved files
	vector<string> sessions = SessionReader::getSessionSaves();
	sort(sessions.begin(), sessions.end());

	vector<string> months;

	for(string filename : sessions)
	{
		if(filename.length() == 22)
		{
			string year = filename.substr(3, 4);
			string month = filename.substr(7, 2) + "/" + year;

			if(months.empty())
			{
				months.push_back(month);
			}
			else if(months.back() != month)
			{
				months.push_back(month);
			}
		}
	}

	// Session chooser labels
	QWidget *sessionchooserlabels = new QWidget();
	QHBoxLayout *scllayout = new QHBoxLayout();
	scllayout->setSpacing(0);
	scllayout->setContentsMargins(0, 0, 0, 0);
	sessionchooserlabels->setLayout(scllayout);
	layout->addWidget(sessionchooserlabels);

	QLabel *monthL = new QLabel(tr("Month"));
	QLabel *dayL = new QLabel(tr("Day"));
	QLabel *timeL = new QLabel(tr("Time"));

	monthL->setAlignment(Qt::AlignCenter);
	dayL->setAlignment(Qt::AlignCenter);
	timeL->setAlignment(Qt::AlignCenter);

	scllayout->addWidget(monthL);
	scllayout->addWidget(dayL);
	scllayout->addWidget(timeL);

	// Session chooser
	QWidget *sessionchooser = new QWidget();
	QHBoxLayout *sclayout = new QHBoxLayout();
	sclayout->setSpacing(0);
	sclayout->setContentsMargins(0, 0, 0, 0);
	sessionchooser->setLayout(sclayout);
	layout->addWidget(sessionchooser);

	monthchooser = new QListWidget();
	for(string month : months)
	{
		monthchooser->addItem(QString::fromStdString(month));
	}
	sclayout->addWidget(monthchooser);

	daychooser = new QListWidget();
	sclayout->addWidget(daychooser);

	timechooser = new QListWidget();
	sclayout->addWidget(timechooser);

	ok = new QPushButton(tr("Open Session"));
	ok->setEnabled(false);
	ok->setStyleSheet("font-size: 16px;");
	layout->addWidget(ok);


	QObject::connect(monthchooser, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this, SLOT(monthSelectionChanged(QListWidgetItem *, QListWidgetItem *)));
	if(monthchooser->count() != 0)
	{
		monthchooser->setCurrentRow(monthchooser->count() - 1);
	}

	QObject::connect(daychooser, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this, SLOT(daySelectionChanged(QListWidgetItem *, QListWidgetItem *)));
	QObject::connect(timechooser, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this, SLOT(timeSelectionChanged(QListWidgetItem *, QListWidgetItem *)));

	QObject::connect(ok, SIGNAL(clicked()), this, SLOT(loadSelectedSession()));

	///////////////////////////
    // Actual session viewer //
	///////////////////////////

	QWidget *viewer = new QWidget();
	QVBoxLayout *viewl = new QVBoxLayout();
	viewl->setSpacing(0);
	viewl->setContentsMargins(0, 0, 0, 0);
	viewer->setLayout(viewl);

	tabs->addTab(viewer, tr("Session Viewer"));

	noSessionOpen = new QLabel(tr("No session open"));
	noSessionOpen->setStyleSheet("padding: 20px; font-size: 28px;");
	noSessionOpen->setAlignment(Qt::AlignCenter);
	viewl->addWidget(noSessionOpen);

	sessionTitle = new QLabel();
	sessionTitle->setStyleSheet("padding: 10px; font-size: 26px;");
	sessionTitle->setAlignment(Qt::AlignCenter);
	sessionTitle->setMaximumHeight(50);
	viewl->addWidget(sessionTitle);
	sessionTitle->setVisible(false);

	sessionInfo = new QLabel();
	sessionInfo->setStyleSheet("padding: 5px; font-size: 18px");
	sessionInfo->setAlignment(Qt::AlignCenter);
	sessionInfo->setMaximumHeight(30);
	viewl->addWidget(sessionInfo);
	sessionInfo->setVisible(false);

	pics = new GalleryWidget(0, this);
	viewl->addWidget(pics);
	pics->setVisible(false);

	map = new Map3D();
	viewl->addWidget(map);
	map->hide();
	//sensorviewer = new SensorDataViewer();
	//viewl->addWidget(sensorviewer);

	timeline = new TimelineWidget(0);
	timeline->setMaximumHeight(100);
	timeline->setMinimumHeight(100);
	viewl->addWidget(timeline);
	timeline->setVisible(false);

	QObject::connect(timeline, SIGNAL(markerPressed(std::string, std::string, float)), this, SLOT(timelineMarkerPressed(std::string, std::string, float)));
	QObject::connect(timeline, SIGNAL(newTimeSelected(float)), this, SLOT(timelineTimeUpdated(float)));
}

void SessionViewer::monthSelectionChanged(QListWidgetItem *selected, QListWidgetItem *previous)
{
	if(selected == nullptr)
	{
		return;
	}

	vector<string> sessions = SessionReader::getSessionSaves();

	string selectedmonth = selected->text().toStdString().substr(3, 4) + selected->text().toStdString().substr(0, 2);

	vector<int> days;

	for(string session : sessions)
	{
		string month = session.substr(3, 6);

		if(month == selectedmonth)
		{
			int day = boost::lexical_cast<int>(session.substr(9, 2));
			days.push_back(day);
		}
	}

	sort(days.begin(), days.end());

	daychooser->clear();
	timechooser->clear();
	ok->setEnabled(false);

	int previousday = -1;

	for(int day : days)
	{
		if(day != previousday)
		{
			daychooser->addItem(QString::number(day));
		}

		previousday = day;
	}
}

void SessionViewer::daySelectionChanged(QListWidgetItem *selected, QListWidgetItem *previous)
{
	if(selected == nullptr)
	{
		return;
	}

	vector<string> sessions = SessionReader::getSessionSaves();

	int selectedday = boost::lexical_cast<int>(selected->text().toStdString());

	vector<string> hours;

	for(string session : sessions)
	{
		int day = boost::lexical_cast<int>(session.substr(9, 2));

		if(day == selectedday)
		{
			hours.push_back(session.substr(12, 6));
		}
	}

	sort(hours.begin(), hours.end());

	timechooser->clear();
	ok->setEnabled(false);

	for(string hour : hours)
	{
		string formatted = hour.substr(0, 2) + ":" + hour.substr(2, 2) + ":" + hour.substr(4, 2);
		timechooser->addItem(QString::fromStdString(formatted));
	}
}

void SessionViewer::timeSelectionChanged(QListWidgetItem *selected, QListWidgetItem *previous)
{
	if(selected == nullptr)
	{
		return;
	}

	ok->setEnabled(true);
}

void SessionViewer::loadSelectedSession()
{
	string filename = "AF_";
	// Append the year and month
	filename.append(monthchooser->currentItem()->text().toStdString().substr(3, 4) + monthchooser->currentItem()->text().toStdString().substr(0, 2));
	// Append the day
	int day = boost::lexical_cast<int>(daychooser->currentItem()->text().toStdString());
	if(day <= 9)
	{
		filename.append("0" + to_string(day));
	}
	else
	{
		filename.append("" + to_string(day));
	}
	// Append the time
	filename.append("T");
	string time = timechooser->currentItem()->text().toStdString();
	time.erase(remove_if(time.begin(), time.end(), not1(ptr_fun(::isdigit))), time.end());
	filename.append(time);
	// Append the extension
	filename.append(".xml");

	// Open the session
	if(_reader.readSession(AutoFlight::getHomeDirectory() + "AutoFlightSaves/Sessions/" + filename))
	{
		cout << "Opened session " << filename << endl;
		vector<RecordedEvent> events = _reader.getEvents();

		timeline->removeEvents();
		timeline->setTime((float) events.back().getTimeFromStart() / 1000.0f);

		for(RecordedEvent event : events)
		{
			timeline->addEvent(event);
		}

		calcPositions(events);

		openedSession();
	}
	else
	{
		cerr << "Could not open session " << filename << endl;
	}
}

void SessionViewer::openedSession()
{
	noSessionOpen->setVisible(false);

	timeline->setVisible(true);
	sessionTitle->setText(QString(daychooser->currentItem()->text()).append("/").append(monthchooser->currentItem()->text()).append(" ").append(timechooser->currentItem()->text()));
	sessionTitle->setVisible(true);
	sessionInfo->setText(tr("Session duration: ").append(QString::number(_reader.getSessionDuration(), 'f', 1)).append(tr(" | Total flight time: ")).append(QString::number(_reader.getFlightTime(), 'f', 1)).append(tr(" | Pictures taken: ")).append(QString::number(_reader.getPicturesCount(), 'f', 0)));
	sessionInfo->setVisible(true);
	pics->setImages(_reader.getPicturePaths());
	pics->setVisible(true);

	map->show();

	tabs->setCurrentIndex(1);
}

void SessionViewer::timelineMarkerPressed(string type, string content, float time)
{
	if(type == "PictureTaken")
	{
		pics->showImage(content);
	}
}

void SessionViewer::calcPositions(vector<RecordedEvent> &events)
{
	// Calculate the position of the drone if data is available

	string file = "";

	for(RecordedEvent e : events)
	{
		if(e.getType() == "NavdataRecordingStart")
		{
			file = e.getContent();
			_path_starttime = e.getTimeFromStart();
			break;
		}
	}

	if(file.length() <= 0)
	{
		return;
	}
	else
	{
		try
		{
			stringstream buffer;
			ifstream ifs(file);
			buffer << ifs.rdbuf();
			string data = buffer.str();

			vector<string> lines;
			boost::split(lines, data, boost::is_any_of("\n"), boost::token_compress_on);

			lines.erase(lines.begin());
			lines.erase(lines.end());

			float x = 0;
			float y = 0;
			float alt = 0;
			long long previousT = 0;
			long long tFromStart = 0;

			for(string line : lines)
			{
				vector<string> navdata;
				boost::split(navdata, line, boost::is_any_of(";"), boost::token_compress_on);

				boost::posix_time::ptime p_time = boost::date_time::parse_delimited_time<boost::posix_time::ptime>(navdata[0], 'T');
				static boost::posix_time::ptime epoch = boost::date_time::parse_delimited_time<boost::posix_time::ptime>("1970-01-01T00:00:00.000", 'T');

				boost::posix_time::time_duration diff = p_time - epoch;
				long long t = diff.total_milliseconds();

				if(previousT != 0)
				{
					navdata[7].erase(navdata[7].begin());
					float vx = boost::lexical_cast<float>(navdata[7]);
					navdata[8].erase(navdata[8].begin());
					float vy = boost::lexical_cast<float>(navdata[8]);
					navdata[3].erase(navdata[3].begin());
					alt = boost::lexical_cast<float>(navdata[3]);
					navdata[4].erase(navdata[4].begin());
					float yaw = boost::lexical_cast<float>(navdata[4]);
					navdata[5].erase(navdata[5].begin());
					float pitch = boost::lexical_cast<float>(navdata[5]);
					navdata[6].erase(navdata[6].begin());
					float roll = boost::lexical_cast<float>(navdata[6]);

					long long dT = t - previousT;

					tFromStart += dT;

					// Calculate position using speed and direction data
					float translateX = vx * 0.001 * dT;
					float translateY = vy * 0.001 * dT;
					x = x + cos(M_PI/180 * (yaw-90)) * translateY + cos(M_PI/180 * (90-(yaw-90))) * translateX;
					y = y + sin(M_PI/180 * (yaw-90)) * translateY + sin(M_PI/180 * (90-(yaw-90))) * translateX;

					if(path.size() >= 1)
					{
						if((abs(x - path[path.size() - 1]->x) >= 0.05f) || (abs(y - path[path.size() - 1]->y) >= 0.05f))
						{
							path.push_back(new glm::vec3(-x, -y, alt));
						}
					}
					else
					{
						path.push_back(new glm::vec3(-x, -y, alt));
					}

					float *attitude = new float[7];
					attitude[0] = tFromStart;
					attitude[1] = x;
					attitude[2] = y;
					attitude[3] = alt;
					attitude[4] = yaw;
					attitude[5] = pitch;
					attitude[6] = roll;
					_drone_attitude.push_back(attitude);
				}

				previousT = t;
			}

			map->setDronePath(&path);
			_mapAvailable = true;
		}
		catch(exception &e)
		{
			cerr << "Could not read navdata file: " << e.what() << endl;
			return;
		}
	}
}

int SessionViewer::getIndexFromTime(long long time)
{
	long long t = (time - _path_starttime);

	if(t < _drone_attitude[0][0] || t > _drone_attitude[_drone_attitude.size()-1][0])
	{
		return -1;
	}

	unsigned int i;

	for(i = 0; i < _drone_attitude.size() - 1; i++)
	{
		if(_drone_attitude[i][0] <= t && _drone_attitude[i+1][0] >= t)
		{
			// Found index for time
			break;
		}
	}

	return i;
}

void SessionViewer::timelineTimeUpdated(float newTime)
{
	if(_mapAvailable)
	{
		long long time_in_ms = newTime * 1000.0f;
		int i = getIndexFromTime(time_in_ms);

		if(i < 0)
		{

		}
		else
		{
			map->setDronePosition(_drone_attitude[i][1], _drone_attitude[i][2]);
			map->setDroneAttitude(-_drone_attitude[i][4], 0, 0, _drone_attitude[i][3]);
			//-, -_drone_attitude[i][5], -_drone_attitude[i][6]
		}
	}
}
