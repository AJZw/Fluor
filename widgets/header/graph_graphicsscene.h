/**** General **************************************************************
** Version:    v0.9.3
** Date:       2019-04-23
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

/**** LGPLv3 License *******************************************************
** graph_graphicsscene.h is part of Fluor
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
**
** 
***************************************************************************/

#ifndef GRAPH_GRAPHICSSCENE_H
#define GRAPH_GRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QWidget>
#include <QSize>

#include "graph_settings.h"
#include "graph_graphicsitems.h"

namespace Graph {

class GraphicsScene : public QGraphicsScene {
    Q_OBJECT

    public:
        explicit GraphicsScene(QWidget* parent = nullptr);
        GraphicsScene(const GraphicsScene &obj) = delete;
        GraphicsScene& operator=(const GraphicsScene &obj) = delete;
        GraphicsScene(GraphicsScene&&) = delete;
        GraphicsScene& operator=(GraphicsScene&&) = delete;
        ~GraphicsScene() = default;

    private:
        Graph::Settings settings;

        Graph::GraphBackground* item_background;

        Graph::AxisTitleX* item_x_axis_title;
        Graph::AxisTickLabelsX item_x_axis_labels;
        //Graph::AxisTicksX item_x_axis_ticks;
        Graph::Colorbar* item_x_colorbar;
        Graph::AxisLinesX item_x_axis_lines;

        Graph::AxisTitleY* item_y_axis_title;
        Graph::AxisTickLabelsY item_y_axis_labels;
        //Graph::AxisTicksY item_y_axis_ticks;
        Graph::AxisLinesY item_y_axis_lines;

        Graph::GraphOutline* item_outline;

        void calculateSizes(const QSize& rect);


    public slots:
        virtual void resizeScene(const QSize& rect);
        //void resizeScene(const QRectF& rect);

};

class GraphicsPlot : public GraphicsScene {
    Q_OBJECT

    public:
        explicit GraphicsPlot(QWidget* parent = nullptr);
        GraphicsPlot(const GraphicsPlot &obj) = delete;
        GraphicsPlot& operator=(const GraphicsPlot &obj) = delete;
        GraphicsPlot(GraphicsPlot&&) = delete;
        GraphicsPlot& operator=(GraphicsPlot&&) = delete;
        ~GraphicsPlot() = default;

    public slots:
        virtual void resizeScene(const QSize& rect);
};




} // Graph namespace

#endif // GRAPH_GRAPHICSSCENE_H