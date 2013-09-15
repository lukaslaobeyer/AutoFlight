#ifndef SESSIONVIEWER_H
#define SESSIONVIEWER_H

#include <QMainWindow>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QTabWidget>
#include <QLabel>
#include "../widgets/timelinewidget.h"

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
	private:
		QTabWidget *tabs;
		QListWidget *monthchooser;
		QListWidget *daychooser;
		QListWidget *timechooser;
		QPushButton *ok;

		QLabel *noSessionOpen;
		QLabel *sessionTitle;
		TimelineWidget *timeline;

		void openedSession();
};

#endif
