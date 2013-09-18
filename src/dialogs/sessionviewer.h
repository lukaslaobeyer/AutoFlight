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
		void timelineMarkerPressed(RecordedEvent *e);
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

		SessionReader _reader;

		void openedSession();
};

#endif
