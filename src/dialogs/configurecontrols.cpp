#include "configurecontrols.h"
#include <QtWidgets>
#include <Gamepad.h>
#include <boost/thread.hpp>

using namespace std;

ConfigureControls::ConfigureControls(unsigned int deviceID, ControllerConfiguration *config, QWidget *parent) : QDialog(parent)
{
	int *configArray = NULL;

	// The device ID passed to this constructor is not the same as the device index expected by the Gamepad_deviceAtIndex function,
	// so change the deviceID variable to the correct index

	for(unsigned int i = 0; i < Gamepad_numDevices(); i++)
	{
		if(Gamepad_deviceAtIndex(i) != NULL)
		{
			if(Gamepad_deviceAtIndex(i)->deviceID == deviceID)
			{
				_deviceID = i;
				break;
			}
		}
	}

	if(config == NULL)
	{
		cc = new ControllerConfiguration;

		cc->productID = Gamepad_deviceAtIndex(_deviceID)->productID;
		cc->vendorID = Gamepad_deviceAtIndex(_deviceID)->vendorID;
		cc->deviceID = deviceID;
		cc->emergency = -1;
		cc->flip = -1;
		cc->height = -1;
		cc->land = -1;
		cc->photo = -1;
		cc->pitch = -1;
		cc->recording = -1;
		cc->roll = -1;
		cc->slow = -1;
		cc->takeoff = -1;
		cc->yaw = -1;
		cc->zap = -1;
	}
	else
	{
		cc = config;
		configArray = ControllerConfiguration_getControlsArray(config);
	}

	setWindowTitle(tr("Configure Controls"));

	QVBoxLayout *layout = new QVBoxLayout();

	setLayout(layout);

	vector<QString> labels = {
			tr("Height Control"),
			tr("Yaw"),
			tr("Pitch"),
			tr("Roll"),
			tr("Takeoff"),
			tr("Land"),
			tr("Emergency"),
			tr("Toggle Recording"),
			tr("Take Picture"),
			tr("Flip"),
			tr("Toggle Camera (Front/Bottom)"),
			tr("Slow Mode")
	};

	for(unsigned int i = 0; i < labels.size(); i++)
	{
		bool showModeSwitch = false; // Axis / Buttons

		row.push_back(new QHBoxLayout());

		QLabel *l = new QLabel(labels[i]);
		row[i]->addWidget(l);

		row[i]->addStretch();
		row[i]->addSpacing(75);

		buttons.push_back(new QPushButton(tr("Click to set")));
		buttons[i]->setCheckable(true);
		buttons[i]->setChecked(false);
		if(i < 4)
		{
			connect(buttons[i], SIGNAL(clicked()), this, SLOT(setAxis()));
		}
		else
		{
			connect(buttons[i], SIGNAL(clicked()), this, SLOT(setButton()));
		}
		row[i]->addWidget(buttons[i]);

		switch(i)
		{
		case 0: // Height control
			showModeSwitch = true; // Axis / Buttons
			axisButtons.push_back(new QPushButton(tr("Click to set up")));
			axisButtons.push_back(new QPushButton(tr("Click to set down")));
			connect(axisButtons[i*2], SIGNAL(clicked()), this, SLOT(setButton()));
			connect(axisButtons[i*2+1], SIGNAL(clicked()), this, SLOT(setButton()));
			axisButtons[i*2]->setCheckable(true);
			axisButtons[i*2+1]->setCheckable(true);
			row[i]->addWidget(axisButtons[i*2]);
			row[i]->addWidget(axisButtons[i*2+1]);
			axisButtons[i*2]->hide();
			axisButtons[i*2+1]->hide();
			break;
		case 1: // Yaw control
			showModeSwitch = true; // Axis / Buttons
			axisButtons.push_back(new QPushButton(tr("Click to set left")));
			axisButtons.push_back(new QPushButton(tr("Click to set right")));
			connect(axisButtons[i*2], SIGNAL(clicked()), this, SLOT(setButton()));
			connect(axisButtons[i*2+1], SIGNAL(clicked()), this, SLOT(setButton()));
			axisButtons[i*2]->setCheckable(true);
			axisButtons[i*2+1]->setCheckable(true);
			row[i]->addWidget(axisButtons[i*2]);
			row[i]->addWidget(axisButtons[i*2+1]);
			axisButtons[i*2]->hide();
			axisButtons[i*2+1]->hide();
			break;
		case 2: // Pitch control
			showModeSwitch = true; // Axis / Buttons
			axisButtons.push_back(new QPushButton(tr("Click to set forward")));
			axisButtons.push_back(new QPushButton(tr("Click to set backward")));
			connect(axisButtons[i*2], SIGNAL(clicked()), this, SLOT(setButton()));
			connect(axisButtons[i*2+1], SIGNAL(clicked()), this, SLOT(setButton()));
			axisButtons[i*2]->setCheckable(true);
			axisButtons[i*2+1]->setCheckable(true);
			row[i]->addWidget(axisButtons[i*2]);
			row[i]->addWidget(axisButtons[i*2+1]);
			axisButtons[i*2]->hide();
			axisButtons[i*2+1]->hide();
			break;
		case 3: // Roll control
			showModeSwitch = true; // Axis / Buttons
			axisButtons.push_back(new QPushButton(tr("Click to set left")));
			axisButtons.push_back(new QPushButton(tr("Click to set right")));
			connect(axisButtons[i*2], SIGNAL(clicked()), this, SLOT(setButton()));
			connect(axisButtons[i*2+1], SIGNAL(clicked()), this, SLOT(setButton()));
			axisButtons[i*2]->setCheckable(true);
			axisButtons[i*2+1]->setCheckable(true);
			row[i]->addWidget(axisButtons[i*2]);
			row[i]->addWidget(axisButtons[i*2+1]);
			axisButtons[i*2]->hide();
			axisButtons[i*2+1]->hide();
			break;
		}

		if(showModeSwitch)
		{
			modeSwitch.push_back(new QPushButton());
			modeSwitch[i]->setIcon(QIcon(QPixmap::fromImage(QImage(":/resources/controller_stick.png"))));
			modeSwitch[i]->setIconSize(QSize(70, 25));
			modeSwitch[i]->setFixedSize(70, 20);
			row[i]->addWidget(modeSwitch[i]);
			connect(modeSwitch[i], SIGNAL(clicked()), this, SLOT(switchControlMode()));

			if(config != NULL)
			{
				if(configArray[i + 11] >= 0)
				{
					switchControlMode(i);
					axisButtons[i*2]->setText(tr("Button %1").arg(configArray[11 + i*2]));
					axisButtons[i*2+1]->setText(tr("Button %1").arg(configArray[11 + i*2+1]));
				}
			}
		}
		else
		{
			QLabel *buttonOnly = new QLabel();
			buttonOnly->setPixmap(QPixmap::fromImage(QImage(":/resources/controller_button-only.png")));
			row[i]->addWidget(buttonOnly);
		}

		if(config != NULL)
		{
			if(configArray[i] >= 0)
			{
				buttons[i]->setText(tr("Button %1").arg(configArray[i]));
			}
		}

		layout->addLayout(row[i]);
	}

	layout->addSpacing(50);

	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

	layout->addWidget(buttonBox);

	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

	if(configArray != NULL)
	{
		delete[] configArray;
	}
}

