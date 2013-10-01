#include "afmainwindow.h"

#include "afconstants.h"

#include "widgets/orientation.h"
#include "widgets/altitude.h"
#include "widgets/speed.h"
#include "widgets/battery.h"
#include "widgets/signalstrength.h"
#include "widgets/connection.h"

#include "dialogs/selectcontroller.h"
#include "dialogs/configurecontrols.h"

#include <QtWidgets>

#include <opencv2/opencv.hpp>

// When on a netbook with limited space, use these values as minimum screen size:
#define MIN_WIDTH 1024
#define MIN_HEIGHT 600
// On a normal sized screen, minimum screen size should be:
#define PREF_WIDTH 1270
#define PREF_HEIGHT 775

#define CONFIRMATION_TIMEOUT 1250

using namespace std;

void AFMainWindow::setWindowAttributes()
{
	setWindowTitle("AutoFlight");
	setWindowIcon(QIcon(":/resources/icon.png"));
	setMinimumSize(PREF_WIDTH, PREF_HEIGHT); //TODO: netbook support
}

AFMainWindow::AFMainWindow(AutoFlight *af, QWidget *parent) : QMainWindow(parent)
{
	_af = af;

	_messageTimer = new QTimer(this);
	QObject::connect(_messageTimer, SIGNAL(timeout()), this, SLOT(hideMessages()));
	_messageTimer->setSingleShot(true);


	QWidget *main = new QWidget();
	setCentralWidget(main);
	
	grid = new QGridLayout();
	main->setLayout(grid);
	
	setWindowAttributes();
	
	createMenuBar();
	
	// Shows important notifications
	msg = new QLabel(this);
	msg->setAlignment(Qt::AlignCenter);
	msg->setStyleSheet("background: rgba(30, 30, 30, 0.85); font-size: 24px; color: #FFFFFF; border-radius: 10px;");
	msg->hide();

	videoPanel = new VideoDisplay();
	//videoPanel->setAlignment(Qt::AlignCenter);
	grid->addWidget(videoPanel, 0, 0, 1, 1);

	videoPanel->setCurrentFrame(QImage(":/resources/autoflight.png"));

	horizontalToolbar = createHorizontalToolbar();
	verticalToolbar = createVerticalToolbar();

	grid->addWidget(horizontalToolbar, 1, 0, 1, 1);
	grid->addWidget(verticalToolbar, 0, 1, 2, 1);

	_af->ardrone()->addNavdataListener(this);
	_af->ardrone()->addVideoListener(this);
	_af->ardrone()->addControllerInputListener(this);

	QObject::connect(this, SIGNAL(videoFrameAvailableSignal(QImage)), this, SLOT(videoFrameAvailable(QImage)));
	QObject::connect(this, SIGNAL(navdataAvailableSignal(AFNavdata *)), videoPanel, SLOT(navdataAvailable(AFNavdata *)));
	QObject::connect(this, SIGNAL(controllerInputAvailableSignal(ControllerInput *)), videoPanel, SLOT(controllerInputAvailable(ControllerInput *)));

	installEventFilter(this);
}

void AFMainWindow::showMessage(string message)
{
	msg->setText(QString::fromStdString(message));
	msg->setGeometry((int) (size().width() / 2 - 200), (int) (size().height() / 2 - 25), 400, 50);
	msg->raise();
	msg->show();

	_messageTimer->start(CONFIRMATION_TIMEOUT);
}

void AFMainWindow::hideMessages()
{
	_messageTimer->stop(); // Stop the timer if it's running
	msg->hide();
}

void AFMainWindow::navdataAvailable(AFNavdata *nd)
{
	Q_EMIT navdataAvailableSignal(nd);
}

void AFMainWindow::videoFrameAvailable(cv::Mat f)
{
	// Convert cv::Mat to QImage
	QImage img(f.data, f.cols, f.rows, f.step, QImage::Format_RGB888);
	img = img.rgbSwapped();

	Q_EMIT videoFrameAvailableSignal(img);
}

void AFMainWindow::controllerInputAvailable(ControllerInput *in)
{
	Q_EMIT controllerInputAvailableSignal(in);
}

