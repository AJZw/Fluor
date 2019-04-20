/**** General **************************************************************
** Version:    v0.9.1
** Date:       2019-04-07
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

/**** LGPLv3 License *******************************************************
** laser_controller.h is part of Fluor
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
** Controller of the laser selection widgets
**
** :class: Controller
** Controls the signal and slots of the laser selection
** 
***************************************************************************/

#ifndef LASER_CONTROLLER_H
#define LASER_CONTROLLER_H

#include <QWidget>
#include <QStringList>
#include <QPaintEvent>

//#include "data_cytometer.h"

namespace Laser {

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
        void unfocus(QEvent* event);
        //void reloadCytometer(const DataCytometer* data=nullptr);
        void reloadMaxSize(const QWidget* widget=nullptr);

    private slots:
        void clickedPushButton(bool checked);
        void finishedLineEdit();
    
    signals:
        void unfocused(QEvent* event);
        //void reloadedCytometer(const DataCytometer* data=nullptr);
        void reloadedMaxSize(const QWidget* widget=nullptr);

        void showPushButton();
        void hidePushButton();
        void showLineEdit();
        void hideLineEdit();

};

} // Laser namespace


#endif //LASER_CONTROLLER_H