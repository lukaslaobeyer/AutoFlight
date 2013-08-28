#ifndef SELECTCONTROLLER_H
#define SELECTCONTROLLER_H

#include <vector>
#include <QDialog>
#include <QListWidget>
#include <Gamepad.h>

class SelectController : public QDialog
{
	Q_OBJECT

	public:
		explicit SelectController(QWidget *parent = 0);
		int getSelectedDeviceID();
	public Q_SLOTS:
		void selectedDeviceChanged();
	private:
		QListWidget *controllerListWidget = NULL;
		std::vector<Gamepad_device *> controllerList;
		int selectedDeviceID = -1;
};

#endif
