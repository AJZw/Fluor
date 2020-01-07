/**** General **************************************************************
** Version:    v0.9.2
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
** :class: Laser::Controller
** Controls the signal and slots of the laser selection
** 
***************************************************************************/

#ifndef LASER_CONTROLLER_H
#define LASER_CONTROLLER_H

#include <QWidget>
#include <QStringList>
#include <QPaintEvent>
#include "data_instruments.h"

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
        void receiveGlobalEvent(QEvent* event);
        void receiveGlobalSize(const QWidget* widget=nullptr);
        void receiveInstrument(const Data::Instrument& instrument);
        void receiveOutput(int output);

    private slots:
        void clickedPushButton(bool checked);
        void finishedLineEdit();
    
    signals:
        void sendGlobalEvent(QEvent* event);
        void sendGlobalSize(const QWidget* widget=nullptr);
        void sendInstrument(const Data::Instrument& instrument);
        void sendOutput(int output);

        void showPushButton();
        void hidePushButton();
        void showLineEdit();
        void hideLineEdit();

};

} // Laser namespace


#endif //LASER_CONTROLLER_H