void AFMainWindow::videoFrameAvailable(QImage f)
{
	videoPanel->setMaximized(true);
	videoPanel->setCurrentFrame(f);
}

void AFMainWindow::createMenuBar() {
	QMenu *drone = new QMenu(tr("AR.Drone"));
	menuBar()->addMenu(drone);
		
		QAction *connectDrone = new QAction(tr("Connect to AR.Drone"), this);
		drone->addAction(connectDrone);
		
		/* TODO: This
		QAction *connectArduino = new QAction(tr("Connect to Arduino"), this);
		drone->addAction(connectArduino);
		*/
		
		drone->addSeparator();
		
		QAction *flatTrim = new QAction(tr("Flat Trim"), this);
		drone->addAction(flatTrim);
		
		QAction *calibMagneto = new QAction(tr("Calibrate Magnetometer"), this);
		drone->addAction(calibMagneto);
		
		drone->addSeparator();
		/* TODO: This
		QAction *pairDrone = new QAction(tr("Pair (Mac-Address coupling)"), this);
		drone->addAction(pairDrone);
		
		QAction *unpairDrone = new QAction(tr("Unpair"), this);
		drone->addAction(unpairDrone);
		*/
	QMenu *tools = new QMenu(tr("Tools"));
	menuBar()->addMenu(tools);
	
		QAction *controlConfig = new QAction(tr("Controller Configuration"), this);
		tools->addAction(controlConfig);
		/* TODO: This
		QAction *controlInfo = new QAction(tr("Controller Information"), this);
		tools->addAction(controlInfo);
		
		tools->addSeparator();
		
		QAction *configEditor = new QAction(tr("Configuration Editor"), this);
		tools->addAction(configEditor);
		
		tools->addSeparator();
		
		QAction *gpsViewer = new QAction(tr("GPS Viewer"), this);
		tools->addAction(gpsViewer);
		*/
	QMenu *view = new QMenu(tr("View"));
	menuBar()->addMenu(view);
		QAction *toggleHUD = new QAction(tr("Head-Up Display"), this);
		toggleHUD->setCheckable(true);
		toggleHUD->setShortcut(QKeySequence::fromString("F5"));
		view->addAction(toggleHUD);
		
		/* TODO: This
		view->addSeparator();
		
		QAction *toggleFullscreen = new QAction(tr("Fullscreen"), this);
		toggleFullscreen->setCheckable(true);
		view->addAction(toggleFullscreen);
		*/
	QMenu *help = new QMenu(tr("Help"));
	menuBar()->addMenu(help);
		/* TODO: This
		QAction *onlineHelp = new QAction(tr("Online Help"), this);
		help->addAction(onlineHelp);
		
		help->addSeparator();
		*/
		QAction *about = new QAction(tr("About AutoFlight"), this);
		help->addAction(about);

	QWidget::connect(connectDrone, SIGNAL(triggered()), this, SLOT(attemptConnection()));
	QWidget::connect(flatTrim, SIGNAL(triggered()), this, SLOT(flatTrimActionTriggered()));
	QWidget::connect(calibMagneto, SIGNAL(triggered()), this, SLOT(calibrateMagnetometerActionTriggered()));

	QWidget::connect(controlConfig, SIGNAL(triggered()), this, SLOT(showControlConfigDialog()));

	QWidget::connect(toggleHUD, SIGNAL(triggered(bool)), this, SLOT(toggleHUD(bool)));

	QWidget::connect(about, SIGNAL(triggered()), this, SLOT(showAboutDialog()));
}

