/**** General **************************************************************
** Version:    v0.9.3
** Date:       2019-04-27
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

/**** LGPLv3 License *******************************************************
** graph_graphicsview.h is part of Fluor
**        
** Fluor is free software: you can redistribute it and/or
** modify it under the terms of the Lesser GNU General Public License as
** published by the Free Software Foundation, either version 3 of the
** License, or (at your option) any later version.
** 
** Fluor is distributed in the hope that it will be useful, but
** WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the Lesser
** GNU General Public License for more details.
** 
** You should have received a copy of the Lesser GNU General Public License
** along with Fluor. If not, see <https://www.gnu.org/licenses/>.  
***************************************************************************/

/**** DOC ******************************************************************
** The view into a graph. Receives outside events
** 
** :class: Graph::GraphicsView
** Receives and forwards outside events towards the scene
**
***************************************************************************/

#ifndef GRAPH_GRAPHICSVIEW_H
#define GRAPH_GRAPHICSVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QWidget>
#include <QResizeEvent>
#include <QRect>

//#include <QRectF>

namespace Graph {

class GraphicsView : public QGraphicsView {
    Q_OBJECT

    public:
        explicit GraphicsView(QGraphicsScene* scene, QWidget* parent = nullptr);
        GraphicsView(const GraphicsView &obj) = delete;
        GraphicsView& operator=(const GraphicsView &obj) = delete;
        GraphicsView(GraphicsView&&) = delete;
        GraphicsView& operator=(GraphicsView&&) = delete;
        ~GraphicsView() = default;

    private:
        void resizeEvent(QResizeEvent* event);
        bool eventFilter(QObject* obj, QEvent* event);

    signals:
        void resizedView(const QSize space);

};


} // Graph namespace

#endif // GRAPH_GRAPHICSVIEW_H