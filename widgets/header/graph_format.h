/**** General **************************************************************
** Version:    v0.9.3
** Date:       2019-07-24
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

/**** LGPLv3 License *******************************************************
** graph_format.h is part of Fluor
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
** :class: Graph::Format::Axis
** Storage class for the properties a single axis
**
** :class: Graph::Format::AxisRange
** Storage class for the variable range of an axis
**
** :class: Graph::Format::Tick
** Storage class for the properties of a Tick
**
** :class: Graph::Format::Ticks
** Storage class for the range of Ticks of a single axis
**
** :class: Graph::Format::Settings
** Storage class for all axis data of a graph
**
** :class: Graph::Format::Style
** Storage class for the pens/brushes as used during painting.
** Receives input from QSS stylesheet using QProperties
**
***************************************************************************/

#ifndef GRAPH_FORMAT_H
#define GRAPH_FORMAT_H

#include <array>

#include <Qt>
#include <QWidget>
#include <QString>
#include <QColor>
#include <QFont>
#include <QBrush>
#include <QPen>

namespace Graph {

namespace Format {

struct Axis {
    explicit Axis(int min, int max, QString label);
    Axis(const Axis &obj) = default;
    Axis& operator=(const Axis &obj) = default;
    Axis(Axis&&) = default;
    Axis& operator=(Axis&&) = default;
    ~Axis() = default;

    const int min;
    const int max;
    const QString label;
};

struct AxisRange {
    explicit AxisRange(int begin, int end);
    AxisRange(const AxisRange &obj) = default;
    AxisRange& operator=(const AxisRange &obj) = default;
    AxisRange(AxisRange&&) = default;
    AxisRange& operator=(AxisRange&&) = default;
    ~AxisRange() = default;

    int begin;                      // in global values
    int end;                        // in global values
    const int default_begin;        // in global values
    const int default_end;          // in global values
};

struct Tick {
    explicit Tick(int location);
    explicit Tick(int location, QString label);
    Tick(const Tick &obj) = default;
    Tick& operator=(const Tick &obj) = default;
    Tick(Tick&&) = default;
    Tick& operator=(Tick&&) = default;
    ~Tick() = default;

    const int location;
    const QString label;
};

template<std::size_t TICK_COUNT>
struct Ticks {
    Ticks(std::array<Format::Tick, TICK_COUNT> const& ticks)  :
        valid(false), index_begin(0), index_end(0),
        ticks(ticks)
    {};
    Ticks(const Ticks &obj) = default;
    Ticks& operator=(const Ticks &obj) = default;
    Ticks(Ticks&&) = default;
    Ticks& operator=(Ticks&&) = default;
    ~Ticks() = default;

    bool valid;
    std::size_t index_begin;
    std::size_t index_end;      // Technically the index after the last relevant tick index, so can be out-of-bounds
    const std::array<Format::Tick, TICK_COUNT> ticks;

    void findIndexes(int begin, int end);
};

struct Settings {
    Settings();
    Settings(const Settings &obj) = default;
    Settings& operator=(const Settings &obj) = default;
    Settings(Settings&&) = default;
    Settings& operator=(Settings&&) = default;
    ~Settings() = default;

    const Axis x_axis = Format::Axis(0, 1500, QString("Wavelength (nm)"));
    AxisRange x_range = Format::AxisRange(300, 900);
    Ticks<31> x_ticks = {{
        Format::Tick(0, "0"), Format::Tick(50),
        Format::Tick(100, "100"), Format::Tick(150), 
        Format::Tick(200, "200"), Format::Tick(250), 
        Format::Tick(300, "300"), Format::Tick(350), 
        Format::Tick(400, "400"), Format::Tick(450), 
        Format::Tick(500, "500"), Format::Tick(550), 
        Format::Tick(600, "600"), Format::Tick(650), 
        Format::Tick(700, "700"), Format::Tick(750), 
        Format::Tick(800, "800"), Format::Tick(850), 
        Format::Tick(900, "900"), Format::Tick(950), 
        Format::Tick(1000, "1000"), Format::Tick(1050), 
        Format::Tick(1100, "1100"), Format::Tick(1150), 
        Format::Tick(1200, "1200"), Format::Tick(1250), 
        Format::Tick(1300, "1300"), Format::Tick(1350), 
        Format::Tick(1400, "1400"), Format::Tick(1450), 
        Format::Tick(1500, "1500")
    }};

