#include "dronesettings.h"

#include <QtWidgets>

DroneSettings::DroneSettings(ARDroneConfiguration *ardsettings, QWidget *parent) : QDialog(parent)
{
	setWindowTitle(tr("AR.Drone Flight Settings"));

	setMinimumWidth(600);

	QGridLayout *layout = new QGridLayout();
	setLayout(layout);

	QLabel *alt_max = new QLabel(tr("Altitude limit"));
	QLabel *tilt_max = new QLabel(tr("Max. tilt"));
	QLabel *vz_max = new QLabel(tr("Max. vertical velocity"));
	QLabel *vyaw_max = new QLabel(tr("Max. yaw rotation speed"));
	QLabel *outdoor = new QLabel(tr("Outdoor flight"));
	QLabel *no_hull = new QLabel(tr("Flight without indoor hull"));

	layout->addWidget(alt_max, 0, 0);
	layout->addWidget(tilt_max, 1, 0);
	layout->addWidget(vz_max, 2, 0);
	layout->addWidget(vyaw_max, 3, 0);
	layout->addWidget(outdoor, 4, 0);
	layout->addWidget(no_hull, 5, 0);

	alt_max_slider = new QSlider(Qt::Horizontal);
	alt_max_slider->setTracking(true);
	alt_max_slider->setRange(12, 500);

	tilt_max_slider = new QSlider(Qt::Horizontal);
	tilt_max_slider->setTracking(true);
	tilt_max_slider->setRange(5, 30);

	vz_max_slider = new QSlider(Qt::Horizontal);
	vz_max_slider->setTracking(true);
	vz_max_slider->setRange(20, 200);

	vyaw_max_slider = new QSlider(Qt::Horizontal);
	vyaw_max_slider->setTracking(true);
	vyaw_max_slider->setRange(40, 350);

	outdoor_ckbx = new QCheckBox();

	no_hull_ckbx = new QCheckBox();

	layout->addWidget(alt_max_slider, 0, 1);
	layout->addWidget(tilt_max_slider, 1, 1);
	layout->addWidget(vz_max_slider, 2, 1);
	layout->addWidget(vyaw_max_slider, 3, 1);
	layout->addWidget(outdoor_ckbx, 4, 1);
	layout->addWidget(no_hull_ckbx, 5, 1);

	alt_max_spinner = new QDoubleSpinBox();
	alt_max_spinner->setDecimals(1);
	alt_max_spinner->setRange(1.2, 50);

	tilt_max_spinner = new QSpinBox();
	tilt_max_spinner->setRange(5, 30);

	vz_max_spinner = new QDoubleSpinBox();
	vz_max_spinner->setDecimals(1);
	vz_max_spinner->setRange(0.2, 2);

	vyaw_max_spinner = new QSpinBox();
	vyaw_max_spinner->setRange(40, 350);

	layout->addWidget(alt_max_spinner, 0, 2);
	layout->addWidget(tilt_max_spinner, 1, 2);
	layout->addWidget(vz_max_spinner, 2, 2);
	layout->addWidget(vyaw_max_spinner, 3, 2);

	QLabel *alt_max_unit = new QLabel("m");
	QLabel *tilt_max_unit = new QLabel("degrees");
	QLabel *vz_max_unit = new QLabel("m/s");
	QLabel *vyaw_max_unit = new QLabel("deg/s");

	layout->addWidget(alt_max_unit, 0, 3);
	layout->addWidget(tilt_max_unit, 1, 3);
	layout->addWidget(vz_max_unit, 2, 3);
	layout->addWidget(vyaw_max_unit, 3, 3);

	QSpacerItem *spacer = new QSpacerItem(10, 25, QSizePolicy::Expanding, QSizePolicy::Expanding);
	layout->addItem(spacer, 6, 0, 1, 4);

	QDialogButtonBox *bbox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply);
	layout->addWidget(bbox, 7, 0, 1, 4, Qt::AlignCenter);

	QObject::connect(alt_max_slider, SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)));
	QObject::connect(tilt_max_slider, SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)));
	QObject::connect(vz_max_slider, SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)));
	QObject::connect(vyaw_max_slider, SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)));

	QObject::connect(alt_max_spinner, SIGNAL(valueChanged(double)), this, SLOT(valueChanged(double)));
	QObject::connect(tilt_max_spinner, SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)));
	QObject::connect(vz_max_spinner, SIGNAL(valueChanged(double)), this, SLOT(valueChanged(double)));
	QObject::connect(vyaw_max_spinner, SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)));

	QObject::connect(bbox, SIGNAL(accepted()), this, SLOT(handleAccept()));
	QObject::connect(bbox, SIGNAL(rejected()), this, SLOT(reject()));
	QObject::connect(bbox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(handleApply()));

	// Apply control values
	if(ardsettings != nullptr)
	{
		alt_max_slider->setValue((int)(ardsettings->altitude_max * 10.0));
		alt_max_spinner->setValue(ardsettings->altitude_max);
		tilt_max_slider->setValue((int)(ardsettings->pitch_roll_max));
		tilt_max_spinner->setValue((int) ardsettings->pitch_roll_max);
		vz_max_slider->setValue((int)(ardsettings->vertical_speed_max * 100.0));
		vz_max_spinner->setValue(ardsettings->vertical_speed_max);
		vyaw_max_slider->setValue((int)(ardsettings->yaw_speed_max));
		vyaw_max_spinner->setValue((int) ardsettings->yaw_speed_max);

		if(ardsettings->no_hull)
		{
			no_hull_ckbx->setCheckState(Qt::Checked);
		}
		else
		{
			no_hull_ckbx->setCheckState(Qt::Unchecked);
		}

		if(ardsettings->outdoor_flight)
		{
			outdoor_ckbx->setCheckState(Qt::Checked);
		}
		else
		{
			outdoor_ckbx->setCheckState(Qt::Unchecked);
		}
	}
}

