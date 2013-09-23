#include "gallerywidget.h"

#include <QtWidgets>

using namespace std;

GalleryWidget::GalleryWidget(QWidget *parent, QWidget *dialogParent) : QWidget(parent)
{
	setStyleSheet("background: #ABABAB;");
	setFixedHeight(120);

	QVBoxLayout *layout = new QVBoxLayout();
	setLayout(layout);

	QScrollArea *scroll = new QScrollArea();
	scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	layout->addWidget(scroll);

	_l = new QHBoxLayout();
	scroll->setLayout(_l);

	_l->setAlignment(Qt::AlignCenter);

	noImgs = new QLabel(tr("No images"));
	noImgs->setStyleSheet("font-size: 18px; color: #424242;");
	_l->addWidget(noImgs);

	_dialogParent = dialogParent;
}

void GalleryWidget::setImages(vector<string> paths)
{
	if(paths.size() == 0)
	{
		return;
	}

	noImgs->setVisible(false);

	int i = 0;

	for(string path : paths)
	{
		images.push_back(QImage(QString::fromStdString(path)));

		QPushButton *thumbnail = new QPushButton();
		thumbnail->setIcon(QIcon(QPixmap::fromImage(images[i]).scaledToWidth(120)));
		thumbnail->setIconSize(QSize(120, 67));
		thumbnail->setFixedWidth(130);
		thumbnail->setFixedHeight(77);
		thumbnail->setStyleSheet("QPushButton { background: transparent; border: none; } QPushButton:hover, QPushButton:focus { background: white; }");
		_l->addWidget(thumbnail);

		QObject::connect(thumbnail, SIGNAL(clicked()), this, SLOT(thumbnailClicked()));

		thumbnails.push_back(thumbnail);

		i++;
	}
}

void GalleryWidget::thumbnailClicked()
{
	int clickedIndex = -1;

	for(unsigned int i = 0; i < thumbnails.size(); i++)
	{
		if(QObject::sender() == thumbnails[i])
		{
			clickedIndex = i;
		}
	}

	if(clickedIndex < 0)
	{
		return;
	}

	QDialog imageDialog(_dialogParent);
	imageDialog.setWindowTitle(tr("Picture"));

	QLabel *image = new QLabel();
	image->setPixmap(QPixmap::fromImage(images[clickedIndex]));

	QVBoxLayout *l = new QVBoxLayout();
	l->addWidget(image);

	imageDialog.setLayout(l);
	imageDialog.exec();
}

void GalleryWidget::showImage(string imageUrl)
{
	QDialog imageDialog(_dialogParent);
	imageDialog.setWindowTitle(tr("Picture"));

	QLabel *image = new QLabel();
	image->setPixmap(QPixmap::fromImage(QImage(QString::fromStdString(imageUrl))));

	QVBoxLayout *l = new QVBoxLayout();
	l->addWidget(image);

	imageDialog.setLayout(l);
	imageDialog.exec();
}
