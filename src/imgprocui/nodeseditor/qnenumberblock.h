#ifndef QNENUMBERBLOCK_H
#define QNENUMBERBLOCK_H

#include "qneblock.h"
#include "qneport.h"
#include <QLineEdit>

class QNENumberBlock : public QNEBlock
{
	public:
		QNENumberBlock(QGraphicsItem *parent = 0);
		void initialize();

		int intValue();
		double doubleValue();
		void setValue(double val);
	private:
		QLineEdit *_lineEdit = NULL;
};

#endif
