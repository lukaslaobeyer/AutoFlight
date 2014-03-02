/* Copyright (c) 2012, STANISLAW ADASZEWSKI
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of STANISLAW ADASZEWSKI nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL STANISLAW ADASZEWSKI BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */

#include "qnodeseditor.h"

#include <QGraphicsScene>
#include <QEvent>
#include <QGraphicsSceneMouseEvent>
#include <algorithm>

#include "qneport.h"
#include "qneconnection.h"
#include "qneblock.h"

QNodesEditor::QNodesEditor(QObject *parent) :
    QObject(parent)
{
	conn = 0;
}

void QNodesEditor::install(QGraphicsScene *s)
{
	s->installEventFilter(this);
	scene = s;
}

QGraphicsItem* QNodesEditor::itemAt(const QPointF &pos)
{
	QList<QGraphicsItem*> items = scene->items(QRectF(pos - QPointF(1,1), QSize(3,3)));

	for(QGraphicsItem *item : items)
		if (item->type() > QGraphicsItem::UserType)
			return item;

	return 0;
}

void QNodesEditor::addBlock(BlockType type, int x, int y)
{
	static int id_count = 0;

	QNEBlock *block = nullptr;

	if(type == ImageIn)
	{
		QNEBlock *imgIn = new QNEBlock;
		scene->addItem(imgIn);
		imgIn->addPort(tr("Image In"), 0, QNEPort::NamePort);
		imgIn->addPort(" ", 0, QNEPort::TypePort);
		imgIn->addOutputPort("out [img]");
		imgIn->setPos(x, y);

		imgIn->setID(id_count);
		imgIn->setType(ImageIn);
		id_count++;

		block = imgIn;
	}
	else if(type == ImageDisplay)
	{
		QNEBlock *imgDisp = new QNEBlock;
		scene->addItem(imgDisp);
		imgDisp->addPort(tr("Display Image"), 0, QNEPort::NamePort);
		imgDisp->addPort(" ", 0, QNEPort::TypePort);
		imgDisp->addInputPort("in [img]");
		imgDisp->setPos(x, y);

		imgDisp->setID(id_count);
		imgDisp->setType(ImageDisplay);
		id_count++;

		block = imgDisp;
	}
	else if(type == GaussianBlur)
	{
		QNEBlock *gaussianBlur = new QNEBlock;
		scene->addItem(gaussianBlur);
		gaussianBlur->addPort(tr("Gaussian Blur"), 0, QNEPort::NamePort);
		gaussianBlur->addPort(" ", 0, QNEPort::TypePort);
		gaussianBlur->addInputPort("in [img]");
		gaussianBlur->addInputPort("kernel width [uint]");
		gaussianBlur->addInputPort("kernel height [uint]");
		gaussianBlur->addPort("", 0, QNEPort::TypePort);
		gaussianBlur->addOutputPort("out [img]");
		gaussianBlur->setPos(x, y);

		gaussianBlur->setID(id_count);
		gaussianBlur->setType(GaussianBlur);
		id_count++;

		block = gaussianBlur;
	}
	else if(type == Number)
	{
		QNENumberBlock *number = new QNENumberBlock;
		scene->addItem(number);
		number->initialize(/*this*/);
		number->setPos(x, y);

		number->setID(id_count);
		number->setType(Number);
		id_count++;

		block = number;
	}

	if(block != nullptr) // Check if an item was actually added
	{
		for(INodesEditorListener *listener : listeners)
		{
			listener->nodeAdded(block);
		}
	}
}

