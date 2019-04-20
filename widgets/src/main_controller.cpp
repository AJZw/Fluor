/**** General **************************************************************
** Version:    v0.9.1
** Date:       2019-03-11
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include "main_controller.h"
#include "central_controller.h"
#include "application.h"
#include <QApplication>
#include <QEvent>
#include <QDebug>

namespace Main {

/*
Initiliaser - builds a QMainWindow widget and set the connections
*/
Controller::Controller(QWidget* parent) :
    QMainWindow(parent),
    window_width(800),
    window_height(200)
{
    // Set MainWindow properties
    this->setWindowTitle("Fluor");
    this->setGeometry(0, 0, this->window_width, this->window_height);
    this->setMinimumSize(this->window_width, this->window_height);
    this->setContentsMargins(8, 8, 8, 8);

    Central::Controller* controller_widget = new Central::Controller(this);
    this->setCentralWidget(controller_widget);

    this->installEventFilter(this);

    // Connect signals and slots
    QObject::connect(static_cast<Application*>(QApplication::instance()), &Application::globalMouseButtonRelease, controller_widget, &Central::Controller::unfocus);
    QObject::connect(this, &Main::Controller::resized, controller_widget, &Central::Controller::reloadMaxSize);
    QObject::connect(this, &Main::Controller::moved, controller_widget, &Central::Controller::reloadMaxSize);
}

/*
eventFilter - captures MouseClickEvent, resize, and close event
    :param obj: source object
    :param event: the event
*/
bool Controller::eventFilter(QObject *obj, QEvent *event){
    Q_UNUSED(obj);
    switch(event->type()){
    case QEvent::Resize:
        emit this->resized(this);
        return false;
    case QEvent::Move:
        emit this->moved(this);
        return false;
    //case QEvent::Close:
    //    // write settings
    //    return false;
    default:
        break;
    }

    return QMainWindow::eventFilter(obj, event);
}

} // Main namespace
