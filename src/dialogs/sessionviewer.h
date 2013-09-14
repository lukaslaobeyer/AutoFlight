#ifndef SESSIONVIEWER_H
#define SESSIONVIEWER_H

#include <QMainWindow>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>

class SessionViewer : public QMainWindow
{
	Q_OBJECT

	public:
		explicit SessionViewer(QWidget *parent = 0);

	private Q_SLOTS:
		void monthSelectionChanged(QListWidgetItem *selected, QListWidgetItem *previous);
		void daySelectionChanged(QListWidgetItem *selected, QListWidgetItem *previous);
		void timeSelectionChanged(QListWidgetItem *selected, QListWidgetItem *previous);
	private:
		QListWidget *monthchooser;
		QListWidget *daychooser;
		QListWidget *timechooser;
		QPushButton *ok;
};

#endif
