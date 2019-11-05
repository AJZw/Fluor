/**** General **************************************************************
** Version:    v0.9.1
** Date:       2019-03-03
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

/**** LGPLv3 License *******************************************************
** fluor_buttons.h is part of Fluor
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
** The Fluorophore menu buttons
**
** :class: Fluor::PushButton
** A pushbutton to be shown if the FluorLineEdit is inactive, clicking the 
** button shows the FluorLineEdit
**
** :class: Fluor::EmissionButton
** A button for visualizing and settings the emission visibility of a fluorophore
**
** :class: Fluor::ExcitationButton
** A button for visualizing and settings the excitation visibility of a fluorophore
**
** :class: Fluor::RemoveButton
** A button for removing the flourophore from the layout
**
***************************************************************************/

#ifndef FLUOR_BUTTONS_H
#define FLUOR_BUTTONS_H

#include <QPushButton>
#include <QObject>
#include <QEvent>

namespace Fluor {

class PushButton : public QPushButton {
    Q_OBJECT

    public:
        explicit PushButton(QWidget* parent=nullptr);
        PushButton(const PushButton &obj) = delete;
        PushButton& operator=(const PushButton &obj) = delete;
        PushButton(PushButton&&) = delete;
        PushButton& operator=(PushButton&&) = delete;
        ~PushButton() = default;
    
    public slots:
        void hideButton();
        void showButton();
};

class EmissionButton : public QPushButton {
    Q_OBJECT

    public:
        explicit EmissionButton(QWidget* parent=nullptr);
        EmissionButton(const EmissionButton &obj) = delete;
        EmissionButton& operator=(const EmissionButton &obj) = delete;
        EmissionButton(EmissionButton&&) = delete;
        EmissionButton& operator=(EmissionButton&&) = delete;
        ~EmissionButton() = default;
    
    private:
        QString tooltip_active;
        QString tooltip_inactive;

        void setToolTip();
        bool eventFilter(QObject* obj, QEvent* event);
    
    signals:
        void clicked(bool active);
        void selected();
        void hoverEntered();
        void hoverLeaved();

    private slots:
        void click(bool checked);

    public slots:
        void toggleActive(bool checked);
        void setActive(bool active);
        void setSelect(bool select);
};

class ExcitationButton : public QPushButton {
    Q_OBJECT

    public:
        explicit ExcitationButton(QWidget* parent=nullptr);
        ExcitationButton(const ExcitationButton &obj) = delete;
        ExcitationButton& operator=(const ExcitationButton &obj) = delete;
        ExcitationButton(ExcitationButton&&) = delete;
        ExcitationButton& operator=(ExcitationButton&&) = delete;
        ~ExcitationButton() = default;

    private:
        QString tooltip_active;
        QString tooltip_inactive;
    
        void setToolTip();
        bool eventFilter(QObject* obj, QEvent* event);
    
    signals:
        void clicked(bool active);
        void hoverEntered();
        void hoverLeaved();

    private slots:
        void click(bool checked);
        
    public slots:
        void toggleActive(bool checked);
        void setActive(bool active);
};

class RemoveButton : public QPushButton {
    Q_OBJECT

    public:
        explicit RemoveButton(QWidget* parent=nullptr);
        RemoveButton(const RemoveButton &obj) = delete;
        RemoveButton& operator=(const RemoveButton &obj) = delete;
        RemoveButton(RemoveButton&&) = delete;
        RemoveButton& operator=(RemoveButton&&) = delete;
        ~RemoveButton() = default;

    private:
        QString tooltip_active;
        QString tooltip_inactive;

        void setToolTip();
        bool eventFilter(QObject* obj, QEvent* event);
    
    signals:
        void clicked(bool active);
        void hoverEntered();
        void hoverLeaved();

    private slots:
        void click(bool checked);

    public slots:
        void setActive(const bool active);
};

} // Fluor namespace

#endif //FLUOR_BUTTONS_H