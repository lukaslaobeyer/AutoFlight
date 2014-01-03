#ifndef DRONESETTINGS_H
#define DRONESETTINGS_H

#include <QWidget>
#include <QDialog>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QCheckBox>
#include "../ardrone/ardroneconfiguration.h"

class DroneSettings : public QDialog
{
	Q_OBJECT

	public:
		explicit DroneSettings(ARDroneConfiguration *ardsettings, QWidget *parent = 0);

		ARDroneConfiguration *getConfiguration();
	private Q_SLOTS:
		void valueChanged(int val);
		void valueChanged(double val);

		void handleAccept();
		void handleApply();
	private:
		ARDroneConfiguration *_ardsettings = nullptr;

		QSlider *alt_max_slider, *tilt_max_slider, *vz_max_slider, *vyaw_max_slider;
		QCheckBox *outdoor_ckbx, *no_hull_ckbx;
		QDoubleSpinBox *alt_max_spinner, *vz_max_spinner;
		QSpinBox *tilt_max_spinner, *vyaw_max_spinner;
};

#endif
