/**** General **************************************************************
** Version:    v0.9.1
** Date:       2019-03-03
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

/**** LGPLv3 License *******************************************************
** toolbar_buttons.h is part of Fluor
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
** The bar menu buttons
**
** :class: Bar::SquarePushButton
** A QPushButton that always has a square shape. Can be enabled/disabled and active/inactive
**
** :class: Bar::IconPushButton
** A SquarePushButton of a fixed icon size
**
** :class: Bar::LaserButton
** Laser visibility toggle button, contains both enable/disable, active/inactive states
**
** :class: Bar::ExcitationPushButton
** Excitation visibility toggle button, contains active/inactive states
**
** :class: Bar::EmissionButton
** Emission visibility toggle button, contains active/inactive states
**
** :class: Bar::DetectorButton
** Detector visibility toggle button, contains both enable/disable, active/inactive states
**
** :class: Bar::GraphAddButton
** Button to add a graph, contains enable/disable states
**
** :class: Bar::GraphRemoveButton
** Button to remove a graph, contains enable/disable states
**
** :class: Bar::LasersButton
** Button to expand/reduce to amount of graphs, contains enable/disable, active/inactive states
**
** :class: Bar::Label
** A label to show information, contains enable/disable state
**
***************************************************************************/

#ifndef TOOLBAR_BUTTONS_H
#define TOOLBAR_BUTTONS_H

#include <QPushButton>
#include <QLabel>
#include <QSize>
#include <QString>
#include <QIcon>
#include "data_styles.h"

namespace Bar {

class SquarePushButton : public QPushButton {
    Q_OBJECT

    public:
        explicit SquarePushButton(QWidget* parent=nullptr);
        SquarePushButton(const SquarePushButton &obj) = delete;
        SquarePushButton& operator=(const SquarePushButton &obj) = delete;
        SquarePushButton(SquarePushButton&&) = delete;
        SquarePushButton& operator=(SquarePushButton&&) = delete;
        virtual ~SquarePushButton() = default;
    
    private:
        QSize sizeHint() const;
        QSize minimumSizeHint() const;
        void setToolTip();

    protected:
        QString tooltip_disabled;
        QString tooltip_active;
        QString tooltip_inactive;

    signals:
        void clicked(bool active);

    public slots:
        void hideButton();
        void showButton();
        void click(bool checked);
        void setEnabled(bool enable);

    protected slots:
        virtual void toggleActive(bool checked);
        virtual void setActive(const bool active);
};

class IconPushButton : public SquarePushButton {
    Q_OBJECT

    public:
        explicit IconPushButton(QWidget* parent=nullptr);
        IconPushButton(const IconPushButton &obj) = delete;
        IconPushButton& operator=(const IconPushButton &obj) = delete;
        IconPushButton(IconPushButton&&) = delete;
        IconPushButton& operator=(IconPushButton&&) = delete;
        virtual ~IconPushButton() = default;
};

class LaserButton : public IconPushButton {
    Q_OBJECT

    public:
        explicit LaserButton(QWidget* parent=nullptr);
        LaserButton(const LaserButton& obj) = delete;
        LaserButton& operator=(const LaserButton& obj) = delete;
        LaserButton(LaserButton&&) = delete;
        LaserButton& operator=(LaserButton&&) = delete;
        virtual ~LaserButton() = default;

    signals:
        void clicked(bool active);
};

class ExcitationButton : public IconPushButton {
    Q_OBJECT

    public:
        explicit ExcitationButton(QWidget* parent=nullptr);
        ExcitationButton(const ExcitationButton &obj) = delete;
        ExcitationButton& operator=(const ExcitationButton &obj) = delete;
        ExcitationButton(ExcitationButton&&) = delete;
        ExcitationButton& operator=(ExcitationButton&&) = delete;
        virtual ~ExcitationButton() = default;
};

class EmissionButton : public IconPushButton {
    Q_OBJECT

    public:
        explicit EmissionButton(QWidget* parent=nullptr);
        EmissionButton(const EmissionButton &obj) = delete;
        EmissionButton& operator=(const EmissionButton &obj) = delete;
        EmissionButton(EmissionButton&&) = delete;
        EmissionButton& operator=(EmissionButton&&) = delete;
        virtual ~EmissionButton() = default;
};

class DetectorButton : public IconPushButton {
    Q_OBJECT

    public:
        explicit DetectorButton(QWidget* parent=nullptr);
        DetectorButton(const DetectorButton& obj) = delete;
        DetectorButton& operator=(const DetectorButton& obj) = delete;
        DetectorButton(DetectorButton&&) = delete;
        DetectorButton& operator=(const DetectorButton&&) = delete;
        virtual ~DetectorButton() = default; 
};

class GraphAddButton : public IconPushButton {
    Q_OBJECT

    public:
        explicit GraphAddButton(QWidget* parent=nullptr);
        GraphAddButton(const GraphAddButton& obj) = delete;
        GraphAddButton& operator=(const GraphAddButton& obj) = delete;
        GraphAddButton(GraphAddButton&&) = delete;
        GraphAddButton& operator=(const GraphAddButton&&) = delete;
        virtual ~GraphAddButton() = default;
    
    protected slots:
        virtual void setActive(const bool active) override;
};

class GraphRemoveButton : public IconPushButton {
    Q_OBJECT

    public:
        explicit GraphRemoveButton(QWidget* parent=nullptr);
        GraphRemoveButton(const GraphRemoveButton& obj) = delete;
        GraphRemoveButton& operator=(const GraphRemoveButton& obj) = delete;
        GraphRemoveButton(GraphRemoveButton&&) = delete;
        GraphRemoveButton& operator=(const GraphRemoveButton&&) = delete;
        virtual ~GraphRemoveButton() = default;
    
    protected slots:
        virtual void setActive(const bool active) override;
};

class LasersButton : public IconPushButton {
    Q_OBJECT

    public:
        explicit LasersButton(QWidget* parent=nullptr);
        LasersButton(const LasersButton& obj) = delete;
        LasersButton& operator=(const LasersButton& obj) = delete;
        LasersButton(LasersButton&&) = delete;
        LasersButton& operator=(const LasersButton&&) = delete;
        virtual ~LasersButton() = default; 
};

class Label : public QLabel {
    Q_OBJECT

    public:
        explicit Label(QWidget* parent=nullptr);
        Label(const Label& obj) = delete;
        Label& operator=(const Label& obj) = delete;
        Label(Label&&) = delete;
        Label& operator=(const Label&&) = delete;
        ~Label() = default; 
};

} // Bar namespace

#endif //TOOLBAR_BUTTONS_H