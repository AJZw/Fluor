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
** The GraphicsItems for a graph
**
** :class: Graph::Axis::AbstractLabel
** The abstract graphicsitem for a title label on an axis
**
** :class: Graph::Axis::LabelX
** A text graphicsitem for a horizontal (x) title label
**
** :class: Graph::Axis::LabelY
** A ltext graphicsitem for a vertical (y) title label
**
** :class: Graph::Axis::GridLine
** A line graphicsitem with the properties of a gridline
**
** :class: Graph::Axis::AbstractGridLines
** A class holding multiple GridLine graphicsitem for distribution over a region
**
** :class: Graph::Axis::TicksX
** A class holding multiple GridLine line graphicsitem for horizontal (x) tick distribution over a region
**
** :class: Graph::Axis::TicksY
** A class holding multiple GridLine line graphicsitem for horizontal (y) tick distribution over a region
**
** :class: Graph::Axis::GridLinesX
** A class holding multiple GridLine line graphicsitem for horizontal (x) gridline distribution over a region
**
** :class: Graph::Axis::GridLinesY
** A class holding multiple GridLine line graphicsitem for vertical (y) gridline distribution over a region
**
** :class: Graph::Axis::GridLabel
** A class storing properties of a grid label
**
** :class: Graph::Axis::AbstractGridLabels
** A abstract class holding multiple GridLabel graphicsitem for distribution over a region
**
** :class: Graph::Axis::GridLabelsX
** A class holding multiple GridLabel text graphicsitem for horizontal (x) ticklabel distribution over a region
**
** :class: Graph::Axis::GridLabelsY
** A class holding multiple GridLabel text graphicsitem for vertical (y) ticklabel distribution over a region
**
** :class: Graph::Background
** A rectangle graphicsitem for background coloring
**
** :class: Graph::Outline
** A rectangle graphicsitem for outlining the background. This is plotting on top, thereby hiding imperfections
**
** :class: Graph::Colorbar
** A rectangle for the horizontal (x) axis. Shows the human visible colorspectrum of light
**
** :class: Graph::Spectrum
** A graphicsitem class for painting, and contain-detection of excitation/emission curves of one fluorophore
**
** :class: Graph::Spectra
** A storage class holding multiple Spectrum classes
**
***************************************************************************/

#ifndef GRAPH_GRAPHICSITEMS_H
#define GRAPH_GRAPHICSITEMS_H

#include "graph_format.h"
#include "data_spectrum.h"
#include "cache.h"

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

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>

namespace Graph {

namespace Axis {

class AbstractLabel : public QGraphicsSimpleTextItem {
    public:
        explicit AbstractLabel(const QString& text, QGraphicsItem* parent = nullptr);
        AbstractLabel(const AbstractLabel &obj) = delete;
        AbstractLabel& operator=(const AbstractLabel &obj) = delete;
        AbstractLabel(AbstractLabel&&) = delete;
        AbstractLabel& operator=(AbstractLabel&&) = delete;
        virtual ~AbstractLabel() = default;

    protected:
        QMargins item_margins;
        int minimum_width;
        int minimum_height;

     public:
        virtual void calculateMinimumSize() = 0;        // Pure virtual <- implement
        
        void setMargins(int left, int top, int right, int bottom);
        const QMargins& margins() const;

        void setFont(const QFont& font);
        void setText(const QString& text);

        int minimumWidth() const;
        int minimumHeight() const;

        virtual void setPosition(const QRectF& space) = 0;   // Pure virtual <- implement
};

class LabelX : public Axis::AbstractLabel {
    public:
        explicit LabelX(const QString& text, QGraphicsItem* parent = nullptr);
        LabelX(const LabelX &obj) = delete;
        LabelX& operator=(const LabelX &obj) = delete;
        LabelX(LabelX&&) = delete;
        LabelX& operator=(LabelX&&) = delete;
        virtual ~LabelX() = default;

    public:
        void calculateMinimumSize() override;
        void setPosition(const QRectF& space) override;
};

class LabelY : public Axis::AbstractLabel {
    public:
        explicit LabelY(const QString& text, QGraphicsItem* parent = nullptr);
        LabelY(const LabelY &obj) = delete;
        LabelY& operator=(const LabelY &obj) = delete;
        LabelY(LabelY&&) = delete;
        LabelY& operator=(LabelY&&) = delete;
        virtual ~LabelY() = default;

    public:
        void calculateMinimumSize() override;
        void setPosition(const QRectF& space) override;
};

class GridLine : public QGraphicsLineItem {
    public:
        explicit GridLine(int location, QGraphicsItem* parent=nullptr);
        GridLine(const GridLine &obj) = delete;
        GridLine& operator=(const GridLine &obj) = delete;
        GridLine(GridLine&&) = delete;
        GridLine& operator=(GridLine&&) = delete;
        ~GridLine() = default;

        void setLocation(int location);
        int location() const;

