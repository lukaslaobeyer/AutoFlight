#ifndef SESSIONVIEWER_H
#define SESSIONVIEWER_H

#include <QMainWindow>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QTabWidget>
#include <QLabel>
#include "../tools/sessionreader.h"
#include "../widgets/timelinewidget.h"
#include "../widgets/gallerywidget.h"
#include "../widgets/sensordataviewer.h"
#include "../widgets/map3d.h"
#include <glm/glm.hpp>

class SessionViewer : public QMainWindow
{
	Q_OBJECT

	public:
		explicit SessionViewer(QWidget *parent = 0);

	private Q_SLOTS:
		void monthSelectionChanged(QListWidgetItem *selected, QListWidgetItem *previous);
		void daySelectionChanged(QListWidgetItem *selected, QListWidgetItem *previous);
		void timeSelectionChanged(QListWidgetItem *selected, QListWidgetItem *previous);

		void loadSelectedSession();

		void timelineTimeUpdated(float newTime);
		void timelineMarkerPressed(std::string type, std::string content, float time);
	private:
		QTabWidget *tabs;
		QListWidget *monthchooser;
		QListWidget *daychooser;
		QListWidget *timechooser;
		QPushButton *ok;

		QLabel *noSessionOpen;
		QLabel *sessionTitle;
		QLabel *sessionInfo;
		TimelineWidget *timeline;

		GalleryWidget *pics;
		SensorDataViewer *sensorviewer;
		Map3D *map;

		SessionReader _reader;

		std::vector<glm::vec3 *> path;
		bool _mapAvailable = false;
		int _path_starttime;
		std::vector<float *> _drone_attitude;

		void openedSession();
		int getIndexFromTime(long long time);
		void calcPositions(std::vector<RecordedEvent> &e);
};

#endif
