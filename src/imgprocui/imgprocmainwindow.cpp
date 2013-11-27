#include "imgprocmainwindow.h"

#include <QtWidgets>
#include "visualpipeline.h"

ImgProcMainWindow::ImgProcMainWindow(QWidget *parent) : QMainWindow(parent)
{
	setWindowTitle(tr("Image Processing Pipeline Editor"));
	setMinimumSize(500, 350);

	VisualPipeline *pipeline = new VisualPipeline();
	setCentralWidget(pipeline);

	pipeline->addBlock(VisualPipeline::test);
	pipeline->addBlock(VisualPipeline::test);
	pipeline->addBlock(VisualPipeline::test);
	pipeline->addBlock(VisualPipeline::test);
	pipeline->addBlock(VisualPipeline::test);
}
