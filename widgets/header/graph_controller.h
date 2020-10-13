/**** General **************************************************************
** Version:    v0.9.10
** Date:       2020-10-13
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

/**** LGPLv3 License *******************************************************
** graph_controller.h is part of Fluor
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
** Controller of a graph
**
** :class: Graph::Controller
** Controls the signal and slots of a graph widget
**
** :class: Graph::ScrollController
** Controls the showing / scrolling of multiple Graph::Controller widgets
** 
***************************************************************************/

#ifndef GRAPH_CONTROLLER_H
#define GRAPH_CONTROLLER_H

#include <cache.h>
#include "graph_graphicsscene.h"
#include "graph_graphicsview.h"
#include "data_instruments.h"
#include "state_gui.h"

#include <QWidget>
#include <QScrollArea>
#include <QPaintEvent>

namespace Graph {

class Controller : public QWidget {
    Q_OBJECT

    public:
        explicit Controller(QWidget* parent = nullptr);
        Controller(const Controller &obj) = delete;
        Controller& operator=(const Controller &obj) = delete;
        Controller(Controller&&) = delete;
        Controller& operator=(Controller&&) = delete;
        ~Controller() = default;
        
        void paintEvent(QPaintEvent* event);

    private:
        Graph::GraphicsScene* graphics_scene;
        Graph::GraphicsView* graphics_view;
        Graph::Format::Style* graphics_style;

    signals:
        void sendGlobalEvent(QEvent* event);

        void sendCacheState(const std::vector<Cache::ID>& cache_state);
        void sendCacheUpdate();
        void sendCacheRequestUpdate();

        void sendGraphSelect(const Controller* graph, bool state);
        void sendGraphState(const State::GraphState& state);

        void sendPainterUpdate(const Graph::Format::Style* style);

    public slots:
        void receiveGlobalEvent(QEvent* event);

        void receiveCacheState(const std::vector<Cache::ID>& cache_state);
        void receiveCacheUpdate();

        void setSelect(bool state);
        void receivePlotSelected(bool state);
        void receiveGraphState(const State::GraphState& state);

        void receiveStyleChanged();
};

class ScrollController : public QScrollArea {
    Q_OBJECT
    Q_PROPERTY(QString layout_spacing READ layoutSpacing WRITE setLayoutSpacing)
    Q_PROPERTY(QString layout_margins_scroll_bar READ layoutMarginsScrollBar WRITE setLayoutMarginsScrollBar)

    public:
        explicit ScrollController(QWidget* parent = nullptr);
        ScrollController(const ScrollController &obj) = delete;
        ScrollController& operator=(const ScrollController &obj) = delete;
        ScrollController(ScrollController&&) = delete;
        ScrollController& operator=(ScrollController&&) = delete;
        ~ScrollController() = default;
        
        QString layoutSpacing() const;
        void setLayoutSpacing(QString layout_spacing);
        QString layoutMarginsScrollBar() const;
        void setLayoutMarginsScrollBar(QString layout_spacing_scroll_bar);
    
    private:
        std::vector<Controller*> graph_widgets;
        Controller* graph_selected;
        int margin_scrollbar;
        int column_max;

    private:
        void addGraph();
        void removeGraph();
        void rebuildLayout();
    
    signals:
        void sendGlobalEvent(QEvent* event);
        void sendCacheState(const std::vector<Cache::ID>& cache_state);
        void sendCacheUpdate();
        void sendCacheRequestUpdate();

        void sendGraphSelect(std::size_t index, bool state);

    private slots:
        void hidingScrollBar();
        void showingScrollBar();

    public slots:
        void receiveGlobalEvent(QEvent* event);
        void receiveCacheState(const std::vector<Cache::ID>& cache_state);
        void receiveCacheUpdate();
        void receiveCacheRequestUpdate();

        void receiveGraphState(std::vector<State::GraphState>& state);
        void receiveGraphSelect(const Controller* graph, bool state);
};

} // Graph namespace

#endif // GRAPH_CONTROLLER_H
