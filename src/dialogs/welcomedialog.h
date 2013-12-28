#ifndef WELCOMEDIALOG_H
#define WELCOMEDIALOG_H

#include <QWidget>
#include <QDialog>

class WelcomeDialog : public QDialog
{
	Q_OBJECT

	public:
		explicit WelcomeDialog(QWidget *parent = 0);
	public Q_SLOTS:
		void openGettingStartedPage();
};

#endif
