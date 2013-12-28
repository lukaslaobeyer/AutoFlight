#include "welcomedialog.h"
#include <QtWidgets>

WelcomeDialog::WelcomeDialog(QWidget *parent) : QDialog(parent)
{
	setWindowTitle(tr("Welcome to AutoFlight"));

	QVBoxLayout *l = new QVBoxLayout;

	QPushButton *welcome = new QPushButton();
	welcome->setIcon(QPixmap(":/resources/welcome.png"));
	welcome->setIconSize(QSize(640, 400));
	welcome->setStyleSheet("padding: 0;");
	welcome->setFlat(true);
	welcome->setFocusPolicy(Qt::NoFocus);
	l->addWidget(welcome);

	QPushButton *close = new QPushButton(tr("Try AutoFlight!"));
	close->setStyleSheet("font-size: 24px; padding: 20px;");
	l->addWidget(close);

	setLayout(l);
	setFixedSize(l->sizeHint());

	connect(welcome, SIGNAL(clicked()), this, SLOT(openGettingStartedPage()));
	connect(close, SIGNAL(clicked()), this, SLOT(close()));
}

void WelcomeDialog::openGettingStartedPage()
{
	QDesktopServices::openUrl(QUrl("http://lbpclabs.com/gettingstarted"));
}