    private:
        int line_location;
};

class AbstractGridLines : public QGraphicsItem {
    public:
        explicit AbstractGridLines(QGraphicsItem* parent=nullptr);
        AbstractGridLines(const AbstractGridLines&) = delete;
        AbstractGridLines& operator=(const AbstractGridLines&) = delete;
        AbstractGridLines(AbstractGridLines&&) = delete;
        AbstractGridLines& operator=(AbstractGridLines&&) = delete;
        virtual ~AbstractGridLines() = default;

    protected:
        std::vector<Axis::GridLine*> items;         // Have to be in ordered position

        QMargins item_margins;
        int line_length;

        int minimum_width;
        int minimum_height;
    
    public:
        virtual void calculateMinimumSize() = 0;         // Pure virtual <- implement

        virtual QRectF boundingRect() const override;
        virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

        void setMargins(int left, int top, int right, int bottom);
        const QMargins& margins() const;

        int minimumWidth() const;
        int minimumHeight() const;

        void setFont(const QFont& font);

        virtual void setPosition(const Graph::Format::Settings& settings, const QRectF& space) = 0;      // pure virtual <- implement
        virtual void setLines(const Graph::Format::Settings& settings) = 0;                              // pure virutal <- implement
};

class TicksX : public Axis::AbstractGridLines {
    public:
        explicit TicksX(QGraphicsItem* parent=nullptr);
        TicksX(const TicksX &obj) = delete;
        TicksX& operator=(const TicksX &obj) = delete;
        TicksX(TicksX&&) = delete;
        TicksX& operator=(TicksX&&) = delete;
        virtual ~TicksX() = default;

    public:
        void calculateMinimumSize() override;
 
        void setPosition(const Graph::Format::Settings& settings, const QRectF& space) override;
        void setLines(const Graph::Format::Settings& settings) override;
};

class TicksY : public Axis::AbstractGridLines {
    public:
        explicit TicksY(QGraphicsItem* parent=nullptr);
        TicksY(const TicksY &obj) = delete;
        TicksY& operator=(const TicksY &obj) = delete;
        TicksY(TicksY&&) = delete;
        TicksY& operator=(TicksY&&) = delete;
        virtual ~TicksY() = default;

    public:
        void calculateMinimumSize() override;

        void setPosition(const Graph::Format::Settings& settings, const QRectF& space) override;
        void setLines(const Graph::Format::Settings& settings) override;
};

class GridLinesX : public Axis::AbstractGridLines {
    public:
        explicit GridLinesX(QGraphicsItem* parent=nullptr);
        GridLinesX(const GridLinesX &obj) = delete;
        GridLinesX& operator=(const GridLinesX &obj) = delete;
        GridLinesX(GridLinesX&&) = delete;
        GridLinesX& operator=(GridLinesX&&) = delete;
        virtual ~GridLinesX() = default;
    
    public:
        void calculateMinimumSize() override;

        void setPosition(const Graph::Format::Settings& settings, const QRectF& space) override;
        void setLines(const Graph::Format::Settings& settings) override;
};

class GridLinesY : public Axis::AbstractGridLines {
    public:
        explicit GridLinesY(QGraphicsItem* parent=nullptr);
        GridLinesY(const GridLinesY &obj) = delete;
        GridLinesY& operator=(const GridLinesY &obj) = delete;
        GridLinesY(GridLinesY&&) = delete;
        GridLinesY& operator=(GridLinesY&&) = delete;
        virtual ~GridLinesY() = default;
    
    public:
        void calculateMinimumSize();

        void setPosition(const Graph::Format::Settings& settings, const QRectF& space);
        void setLines(const Graph::Format::Settings& settings);
};

class GridLabel : public QGraphicsSimpleTextItem {
    public:
        explicit GridLabel(int location, QString label, QGraphicsItem* parent=nullptr);
        GridLabel(const GridLabel &obj) = delete;
        GridLabel& operator=(const GridLabel &obj) = delete;
        GridLabel(GridLabel&&) = delete;
        GridLabel& operator=(GridLabel&&) = delete;
        ~GridLabel() = default;

        void setLocation(int location);
        int location() const;

    private:
        int label_location;
};

class AbstractGridLabels : public QGraphicsItem {
    public:
        explicit AbstractGridLabels(QGraphicsItem* parent=nullptr);
        AbstractGridLabels(const AbstractGridLabels &obj) = delete;
        AbstractGridLabels& operator=(const AbstractGridLabels &obj) = delete;
        AbstractGridLabels(AbstractGridLabels&&) = delete;
        AbstractGridLabels& operator=(AbstractGridLabels&&) = delete;
        virtual ~AbstractGridLabels() = default;

    protected:
        std::vector<GridLabel*> items;

        QMargins item_margins;
        int space_offset;

        int minimum_width;
        int minimum_height;

    public:
        virtual void calculateMinimumSize() = 0;            // pure virtual <- implement

        virtual QRectF boundingRect() const override;
        virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

