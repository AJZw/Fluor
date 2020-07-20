/**** General **************************************************************
** Version:    v0.9.3
** Date:       2019-04-23
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include "graph_graphicsview.h"
#include "graph_format.h"

#include <QSize>
#include <QDebug>
#include <QResizeEvent>

namespace Graph {

GraphicsView::GraphicsView(QGraphicsScene* scene, QWidget* parent) :
    QGraphicsView(scene, parent)
{
    // Set scene rect
    //this->setSceneRect(0, 0, 100, 100);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    this->setMouseTracking(true);
}

void GraphicsView::resizeEvent(QResizeEvent* event){
    // Get new size
    QSize event_size = event->size();

    // Set view size
    this->setSceneRect(0, 0, event_size.width(), event_size.height());

    // Connect to scene (to reposition items)
    emit this->resizedView(event_size);

    // Propagate
    QGraphicsView::resizeEvent(event);
}

} // Graph namespace
