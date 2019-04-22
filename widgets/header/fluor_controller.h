/**** General **************************************************************
** Version:    v0.9.2
** Date:       2019-03-03
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

/**** LGPLv3 License *******************************************************
** fluor_controller.h is part of Fluor
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
** The fluorophore menu controller
** 
** :class: Controller
** Builds and controlls the fluorophore menu. Combines a FLuorLineEdit with 
** a QScrollWidget and the fluorophore buttons
** Internally, connects the signal and slots of internal events and forms an
** interface for external signals and slots
**
***************************************************************************/

#ifndef FLUOR_CONTROLLER_H
#define FLUOR_CONTROLLER_H

#include <QWidget>
#include <QStringList>
#include <QPaintEvent>
#include "data_fluorophores.h"

namespace Fluor {

class Controller : public QWidget{
    Q_OBJECT
    
    public:
        explicit Controller(QWidget *parent = nullptr);
        Controller(const Controller &obj) = delete;
        Controller& operator=(const Controller &obj) = delete;
        Controller(Controller&&) = delete;
        Controller& operator=(Controller&&) = delete;
        ~Controller() = default;

        void paintEvent(QPaintEvent* event);

    public slots:
        void receiveGlobalEvent(QEvent* event);
        void reloadGlobalSize(const QWidget* widget=nullptr);
        void reloadData(const DataFluorophores* data=nullptr);

    private slots:
        void clickedPushButton(bool checked);
        void finishedLineEdit();
    
    signals:
        void sendGlobalEvent(QEvent* event);
        void reloadedGlobalSize(const QWidget* widget=nullptr);
        void reloadedData(const DataFluorophores* data=nullptr);

        void showPushButton();
        void hidePushButton();
        void showLineEdit();
        void hideLineEdit();

        void output(QStringList outputs);

};

} // Fluor namespace

#endif //FLUOR_CONTROLLER_H