        void setMargins(int left, int top, int right, int bottom);
        const QMargins& margins() const;

        int minimumWidth() const;
        int minimumHeight() const;

        void setFont(const QFont& font);

        virtual void setPosition(const Graph::Format::Settings& settings, const QRectF& space) = 0;     // pure virtual <- implement
        virtual void setLabels(const Graph::Format::Settings& settings) = 0;                            // pure virtual <- implement
};

class GridLabelsX : public Axis::AbstractGridLabels {
    public:
        explicit GridLabelsX(QGraphicsItem* parent=nullptr);
        GridLabelsX(const GridLabelsX &obj) = delete;
        GridLabelsX& operator=(const GridLabelsX &obj) = delete;
        GridLabelsX(GridLabelsX&&) = delete;
        GridLabelsX& operator=(GridLabelsX&&) = delete;
        virtual ~GridLabelsX() = default;

    public:
        void calculateMinimumSize() override;

        void setPosition(const Graph::Format::Settings& settings, const QRectF& space) override;
        void setLabels(const Graph::Format::Settings& settings) override;
};

class GridLabelsY : public Axis::AbstractGridLabels {
    public:
        explicit GridLabelsY(QGraphicsItem* parent=nullptr);
        GridLabelsY(const GridLabelsY &obj) = delete;
        GridLabelsY& operator=(const GridLabelsY &obj) = delete;
        GridLabelsY(GridLabelsY&&) = delete;
        GridLabelsY& operator=(GridLabelsY&&) = delete;
        virtual ~GridLabelsY() = default;

    public:
        void calculateMinimumSize() override;

        void setPosition(const Graph::Format::Settings& settings, const QRectF& space) override;
        void setLabels(const Graph::Format::Settings& settings) override;
};

} // Axis namespace

class Background : public QGraphicsRectItem {
    public:
        explicit Background(QGraphicsItem* parent = nullptr);
        Background(const Background &obj) = delete;
        Background& operator=(const Background &obj) = delete;
        Background(Background&&) = delete;
        Background& operator=(Background&&) = delete;
        ~Background() = default;

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

class Outline : public QGraphicsRectItem {
    public:
        explicit Outline(QGraphicsItem* parent = nullptr);
        Outline(const Outline &obj) = delete;
        Outline& operator=(const Outline &obj) = delete;
        Outline(Outline&&) = delete;
        Outline& operator=(Outline&&) = delete;
        ~Outline() = default;

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

        void setPosition(const Graph::Format::Settings& settings, const QRectF& space);
};

class Spectrum : public QGraphicsItem {
    public:
        explicit Spectrum(const Data::Spectrum* data, QGraphicsItem* parent=nullptr);
        Spectrum(const Spectrum &obj) = delete;
        Spectrum& operator=(const Spectrum &obj) = delete;
        Spectrum(Spectrum&&) = delete;
        Spectrum& operator=(Spectrum&&) = delete;
        virtual ~Spectrum() = default;

    private:
        const Data::Spectrum* spectrum_source;
        Data::Polygon spectrum_excitation;
        Data::Polygon spectrum_emission;
        Data::Polygon spectrum_emission_fill;

        QRectF spectrum_space;
        QMargins item_margins;

        int minimum_width;
        int minimum_height;

        bool visible_excitation;
        bool visible_emission;
        double intensity_coefficient;
    
    public:
        virtual QRectF boundingRect() const override;
        virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
        
        void setMargins(int left, int top, int right, int bottom);
        const QMargins& margins() const;

        int minimumWidth() const;
        int minimumHeight() const;

        void calculateMinimumSize();
        void setPosition(const Graph::Format::Settings& settings, const QRectF& space);
        void updateSpectrum(bool visible_excitation, bool visible_emission);

        const Data::Spectrum* source() const;
};

class Spectra : public QGraphicsItem {
    public:
        explicit Spectra(QGraphicsItem* scene=nullptr);
        Spectra(const Spectra &obj) = delete;
        Spectra& operator=(const Spectra &obj) = delete;
        Spectra(Spectra&&) = delete;
        Spectra& operator=(Spectra&&) = delete;
        virtual ~Spectra() = default;

    private:
        std::vector<Graph::Spectrum*> spectra_items;

        int minimum_width;
        int minimum_height;

        QRectF spectra_space;

    public:
        virtual QRectF boundingRect() const override;
        virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
        
        std::size_t size() const;

        int minimumWidth() const;
        int minimumHeight() const;

        void calculateMinimumSize();
        void setPosition(const Graph::Format::Settings& settings, const QRectF& space);

        void syncSpectra(const std::vector<Cache::CacheID>& cache_state, const Graph::Format::Settings& settings);
        void updateSpectra(const std::vector<Cache::CacheID>& cache_state);

    private:
        std::size_t findIndex(const Data::Spectrum* id, std::size_t index_start) const;
};

} // Graph namespace

#endif // GRAPH_GRAPHICSITEMS_H