void ConfigureControls::switchControlMode(int row)
{
	static bool isInButtonState[] = {false, false, false, false};

	if(row < 0)
	{
		row = 0;

		if(QObject::sender() == modeSwitch[0])
		{
			row = 0;
		}
		else if(QObject::sender() == modeSwitch[1])
		{
			row = 1;
		}
		else if(QObject::sender() == modeSwitch[2])
		{
			row = 2;
		}
		else if(QObject::sender() == modeSwitch[3])
		{
			row = 3;
		}
	}

	if(isInButtonState[row])
	{
		modeSwitch[row]->setIcon(QIcon(QPixmap::fromImage(QImage(":/resources/controller_stick.png"))));
		buttons[row]->show();
		axisButtons[row*2]->hide();
		axisButtons[row*2+1]->hide();
		isInButtonState[row] = false;
	}
	else
	{
		modeSwitch[row]->setIcon(QIcon(QPixmap::fromImage(QImage(":/resources/controller_button.png"))));
		buttons[row]->hide();
		axisButtons[row*2]->show();
		axisButtons[row*2+1]->show();
		isInButtonState[row] = true;
	}
}

ControllerConfiguration *ConfigureControls::getControllerConfiguration()
{
	return cc;
}

void ConfigureControls::setAxis()
{
	int timeout = 2000;
	int sleeptime = 10;
	int selectedAxisID = -1;

	for(int i = 0; i < timeout / sleeptime; i++)
	{
		for(unsigned int j = 0; j < Gamepad_deviceAtIndex(_deviceID)->numAxes; j++)
		{
			if(Gamepad_deviceAtIndex(_deviceID)->axisStates[j] >= 0.5f || Gamepad_deviceAtIndex(_deviceID)->axisStates[j] <= -0.5f)
			{
				selectedAxisID = j;
				break;
			}
		}

		if(selectedAxisID >= 0)
		{
			break;
		}

		boost::this_thread::sleep_for(boost::chrono::milliseconds(sleeptime));
	}

	int senderID = -1;

	if(QObject::sender() == buttons[0])
	{
		// Takeoff
		senderID = 0;
		cc->height = selectedAxisID;
	}
	else if(QObject::sender() == buttons[1])
	{
		// Land
		senderID = 1;
		cc->yaw = selectedAxisID;
	}
	else if(QObject::sender() == buttons[2])
	{
		// Emergency
		senderID = 2;
		cc->pitch = selectedAxisID;
	}
	else if(QObject::sender() == buttons[3])
	{
		// Toggle recording
		senderID = 3;
		cc->roll = selectedAxisID;
	}

	if(senderID >= 0)
	{
		if(selectedAxisID >= 0)
		{
			buttons[senderID]->setText(tr("Axis %1").arg(selectedAxisID));
		}
		else
		{
			buttons[senderID]->setText(tr("Click to set"));
		}
		buttons[senderID]->setChecked(false);
	}
}