bool QNodesEditor::eventFilter(QObject *o, QEvent *e)
{
	QGraphicsSceneMouseEvent *me = (QGraphicsSceneMouseEvent*) e;

	switch ((int) e->type())
	{
	case QEvent::GraphicsSceneMousePress:
	{

		switch ((int) me->button())
		{
		case Qt::LeftButton:
		{
			QGraphicsItem *item = itemAt(me->scenePos());
			if (item && item->type() == QNEPort::Type)
			{
                conn = new QNEConnection(0);
                scene->addItem(conn);
				conn->setPort1((QNEPort*) item);
				conn->setPos1(item->scenePos());
				conn->setPos2(me->scenePos());
				conn->updatePath();

				return true;
			} else if (item && item->type() == QNEBlock::Type)
			{
				/* if (selBlock)
					selBlock->setSelected(); */
				// selBlock = (QNEBlock*) item;
			}
			break;
		}
		case Qt::RightButton:
		{
			QGraphicsItem *item = itemAt(me->scenePos());
			if (item && (item->type() == QNEConnection::Type))
			{
				QNEConnection *conn = (QNEConnection *) item;
				QNEBlock *node1 = (QNEBlock *) conn->port1()->parentItem();
				QNEBlock *node2 = (QNEBlock *) conn->port2()->parentItem();

				for(INodesEditorListener *listener : listeners)
				{
					listener->connectionDeleted(node1, node2);
				}

				delete item;
			}
			else if(item && (item->type() == QNEBlock::Type))
			{
				QNEBlock *block = (QNEBlock *) item;
				for(INodesEditorListener *listener : listeners)
				{
					listener->nodeDeleted(block->id(), block->type());
				}


				delete item;
			}

			// if (selBlock == (QNEBlock*) item)
				// selBlock = 0;

			break;
		}
		}
		break;
	}
	case QEvent::GraphicsSceneMouseMove:
	{
		if (conn)
		{
			conn->setPos2(me->scenePos());
			conn->updatePath();
			return true;
		}
		break;
	}
	case QEvent::GraphicsSceneMouseRelease:
	{
		if (conn && me->button() == Qt::LeftButton)
		{
			QGraphicsItem *item = itemAt(me->scenePos());
			if (item && item->type() == QNEPort::Type)
			{
				QNEPort *port1 = conn->port1();
				QNEPort *port2 = (QNEPort*) item;

				if (port1->block() != port2->block() && port1->isOutput() != port2->isOutput() && !port1->isConnected(port2))
				{
					conn->setPos2(port2->scenePos());
					conn->setPort2(port2);
					conn->updatePath();
					conn = 0;

					QNEBlock *node1 = (QNEBlock *) port1->parentItem();
					QNEBlock *node2 = (QNEBlock *) port2->parentItem();

					for(INodesEditorListener *listener : listeners)
					{
						listener->connectionMade(node1, node2);
					}

					return true;
				}
			}

			delete conn;
			conn = 0;
			return true;
		}
		break;
	}
	}
	return QObject::eventFilter(o, e);
}

void QNodesEditor::addNodesEditorListener(INodesEditorListener *listener)
{
	listeners.push_back(listener);
}

void QNodesEditor::removeNodesEditorListener(INodesEditorListener *listener)
{
	listeners.erase(std::remove(listeners.begin(), listeners.end(), listener), listeners.end());
}

void QNodesEditor::save(QDataStream &ds)
{
	for(QGraphicsItem *item : scene->items())
		if (item->type() == QNEBlock::Type)
		{
			ds << item->type();
			((QNEBlock*) item)->save(ds);
		}

	for(QGraphicsItem *item : scene->items())
		if (item->type() == QNEConnection::Type)
		{
			ds << item->type();
			((QNEConnection*) item)->save(ds);
		}
}

void QNodesEditor::load(QDataStream &ds)
{
	scene->clear();

	QMap<quint64, QNEPort*> portMap;

	while (!ds.atEnd())
	{
		int type;
		ds >> type;
		if (type == QNEBlock::Type)
		{
            QNEBlock *block = new QNEBlock(0);
            scene->addItem(block);
			block->load(ds, portMap);
		} else if (type == QNEConnection::Type)
		{
            QNEConnection *conn = new QNEConnection(0);
            scene->addItem(conn);
			conn->load(ds, portMap);
		}
	}
}