QWidget *AFMainWindow::createVerticalToolbar()
{
	QVBoxLayout *layout = new QVBoxLayout();
	layout->setSpacing(5);
	
	QWidget *panel = new QWidget();
	panel->setLayout(layout);
	panel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	
	Connection *c = new Connection();
	QObject::connect(c, SIGNAL(droneConnectionRequested()), this, SLOT(attemptConnection()));
	layout->addWidget(c);
	
	layout->addStretch();
	
	QPushButton *launchSessionViewer = new QPushButton("Session Viewer");
	//launchSessionViewer->setIcon(QIcon(QPixmap::fromImage(QImage(":/resources/sessionviewer.png"))));
	launchSessionViewer->setIconSize(QSize(200, 50));
	QObject::connect(launchSessionViewer, SIGNAL(clicked()), this, SLOT(launchSessionViewerDialog()));
	layout->addWidget(launchSessionViewer);

	QPushButton *launchAutoScript = new QPushButton();
	launchAutoScript->setIcon(QIcon(QPixmap::fromImage(QImage(":/resources/autoscript.png"))));
	launchAutoScript->setIconSize(QSize(200, 50));
	QObject::connect(launchAutoScript, SIGNAL(clicked()), this, SLOT(launchAutoScriptIDE()));
	layout->addWidget(launchAutoScript);

	return panel;
}

QWidget *AFMainWindow::createHorizontalToolbar()
{	
	QHBoxLayout *layout = new QHBoxLayout();
	layout->setSpacing(5);
	
	QWidget *panel = new QWidget();
	panel->setLayout(layout);
	panel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	
	layout->addStretch();
	
	Orientation *o = new Orientation();
	layout->addWidget(o);
	QObject::connect(this, SIGNAL(navdataAvailableSignal(AFNavdata *)), o, SLOT(navdataAvailable(AFNavdata *)));
	
	Altitude *a = new Altitude();
	layout->addWidget(a);
	QObject::connect(this, SIGNAL(navdataAvailableSignal(AFNavdata *)), a, SLOT(navdataAvailable(AFNavdata *)));
	
	Speed *s = new Speed();
	layout->addWidget(s);
	QObject::connect(this, SIGNAL(navdataAvailableSignal(AFNavdata *)), s, SLOT(navdataAvailable(AFNavdata *)));
	
	Battery *b = new Battery();
	layout->addWidget(b);
	QObject::connect(this, SIGNAL(navdataAvailableSignal(AFNavdata *)), b, SLOT(navdataAvailable(AFNavdata *)));
	
	SignalStrength *ss = new SignalStrength();
	layout->addWidget(ss);
	
	layout->addStretch();
	
	return panel;
}

void AFMainWindow::attemptConnection()
{
	if(!_af->attemptConnectionToDrone())
	{
		QMessageBox::warning(this, tr("Could not connect"), tr("AutoFlight could not connect to the AR.Drone. Please make sure that you are connected to it over WiFi and try again."));
	}
}

void AFMainWindow::toggleHUD(bool showHUD)
{
	videoPanel->showHUD(showHUD);

	if(showHUD)
	{
		grid->removeWidget(videoPanel);
		grid->removeWidget(horizontalToolbar);
		grid->removeWidget(verticalToolbar);
		grid->addWidget(videoPanel, 0, 0, 2, 2);
	}
	else
	{
		grid->removeWidget(videoPanel);
		grid->addWidget(horizontalToolbar, 1, 0, 1, 1);
		grid->addWidget(videoPanel, 0, 0, 1, 1);
		grid->addWidget(verticalToolbar, 0, 1, 2, 1);
	}

	horizontalToolbar->setVisible(!showHUD);
	verticalToolbar->setVisible(!showHUD);
}

void AFMainWindow::launchAutoScriptIDE()
{
	if(_asWindow == NULL)
	{
		_asWindow = new ASMainWindow(_af->asengine(), this);
	}

	_asWindow->show();
}

void AFMainWindow::launchSessionViewerDialog()
{
	if(_sessionViewerWindow == NULL)
	{
		_sessionViewerWindow = new SessionViewer(this);
	}

	_sessionViewerWindow->show();
}

void AFMainWindow::showControlConfigDialog()
{
	bool invalidController = true;
	bool cancel = false;
	unsigned int controllerID = 0;

	while(invalidController && !cancel)
	{
		SelectController sc(this);
		sc.exec();

		if(sc.result() == QDialog::Accepted)
		{
			if(sc.getSelectedDeviceID() == -1)
			{
				QMessageBox::warning(this, tr("Error"), tr("Please select a device."));
				invalidController = true;
			}
			else
			{
				invalidController = false;
				controllerID = sc.getSelectedDeviceID();
			}
		}
		else
		{
			cancel = true;
		}
	}

	if(!cancel)
	{
		ConfigureControls cc(controllerID, NULL, this);
		cc.exec();

		if(cc.result() == QDialog::Accepted)
		{
			//TODO: Check configuration
			_af->ardrone()->setControllerConfiguration(cc.getControllerConfiguration());
		}
	}
}

