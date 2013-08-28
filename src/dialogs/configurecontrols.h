#ifndef CONFIGURECONTROLS_H
#define CONFIGURECONTROLS_H

#include <QDialog>
#include <QPushButton>
#include <QHBoxLayout>
#include <vector>
#include "../ardrone/input/controllerconfiguration.h"

class ConfigureControls : public QDialog
{
	Q_OBJECT

	public:
		explicit ConfigureControls(unsigned int deviceID, ControllerConfiguration *config = NULL, QWidget *parent = 0);
		ControllerConfiguration *getControllerConfiguration();
	private Q_SLOTS:
		void setButton();
		void setAxis();
		void switchControlMode(int row = -1);
	private:
		ControllerConfiguration *cc;
		std::vector<QPushButton *> buttons;
		std::vector<QPushButton *> axisButtons; // height+; height-; yaw-; yaw+; pitch-; pitch+; roll-; roll+
		std::vector<QHBoxLayout *> row;
		std::vector<QPushButton *> modeSwitch;
		unsigned int _deviceID = 0;
};

#endif
