/**** General **************************************************************
** Version:    v0.9.3
** Date:       2019-04-23
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
** Controller of the  
**
** :class: Controller
** Controls the signal and slots of a graph widget
** 
***************************************************************************/

#ifndef GRAPH_CONTROLLER_H
#define GRAPH_CONTROLLER_H

#include <QWidget>
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

    signals:
        void sendGlobalEvent(QEvent* event);

    public slots:
        void receiveGlobalEvent(QEvent* event);

};

} // Bar namespace

#endif // GRAPH_CONTROLLER_H