void AFMainWindow::clearConfirmationFlags()
{
	_confirmEmergency = false;
	_confirmFlip = false;
}

bool AFMainWindow::eventFilter(QObject *watched, QEvent* e)
{
	if(e->type() == QEvent::KeyPress)
	{
		QKeyEvent *ke = static_cast<QKeyEvent*>(e);

		if(!ke->isAutoRepeat())
		{
			if(_af->ardrone()->isConnected())
			{
				switch(ke->key())
				{
				case Qt::Key_T:
					// Take off or land
					if(_af->ardrone()->drone_isFlying())
					{
						showMessage(tr("Landing").toStdString());
						_af->ardrone()->drone_land();
					}
					else
					{
						showMessage(tr("Taking off").toStdString());
						_af->ardrone()->drone_takeOff();
					}

					break;
				case Qt::Key_F:
					if(!_confirmFlip)
					{
						showMessage(tr("[Flip] Are you sure?").toStdString());
						QTimer::singleShot(CONFIRMATION_TIMEOUT, this, SLOT(clearConfirmationFlags()));
						_confirmFlip = true;
					}
					else
					{
						hideMessages();
						showMessage(tr("Performing flip!").toStdString());
						_af->ardrone()->drone_flip(ardrone::flip::LEFT);
					}
					// Flip
					break;
				case Qt::Key_P:
					// Take Picture
					_af->ardrone()->drone_takePicture();
					showMessage(tr("Picture saved").toStdString());
					break;
				case Qt::Key_R:
					// Start/stop Recording
					if(_af->ardrone()->drone_isRecording())
					{
						showMessage(tr("Stopped recording").toStdString());
					}
					else
					{
						showMessage(tr("Recording").toStdString());
					}
					_af->ardrone()->drone_toggleRecording();
					break;
				case Qt::Key_N:
					// Start/stop Recording Sensor Data
					if(_af->ardrone()->drone_isRecordingNavdata())
					{
						showMessage(tr("Stopped recording sensor data").toStdString());
					}
					else
					{
						showMessage(tr("Recording sensor data").toStdString());
					}
					_af->ardrone()->drone_toggleRecordingNavdata();
					break;
				case Qt::Key_V:
					// Change View
					showMessage(tr("Switching view").toStdString());
					_af->ardrone()->drone_toggleView();
					break;
				case Qt::Key_Y:
					if(!_confirmEmergency)
					{
						showMessage(tr("[Emergency] Are you sure?").toStdString());
						QTimer::singleShot(CONFIRMATION_TIMEOUT, this, SLOT(clearConfirmationFlags()));
						_confirmEmergency = true;
					}
					else
					{
						hideMessages();
						showMessage(tr("Emergency command sent").toStdString());
						_af->ardrone()->drone_emergency();
					}
					// Emergency
					break;

				case Qt::Key_W:
					// Pitch forward
					_af->ardrone()->drone_setTheta(-0.4f);
					break;
				case Qt::Key_A:
					// Roll left
					_af->ardrone()->drone_setPhi(-0.4f);
					break;
				case Qt::Key_S:
					// Pitch backwards
					_af->ardrone()->drone_setTheta(0.4f);
					break;
				case Qt::Key_D:
					// Roll right
					_af->ardrone()->drone_setPhi(0.4f);
					break;

				case Qt::Key_I:
					// Ascend
					_af->ardrone()->drone_setGaz(0.6f);
					break;
				case Qt::Key_J:
					// Rotate counterclockwise
					_af->ardrone()->drone_setYaw(-0.6f);
					break;
				case Qt::Key_K:
					// Descend
					_af->ardrone()->drone_setGaz(-0.6f);
					break;
				case Qt::Key_L:
					// Rotate clockwise
					_af->ardrone()->drone_setYaw(0.6f);
					break;

				default:
					break;
				}
			}
			else
			{
				if(ke->key() >= Qt::Key_0 && ke->key() <= Qt::Key_Z)
				{
					showMessage("Not connected to AR.Drone");
				}
			}
		}
	}
	else if(e->type() == QEvent::KeyRelease)
	{
		QKeyEvent *ke = static_cast<QKeyEvent*>(e);

		if(!ke->isAutoRepeat())
		{
			switch(ke->key())
			{
			case Qt::Key_W:
				// Stop forward pitch
				_af->ardrone()->drone_setTheta(0);
				break;
			case Qt::Key_A:
				// Stop left roll
				_af->ardrone()->drone_setPhi(0);
				break;
			case Qt::Key_S:
				// Stop backward pitch
				_af->ardrone()->drone_setTheta(0);
				break;
			case Qt::Key_D:
				// Stop right roll
				_af->ardrone()->drone_setPhi(0);
				break;

			case Qt::Key_I:
				// Stop ascending
				_af->ardrone()->drone_setGaz(0);
				break;
			case Qt::Key_J:
				// Stop rotating counterclockwise
				_af->ardrone()->drone_setYaw(0);
				break;
			case Qt::Key_K:
				// Stop descending
				_af->ardrone()->drone_setGaz(0);
				break;
			case Qt::Key_L:
				// Stop rotating clockwise
				_af->ardrone()->drone_setYaw(0);
				break;

			default:
				break;
			}
		}
	}

	return false;
}

