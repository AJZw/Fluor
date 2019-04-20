/**** General **************************************************************
** Version:    v0.9.1
** Date:       2018-04-10
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

/**** LGPLv3 License *******************************************************
** application.h is part of Fluor
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
** The event manager
**
** :class: Application
** A QApplication subclass singleton, contains additional styling parameters,
** and captures and forwards global events
**
***************************************************************************/

#ifndef VIEWER_CENTRAL_WIDGET_H
#define VIEWER_CENTRAL_WIDGET_H

#include <QApplication>

class Application : public QApplication {
    Q_OBJECT

    private:
        bool eventFilter(QObject* obj, QEvent* event);

    public:
        Application(int &argc, char **argv);
        //setSettings(const settings &settings);
        void setStyleSheet(QString stylesheet);

    signals:
        void globalMouseButtonRelease(QEvent* event);

};

#endif //VIEWER_CENTRAL_WIDGET_H

