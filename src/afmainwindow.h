#ifndef AFMAINWINDOW_H
#define AFMAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QWidget>
#include <QEvent>
#include <QTimer>
#include <QGridLayout>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
#include "autoflight.h"
#include "imgprocui/imgprocmainwindow.h"
#include "dialogs/sessionviewer.h"
#include "ardrone/ardrone.h"
#include "ardrone/navdata/inavdatalistener.h"
#include "ardrone/navdata/qnavdatalistener.h"
#include "ardrone/video/ivideolistener.h"
#include "ardrone/video/qvideolistener.h"
#include "ardrone/navdata/navdatakeys.h"
#include "ardrone/input/controllerinput.h"
#include "ardrone/input/icontrollerinputlistener.h"
#include "widgets/map3d.h"
#include "widgets/videodisplay.h"
#include "asmainwindow.h"

class AFMainWindow : public QMainWindow, public INavdataListener, public IConnectionStatusListener, public IVideoListener, public IControllerInputListener, public QVideoListener
{
	Q_OBJECT
	
	public:
		explicit AFMainWindow(AutoFlight *af, QWidget *parent = 0);

		void navdataAvailable(AFNavdata *nd);
		void videoFrameAvailable(cv::Mat f);
		void controllerInputAvailable(ControllerInput *in);
		void connectionLost();
		void connectionEstablished();

		void showMessage(std::string msg);
	public Q_SLOTS:
		void hideMessages();
		void flatTrimActionTriggered();
		void calibrateMagnetometerActionTriggered();
		void showAboutDialog();
	protected:
		bool eventFilter(QObject *sender, QEvent *e);
	private:
		void setWindowAttributes();
		void closeEvent(QCloseEvent *);
		void createMenuBar();
		void showFirstRunInfoIfRequired();
		QWidget *createVerticalToolbar();
		QWidget *createHorizontalToolbar();

		QGridLayout *grid;

		QWidget *verticalToolbar;
		QWidget *horizontalToolbar;

		VideoDisplay *videoPanel;
		QLabel *msg;

		QTimer *_messageTimer = NULL;

		bool _confirmFlip = false;      // Needed to request a confirmation for performing a flip/sending an emergency command
		bool _confirmEmergency = false;

		AutoFlight *_af = NULL;

		ASMainWindow *_asWindow = NULL;
		SessionViewer *_sessionViewerWindow = NULL;
		Map3D *_map = NULL;
		ImgProcMainWindow *_imgProc = NULL;
	private Q_SLOTS:
		void attemptConnection();
		void showControlConfigDialog();
		void videoFrameAvailable(QImage f);
		void clearConfirmationFlags(); // Called after a timeout to clear the flags used by the confirmation mechanism for performing a flip/sending emergency commands
		void toggleHUD(bool showHUD);
		void launchAutoScriptIDE();
		void launchSessionViewerDialog();
		void launch3DMapView();
		void launchImageProcessingPipelineEditor();
		void handleConnectionLost();
	Q_SIGNALS:
		void navdataAvailableSignal(AFNavdata *nd);
		void videoFrameAvailableSignal(QImage frame);
		void controllerInputAvailableSignal(ControllerInput *in);
		void connectionLostSignal();
};

#endif
