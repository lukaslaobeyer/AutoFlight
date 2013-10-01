#include "selectcontroller.h"
#include <QtWidgets>

SelectController::SelectController(QWidget *parent) : QDialog(parent)
{
	setWindowTitle(tr("Select Input Device"));

	Gamepad_detectDevices();

	QGridLayout *layout = new QGridLayout;

	setLayout(layout);

	controllerListWidget = new QListWidget();
	QListWidgetItem *controllerItem[Gamepad_numDevices()];

	int device_i = 0;

	for(unsigned int i = 0; i < Gamepad_numDevices(); i++)
	{
		if(Gamepad_deviceAtIndex(i) != NULL)
		{
			controllerItem[device_i] = new QListWidgetItem(Gamepad_deviceAtIndex(i)->description, controllerListWidget);
			controllerList.push_back(Gamepad_deviceAtIndex(i));

			device_i++;
		}
	}

	layout->addWidget(controllerListWidget, 0, 0);

	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

	layout->addWidget(buttonBox, 1, 0);

	connect(controllerListWidget, SIGNAL(itemSelectionChanged()), this, SLOT(selectedDeviceChanged()));

	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

void SelectController::selectedDeviceChanged()
{
	selectedDeviceID = controllerList[controllerListWidget->row(controllerListWidget->selectedItems().at(0))]->deviceID;
}

int SelectController::getSelectedDeviceID()
{
	return selectedDeviceID;
}
