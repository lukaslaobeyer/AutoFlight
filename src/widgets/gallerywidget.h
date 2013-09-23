#ifndef GALLERYWIDGET_H
#define GALLERYWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QImage>
#include <QPushButton>
#include <string>
#include <vector>

class GalleryWidget : public QWidget
{
	Q_OBJECT

	public:
		GalleryWidget(QWidget *parent = 0, QWidget *dialogParent = 0);

		void setImages(std::vector<std::string> paths);
		void showImage(std::string imageUrl);

	private Q_SLOTS:
		void thumbnailClicked();

	private:
		QHBoxLayout *_l;
		QLabel *noImgs;
		std::vector<QImage> images;
		std::vector<QPushButton *> thumbnails;
		QWidget *_dialogParent;
};

#endif