void AFMainWindow::closeEvent(QCloseEvent *event)
{
	_af->sessionrecorder()->addEvent("ProgramExit");

	_af->ardrone()->removeNavdataListener(this);
	_af->ardrone()->removeVideoListener(this);
	_af->ardrone()->removeControllerInputListener(this);
	_af->saveSession();
	//TODO: exit confirmation dialog, etc.
}

void AFMainWindow::flatTrimActionTriggered()
{
	if(!_af->ardrone()->drone_flattrim())
	{
		showMessage(tr("Not connected").toStdString());
	}
	else
	{
		showMessage(tr("Performing Flat Trim").toStdString());
	}
}

void AFMainWindow::calibrateMagnetometerActionTriggered()
{
	if(!_af->ardrone()->drone_calibmagneto())
	{
		showMessage(tr("Not connected").toStdString());
	}
	else
	{
		showMessage(tr("Calibrating Magnetometer").toStdString());
	}
}

void AFMainWindow::showAboutDialog()
{
	QDialog aboutDialog(this);
	aboutDialog.setWindowTitle(tr("About AutoFlight"));
	aboutDialog.setMinimumHeight(200);

	QHBoxLayout *l = new QHBoxLayout();
	aboutDialog.setLayout(l);

	QVBoxLayout *left = new QVBoxLayout();
	left->setAlignment(Qt::AlignTop);
	l->addLayout(left);

	QLabel *icon = new QLabel();
	icon->setPixmap(QPixmap::fromImage(QImage(":/resources/icon.png").scaled(75, 75)));
	icon->setFixedSize(75, 75);
	left->addWidget(icon);

	QVBoxLayout *right = new QVBoxLayout();
	l->addLayout(right);

	QLabel *autoflight = new QLabel(tr("About AutoFlight"));
	autoflight->setStyleSheet("font-size: 24px");
	right->addWidget(autoflight);

	QLabel *version = new QLabel(tr("Version ").append(QString::fromStdString(autoflight::SOFTWARE_VERSION)));
	right->addWidget(version);

	right->addStretch();

	QChar copyrightSymbol(0x00A9);
	QLabel *copyright = new QLabel(QString("Copyright ").append(copyrightSymbol).append(" 2013 Lukas Lao Beyer"));
	right->addWidget(copyright);

	QLabel *website = new QLabel(QString("<a href=\"http://lbpclabs.com/autoflight\">lbpclabs.com/autoflight</a>"));
	website->setOpenExternalLinks(true);
	right->addWidget(website);

	aboutDialog.exec();
}
