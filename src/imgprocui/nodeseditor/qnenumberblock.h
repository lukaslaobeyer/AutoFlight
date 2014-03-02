#ifndef QNENUMBERBLOCK_H
#define QNENUMBERBLOCK_H

#include "qneblock.h"
#include "qneport.h"
#include <QLineEdit>
#include "inodeattributelistener.h"

class QNENumberBlock : public QNEBlock
{
	public:
		QNENumberBlock(QGraphicsItem *parent = 0);
		void initialize(/*INodeAttributeListener *l*/);

		int intValue();
		double doubleValue();
		void setValue(double val);
	private:
		QLineEdit *_lineEdit = NULL;
		/*INodeAttributeListener *_l = nullptr;*/
};

#endif