void DroneSettings::valueChanged(int value)
{
	QObject *sender = QObject::sender();

	if(sender == alt_max_slider)
	{
		alt_max_spinner->setValue((double) value / 10.0);
	}
	else if(sender == tilt_max_slider || sender == tilt_max_spinner)
	{
		tilt_max_slider->setValue(value);
		tilt_max_spinner->setValue(value);
	}
	else if(sender == vz_max_slider)
	{
		vz_max_spinner->setValue((double) value / 100.0);
	}
	else if(sender == vyaw_max_slider || sender == vyaw_max_spinner)
	{
		vyaw_max_slider->setValue(value);
		vyaw_max_spinner->setValue(value);
	}
}

void DroneSettings::valueChanged(double value)
{
	QObject *sender = QObject::sender();

	if(sender == alt_max_spinner)
	{
		alt_max_slider->setValue(value * 10);
	}
	else if(sender == vz_max_spinner)
	{
		vz_max_slider->setValue(value * 100);
	}
}

void DroneSettings::handleApply()
{
	// Apply changes
	if(_ardsettings == nullptr)
	{
		_ardsettings = new ARDroneConfiguration;
	}

	_ardsettings->altitude_max = alt_max_spinner->value();
	_ardsettings->pitch_roll_max = tilt_max_spinner->value();
	_ardsettings->vertical_speed_max = vz_max_spinner->value();
	_ardsettings->yaw_speed_max = vyaw_max_spinner->value();

	if(no_hull_ckbx->checkState() == Qt::Checked)
	{
		_ardsettings->no_hull = true;
	}
	else
	{
		_ardsettings->no_hull = false;
	}

	if(outdoor_ckbx->checkState() == Qt::Checked)
	{
		_ardsettings->outdoor_flight = true;
	}
	else
	{
		_ardsettings->outdoor_flight = false;
	}
}

void DroneSettings::handleAccept()
{
	handleApply();
	accept();
}

ARDroneConfiguration *DroneSettings::getConfiguration()
{
	return _ardsettings;
}
