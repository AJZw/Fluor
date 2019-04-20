/**** General **************************************************************
** Version:    v0.9.1
** Date:       2019-03-11
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

/**** LGPLv3 License *******************************************************
** maun_controller.h is part of Fluor
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
** Controller of the main widget
**
** :class: Controller
** Controls the signal and slots of the main widgets
** 
***************************************************************************/

#ifndef MAIN_CONTROLLER_H
#define MAIN_CONTROLLER_H

#include <QMainWindow>

namespace Main {

class Controller : public QMainWindow {
    Q_OBJECT

    public:
        explicit Controller(QWidget *parent = nullptr);
        Controller(const Controller &obj) = delete;
        Controller& operator=(const Controller &obj) = delete;
        Controller(Controller&&) = delete;
        Controller& operator=(Controller&&) = delete;
        ~Controller() = default;

    //private slots:
        
    private:
        QString window_title;
        int window_width;
        int window_height;

        bool eventFilter(QObject *obj, QEvent *event);

    signals:
        void clicked();
        void resized(const QWidget* widget);
        void moved(const QWidget* widget);
};

} // Main namespace

#endif //MAIN_CONTROLLER_H