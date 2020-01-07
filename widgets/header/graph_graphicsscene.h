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
** The graphicsscene object of a graph
**
** :class: Graph::GraphicsScene
** The graphicsscene holding all QGraphicsItem of a single plot
** 
***************************************************************************/

#ifndef GRAPH_GRAPHICSSCENE_H
#define GRAPH_GRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>
#include <QWidget>
#include <QSize>
#include <QBrush>

#include "graph_format.h"
#include "graph_graphicsitems.h"

namespace Graph {

class GraphicsScene : public QGraphicsScene {
    Q_OBJECT

    public:
        explicit GraphicsScene(Graph::Format::Settings settings=Graph::Format::Settings(), QWidget* parent = nullptr);
        GraphicsScene(const GraphicsScene &obj) = delete;
        GraphicsScene& operator=(const GraphicsScene &obj) = delete;
        GraphicsScene(GraphicsScene&&) = delete;
        GraphicsScene& operator=(GraphicsScene&&) = delete;
        virtual ~GraphicsScene();

    private:
        Graph::Format::Settings settings;
        Graph::PlotRectF plot_rect;

        Graph::Background* item_background;
        Graph::Axis::LabelX* item_x_axis_label;
        Graph::Axis::GridLabelsX* item_x_axis_gridlabels;
        Graph::Axis::TicksX* item_x_axis_ticks;
        Graph::Axis::GridLinesX* item_x_axis_gridlines;
        Graph::Colorbar* item_x_colorbar;
        Graph::Axis::LabelY* item_y_axis_label;
        Graph::Axis::GridLabelsY* item_y_axis_gridlabels;
        Graph::Axis::TicksY* item_y_axis_ticks;
        Graph::Axis::GridLinesY* item_y_axis_gridlines;
        Graph::SpectrumCollection* item_spectra;
        Graph::LaserCollection* item_lasers;
        Graph::DetectorCollection* item_detectors;
        Graph::Outline* item_outline;

        // Keeps track of scroll rotation to be able to scroll through the spectrum items
        std::size_t scroll_count;
        // Keep track of current size for style/dpi changes
        QSize size_current;

    private:
        void calculateSizes(const QSize& rect);

    public slots:
        virtual void resizeScene(const QSize& rect);
        void sync(const std::vector<Cache::CacheID>& cache_state);
        void update(const std::vector<Cache::CacheID>& cache_state);
        void updatePainter(const Graph::Format::Style* style);

        void selectSpectrum(const QPointF& point, std::size_t index=0);

    protected slots:
        virtual void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
        virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
        virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
        virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

        virtual void wheelEvent(QGraphicsSceneWheelEvent* event) override;

    signals:
        void spectrumSelected();
};

} // Graph namespace

#endif // GRAPH_GRAPHICSSCENE_H