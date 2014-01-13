#include "qnenumberblock.h"
#include <QtWidgets>

QNENumberBlock::QNENumberBlock(QGraphicsItem *parent) : QNEBlock(parent) {}

void QNENumberBlock::initialize()
{
	_lineEdit = new QLineEdit("a number");
	_lineEdit->setAlignment(Qt::AlignRight);
	_lineEdit->setStyleSheet("border: none; border-bottom: 1.5px solid #606060; background: transparent;");
	QGraphicsProxyWidget *lineEdit = scene()->addWidget(_lineEdit);
	lineEdit->setParentItem(this);
	lineEdit->setPos(-48, 10);
	lineEdit->setMaximumWidth(96);
	lineEdit->setMaximumHeight(30);

	addPort("Number", 0, QNEPort::NamePort);
	QNEPort *port = addPort("", true);
	port->setPos(55, 22);

	QPainterPath p;
	p.addRoundedRect(-50, -20, 100, 58, 5, 5);
	setPath(p);
}

int QNENumberBlock::intValue()
{
	if(_lineEdit)
	{
		int number;
		bool ok;

		number = _lineEdit->text().toInt(&ok);

		if(ok)
		{
			return number;
		}
	}

	return 0;
}

double QNENumberBlock::doubleValue()
{
	if(_lineEdit)
	{
		double number;
		bool ok;

		number = _lineEdit->text().toDouble(&ok);

		if(ok)
		{
			return number;
		}
	}

	return 0;
}

void QNENumberBlock::setValue(double value)
{
	if(_lineEdit)
	{
		_lineEdit->setText(QString::number(value));
	}
}
