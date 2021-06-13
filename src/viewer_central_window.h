/**** General **************************************************************
** Version:    v0.9.0
** Date:       2018-09-09
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2018 - AJ Zwijnenburg
** License:    GPLv3
***************************************************************************/

/**** GPLv3 License ******************************************************** 
** viewer_central_window.cpp is part of Spectral Viewer
**        
** Spectral Viewer is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License as published
** by the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
** 
** Spectral Viewer is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License along
** with Spectral Viewer. If not, see <https://www.gnu.org/licenses/>.  
***************************************************************************/

/**** DOC ******************************************************************
** Central window of the viewer
**
** :class: Application
** A QApplication subclass, contains additional styling parameters
**
** :class: Window
** A QMainWindow that holds the the MainWidget as a central widget, and the menu's
**
** :class: MainWidget
** A QWidget with a QGridLayout. 
***************************************************************************/

#ifndef VIEWER_CENTRAL_WIDGET_H
#define VIEWER_CENTRAL_WIDGET_H

#include <QApplication>
#include <QMainWindow>
#include <QSettings>

class Application : public QApplication
{
public:
    Application(int &argc, char **argv);
    //setSettings(const settings &settings);
    void setStyleSheet(QString stylesheet);
};

class MainWindow : public QMainWindow
{
    //Q_OBJECT

public:
    MainWindow();

//private slots:
    
private:
    QString window_title;
    int window_width;
    int window_height;

    
};


#endif //VIEWER_CENTRAL_WIDGET_H