void ConfigureControls::setButton()
{
	int timeout = 2000;
	int sleeptime = 10;
	int selectedButtonID = -1;

	for(int i = 0; i < timeout / sleeptime; i++)
	{
		for(unsigned int j = 0; j < Gamepad_deviceAtIndex(_deviceID)->numButtons; j++)
		{
			if(Gamepad_deviceAtIndex(_deviceID)->buttonStates[j] == true)
			{
				selectedButtonID = j;
				break;
			}
		}

		if(selectedButtonID >= 0)
		{
			break;
		}

		boost::this_thread::sleep_for(boost::chrono::milliseconds(sleeptime));
	}

	int senderID = -1;

	if(QObject::sender() == buttons[4])
	{
		// Takeoff
		senderID = 4;
		cc->takeoff = selectedButtonID;
	}
	else if(QObject::sender() == buttons[5])
	{
		// Land
		senderID = 5;
		cc->land = selectedButtonID;
	}
	else if(QObject::sender() == buttons[6])
	{
		// Emergency
		senderID = 6;
		cc->emergency = selectedButtonID;
	}
	else if(QObject::sender() == buttons[7])
	{
		// Toggle recording
		senderID = 7;
		cc->recording = selectedButtonID;
	}
	else if(QObject::sender() == buttons[8])
	{
		// Photo
		senderID = 8;
		cc->photo= selectedButtonID;
	}
	else if(QObject::sender() == buttons[9])
	{
		// Flip
		senderID = 9;
		cc->flip = selectedButtonID;
	}
	else if(QObject::sender() == buttons[10])
	{
		// Toggle camera
		senderID = 10;
		cc->zap = selectedButtonID;
	}
	else if(QObject::sender() == buttons[11])
	{
		// Slow mode
		senderID = 11;
		cc->slow = selectedButtonID;
	}
	else if(QObject::sender() == axisButtons[0])
	{
		// Height +
		senderID = 12;
		cc->heightP = selectedButtonID;
	}
	else if(QObject::sender() == axisButtons[1])
	{
		// Height -
		senderID = 13;
		cc->heightM = selectedButtonID;
	}
	else if(QObject::sender() == axisButtons[2])
	{
		// Yaw -
		senderID = 14;
		cc->yawM = selectedButtonID;
	}
	else if(QObject::sender() == axisButtons[3])
	{
		// Yaw +
		senderID = 15;
		cc->yawP = selectedButtonID;
	}
	else if(QObject::sender() == axisButtons[4])
	{
		// Pitch -
		senderID = 16;
		cc->pitchM = selectedButtonID;
	}
	else if(QObject::sender() == axisButtons[5])
	{
		// Pitch +
		senderID = 17;
		cc->pitchP = selectedButtonID;
	}
	else if(QObject::sender() == axisButtons[6])
	{
		// Roll -
		senderID = 18;
		cc->rollM = selectedButtonID;
	}
	else if(QObject::sender() == axisButtons[7])
	{
		// Roll +
		senderID = 19;
		cc->rollP = selectedButtonID;
	}

	if(senderID >= 0)
	{
		if(senderID <= 11)
		{
			if(selectedButtonID >= 0)
			{
				buttons[senderID]->setText(tr("Button %1").arg(selectedButtonID));
			}
			else
			{
				buttons[senderID]->setText(tr("Click to set"));
			}
			buttons[senderID]->setChecked(false);
		}
		else
		{
			// axisButton
			senderID -= 12;

			if(selectedButtonID >= 0)
			{
				axisButtons[senderID]->setText(tr("Button %1").arg(selectedButtonID));
			}
			else
			{
				switch(senderID)
				{
				case 0:
					axisButtons[senderID]->setText(tr("Click to set up"));
					break;
				case 1:
					axisButtons[senderID]->setText(tr("Click to set down"));
					break;
				case 2:
					axisButtons[senderID]->setText(tr("Click to set left"));
					break;
				case 3:
					axisButtons[senderID]->setText(tr("Click to set right"));
					break;
				case 4:
					axisButtons[senderID]->setText(tr("Click to set forward"));
					break;
				case 5:
					axisButtons[senderID]->setText(tr("Click to set backward"));
					break;
				case 6:
					axisButtons[senderID]->setText(tr("Click to set left"));
					break;
				case 7:
					axisButtons[senderID]->setText(tr("Click to set right"));
					break;
				}
			}

			axisButtons[senderID]->setChecked(false);
		}
	}
}
