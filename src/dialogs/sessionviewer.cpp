#include "sessionviewer.h"
#include "../tools/sessionreader.h"
#include <QtWidgets>

#include <vector>
#include <string>
#include <iostream>

using namespace std;

SessionViewer::SessionViewer(QWidget *parent) : QMainWindow(parent)
{
	setWindowTitle(tr("Session Viewer"));

	QTabWidget *tabs = new QTabWidget();

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

	///////////////////////////
    // Actual session viewer //
	///////////////////////////
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
