/**** General **************************************************************
** Version:    v0.9.3
** Date:       2019-04-23
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

/**** LGPLv3 License *******************************************************
** graph_graphicsitems.h is part of Fluor
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

#ifndef GRAPH_GRAPHICSITEMS_H
#define GRAPH_GRAPHICSITEMS_H

#include "graph_settings.h"
#include <QGraphicsScene>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsItem>
#include <QString>
#include <QMargins>
#include <QFont>
#include <QRectF>
#include <QObject>
#include <QPen>
#include <QColor>

namespace Graph {

class AxisTitleX : public QGraphicsSimpleTextItem {
    public:
        explicit AxisTitleX(const QString& text, QGraphicsItem* parent = nullptr);
        AxisTitleX(const AxisTitleX &obj) = delete;
        AxisTitleX& operator=(const AxisTitleX &obj) = delete;
        AxisTitleX(AxisTitleX&&) = delete;
        AxisTitleX& operator=(AxisTitleX&&) = delete;
        ~AxisTitleX() = default;

    private:
        QMargins item_margins;
        int minimum_width;      // minimum_width and height are precalculated to safe calculations during resizing
        int minimum_height;

        void calculateMinimumSize();

    public:
        void setMargins(int left, int top, int right, int bottom);
        const QMargins& margins() const;

        void setFont(const QFont& font);
        void setText(const QString& text);

        int minimumWidth() const;
        int minimumHeight() const;

        void setPosition(const QRectF& space);

};

class AxisTitleY : public QGraphicsSimpleTextItem {
    public:
        explicit AxisTitleY(const QString& text, QGraphicsItem* parent = nullptr);
        AxisTitleY(const AxisTitleY &obj) = delete;
        AxisTitleY& operator=(const AxisTitleY &obj) = delete;
        AxisTitleY(AxisTitleY&&) = delete;
        AxisTitleY& operator=(AxisTitleY&&) = delete;
        ~AxisTitleY() = default;

    private:
        QMargins item_margins;
        int minimum_width;      // minimum_width and height are precalculated to safe calculations during resizing
        int minimum_height;

        void calculateMinimumSize();

    public:
        void setMargins(int left, int top, int right, int bottom);
        const QMargins& margins() const;

        void setFont(const QFont& font);
        void setText(const QString& text);

        int minimumWidth() const;
        int minimumHeight() const;

        void setPosition(const QRectF& space);

};

class TickLine : public QGraphicsLineItem {
    public:
        explicit TickLine(int location, QGraphicsItem* parent=nullptr);
        TickLine(const TickLine &obj) = delete;
        TickLine& operator=(const TickLine &obj) = delete;
        TickLine(TickLine&&) = delete;
        TickLine& operator=(TickLine&&) = delete;
        ~TickLine() = default;

        void setLocation(int location);
        int location() const;

    private:
        int tick_location;

};

class AxisTicksX : public QObject {
    public:
        explicit AxisTicksX(QGraphicsScene* scene);
        AxisTicksX(const AxisTicksX &obj) = delete;
        AxisTicksX& operator=(const AxisTicksX &obj) = delete;
        AxisTicksX(AxisTicksX&&) = delete;
        AxisTicksX& operator=(AxisTicksX&&) = delete;
        ~AxisTicksX() = default;

    protected:
        QGraphicsScene* graphics_scene;
        std::vector<TickLine*> items;

        QMargins item_margins;
        int tick_length;      

        int minimum_width;
        int minimum_height;

        void calculateMinimumSize();

    public:
        void setMargins(int left, int top, int right, int bottom);
        const QMargins& margins() const;

        int minimumWidth() const;
        int minimumHeight() const;

        void setPosition(const Graph::Settings& settings, const QRectF& space);
        void setTicks(const Graph::Settings& settings);
};

class AxisTicksY : public QObject {
    public:
        explicit AxisTicksY(QGraphicsScene* scene);
        AxisTicksY(const AxisTicksY &obj) = delete;
        AxisTicksY& operator=(const AxisTicksY &obj) = delete;
        AxisTicksY(AxisTicksY&&) = delete;
        AxisTicksY& operator=(AxisTicksY&&) = delete;
        ~AxisTicksY() = default;

    protected:
        QGraphicsScene* graphics_scene;
        std::vector<TickLine*> items;

        QMargins item_margins;
        int tick_length;

        int minimum_width;
        int minimum_height;

        void calculateMinimumSize();

    public:
        void setMargins(int left, int top, int right, int bottom);
        const QMargins& margins() const;

        int minimumWidth() const;
        int minimumHeight() const;

        void setPosition(const Graph::Settings& settings, const QRectF& space);
        void setTicks(const Graph::Settings& settings);
};

class AxisLinesX : public AxisTicksX {
    public:
        explicit AxisLinesX(QGraphicsScene* scene);
        AxisLinesX(const AxisLinesX &obj) = delete;
        AxisLinesX& operator=(const AxisLinesX &obj) = delete;
        AxisLinesX(AxisLinesX&&) = delete;
        AxisLinesX& operator=(AxisLinesX&&) = delete;
        ~AxisLinesX() = default;
    
    public:
        void setPosition(const Graph::Settings& settings, const QRectF& space);
};

class AxisLinesY : public AxisTicksY {
    public:
        explicit AxisLinesY(QGraphicsScene* scene);
        AxisLinesY(const AxisLinesY &obj) = delete;
        AxisLinesY& operator=(const AxisLinesY &obj) = delete;
        AxisLinesY(AxisLinesY&&) = delete;
        AxisLinesY& operator=(AxisLinesY&&) = delete;
        ~AxisLinesY() = default;
    
    public:
        void setPosition(const Graph::Settings& settings, const QRectF& space);
};

class TickLabel : public QGraphicsSimpleTextItem {
    public:
        explicit TickLabel(int location, QString label, QGraphicsItem* parent=nullptr);
        TickLabel(const TickLabel &obj) = delete;
        TickLabel& operator=(const TickLabel &obj) = delete;
        TickLabel(TickLabel&&) = delete;
        TickLabel& operator=(TickLabel&&) = delete;
        ~TickLabel() = default;

        void setLocation(int location);
        int location() const;

    private:
        int tick_location;
};

class AxisTickLabelsX : public QObject {
    public:
        explicit AxisTickLabelsX(QGraphicsScene* scene);
        AxisTickLabelsX(const AxisTickLabelsX &obj) = delete;
        AxisTickLabelsX& operator=(const AxisTickLabelsX &obj) = delete;
        AxisTickLabelsX(AxisTickLabelsX&&) = delete;
        AxisTickLabelsX& operator=(AxisTickLabelsX&&) = delete;
        ~AxisTickLabelsX() = default;

    private:
        QGraphicsScene* graphics_scene;
        std::vector<TickLabel*> items;

        QMargins item_margins;
        const int space_offset;

        int minimum_width;
        int minimum_height;

        void calculateMinimumSize(const QFont& font, const QString& text);

    public:
        void setMargins(int left, int top, int right, int bottom);
        const QMargins& margins() const;

        int minimumWidth() const;
        int minimumHeight() const;

        void setPosition(const Graph::Settings& settings, const QRectF& space);
        void setTicks(const Graph::Settings& settings);
};

class AxisTickLabelsY : public QObject {
    public:
        explicit AxisTickLabelsY(QGraphicsScene* scene);
        AxisTickLabelsY(const AxisTickLabelsY &obj) = delete;
        AxisTickLabelsY& operator=(const AxisTickLabelsY &obj) = delete;
        AxisTickLabelsY(AxisTickLabelsY&&) = delete;
        AxisTickLabelsY& operator=(AxisTickLabelsY&&) = delete;
        ~AxisTickLabelsY() = default;

    private:
        QGraphicsScene* graphics_scene;
        std::vector<TickLabel*> items;

        QMargins item_margins;
        const int space_offset;

        int minimum_width;
        int minimum_height;

        void calculateMinimumSize(const QFont& font, const QString& text);

    public:
        void setMargins(int left, int top, int right, int bottom);
        const QMargins& margins() const;

        int minimumWidth() const;
        int minimumHeight() const;

        void setPosition(const Graph::Settings& settings, const QRectF& space);
        void setTicks(const Graph::Settings& settings);
};

class GraphBackground : public QGraphicsRectItem {
    public:
        explicit GraphBackground(QGraphicsItem* parent = nullptr);
        GraphBackground(const GraphBackground &obj) = delete;
        GraphBackground& operator=(const GraphBackground &obj) = delete;
        GraphBackground(GraphBackground&&) = delete;
        GraphBackground& operator=(GraphBackground&&) = delete;
        ~GraphBackground() = default;

    private:
        QMargins item_margins;
        int minimum_width;
        int minimum_height;

    public:
        void setMargins(int left, int top, int right, int bottom);
        const QMargins& margins() const;

        int minimumWidth() const;
        int minimumHeight() const;

        void setPosition(const QRectF& space);
};

class GraphOutline : public QGraphicsRectItem {
    public:
        explicit GraphOutline(QGraphicsItem* parent = nullptr);
        GraphOutline(const GraphOutline &obj) = delete;
        GraphOutline& operator=(const GraphOutline &obj) = delete;
        GraphOutline(GraphOutline&&) = delete;
        GraphOutline& operator=(GraphOutline&&) = delete;
        ~GraphOutline() = default;

    private:
        QMargins item_margins;
        int minimum_width;
        int minimum_height;

    public:
        void setMargins(int left, int top, int right, int bottom);
        const QMargins& margins() const;

        int minimumWidth() const;
        int minimumHeight() const;

        void setPosition(const QRectF& space);
};

class Colorbar : public QGraphicsRectItem {
    public:
        explicit Colorbar(QGraphicsItem* parent = nullptr);
        Colorbar(const Colorbar &obj) = delete;
        Colorbar& operator=(const Colorbar &obj) = delete;
        Colorbar(Colorbar&&) = delete;
        Colorbar& operator=(Colorbar&&) = delete;
        ~Colorbar() = default;

    private:
        QMargins item_margins;
        int minimum_width;
        int minimum_height;

        QLinearGradient gradient;

    public:
        void setMargins(int left, int top, int right, int bottom);
        const QMargins& margins() const;

        int minimumWidth() const;
        int minimumHeight() const;

        void setPosition(const Graph::Settings& settings, const QRectF& space);
        //void setSpectrum(const Graph::Settings& settings);
};


} // Graph namespace

#endif // GRAPH_GRAPHICSITEMS_H