    const Axis y_axis = Format::Axis(100, 0, QString("Intensity (%)"));
    AxisRange y_range = Format::AxisRange(100, 0);
    Ticks<6> y_ticks = {{
        Format::Tick(0, "0"),
        Format::Tick(20, "20"),
        Format::Tick(40, "40"), 
        Format::Tick(60, "60"), 
        Format::Tick(80, "80"), 
        Format::Tick(100, "100")
    }};
};

class Style : public QWidget {
    Q_OBJECT
    Q_PROPERTY(QString scene READ scene WRITE setScene)
    Q_PROPERTY(QString background READ background WRITE setBackground)
    Q_PROPERTY(QString background_hover READ backgroundHover WRITE setBackgroundHover)
    Q_PROPERTY(QString background_press READ backgroundPress WRITE setBackgroundPress)
    Q_PROPERTY(QString label READ label WRITE setLabel)
    Q_PROPERTY(QString label_weight READ labelWeight WRITE setLabelWeight)
    Q_PROPERTY(QString grid_label READ gridLabel WRITE setGridLabel)
    Q_PROPERTY(QString grid_label_weight READ gridLabelWeight WRITE setGridLabelWeight)
    Q_PROPERTY(QString axis READ axis WRITE setAxis)
    Q_PROPERTY(QString axis_hover READ axisHover WRITE setAxisHover)
    Q_PROPERTY(QString axis_press READ axisPress WRITE setAxisPress)
    Q_PROPERTY(QString absorption_width READ absorptionWidth WRITE setAbsorptionWidth)
    Q_PROPERTY(QString absorption_style READ absorptionStyle WRITE setAbsorptionStyle)
    Q_PROPERTY(QString excitation_width READ excitationWidth WRITE setExcitationWidth)
    Q_PROPERTY(QString excitation_style READ excitationStyle WRITE setExcitationStyle)
    Q_PROPERTY(QString emission_width READ emissionWidth WRITE setEmissionWidth)
    Q_PROPERTY(QString emission_style READ emissionStyle WRITE setEmissionStyle)

    public:
        explicit Style(QWidget* parent=nullptr);
        Style(const Style &obj) = default;
        Style& operator=(const Style &obj) = default;
        Style(Style&&) = default;
        Style& operator=(Style&&) = default;
        ~Style() = default;

        QString scene() const;
        void setScene(QString scene);
        QString background() const;
        void setBackground(QString background);
        QString backgroundHover() const;
        void setBackgroundHover(QString background_hover);
        QString backgroundPress() const;
        void setBackgroundPress(QString background_press);
        QString label() const;
        void setLabel(QString label);
        QString labelWeight() const;
        void setLabelWeight(QString label_weight);
        QString gridLabel() const;
        void setGridLabel(QString grid_label);
        QString gridLabelWeight() const;
        void setGridLabelWeight(QString grid_label_weight);
        QString axis() const;
        void setAxis(QString axis);
        QString axisHover() const;
        void setAxisHover(QString axis_hover);
        QString axisPress() const;
        void setAxisPress(QString axis_press);
        QString absorptionWidth() const;
        void setAbsorptionWidth(QString absorption_width);
        QString absorptionStyle() const;
        void setAbsorptionStyle(QString absorption_style);
        QString excitationWidth() const;
        void setExcitationWidth(QString excitation_width);
        QString excitationStyle() const;
        void setExcitationStyle(QString excitation_style);
        QString emissionWidth() const;
        void setEmissionWidth(QString emission_width);
        QString emissionStyle() const;
        void setEmissionStyle(QString emission_style);

    private:
        QColor style_scene;
        QColor style_background;
        QColor style_background_hover;
        QColor style_background_press;
        QColor style_label;
        QFont::Weight style_label_weight;
        QColor style_grid_label;
        QFont::Weight style_grid_label_weight;
        QColor style_axis;
        QColor style_axis_hover;
        QColor style_axis_press;
        int absorption_width;
        Qt::PenStyle absorption_style;
        int excitation_width;
        Qt::PenStyle excitation_style;
        int emission_width;
        Qt::PenStyle emission_style;

        static QFont::Weight textToFontWeight(const QString& text, QFont::Weight fallback=QFont::Bold);
        static QString fontWeightToText(QFont::Weight weight);
        static Qt::PenStyle textToPenStyle(const QString& text, Qt::PenStyle fallback=Qt::DashDotLine);
        static QString penStyleToText(Qt::PenStyle style);

        bool eventFilter(QObject*, QEvent*);
    
    public:
        QBrush brushScene() const;
        QBrush brushLabel() const;
        QBrush brushGridLabel() const;
        QBrush brushBackground() const;
        QBrush brushBackgroundHover() const;
        QBrush brushBackgroundPress() const;
        QBrush brushEmission(QColor color) const;
        QBrush brushEmissionSelect(QColor color) const;

        QFont fontLabel() const;
        QFont fontGridLabel() const;

        QPen penAxis() const;
        QPen penAxisHover() const;
        QPen penAxisPress() const;
        QPen penAbsorption(QColor color) const;
        QPen penExcitation(QColor color) const;
        QPen penEmission(QColor color) const;
        QPen penAbsorptionSelect(QColor color) const;
        QPen penExcitationSelect(QColor color) const;
        QPen penEmissionSelect(QColor color) const;
    
    signals:
        void styleChanged();

};

} // Format namespace

} // Graph namespace

#endif // GRAPH_FORMAT_H