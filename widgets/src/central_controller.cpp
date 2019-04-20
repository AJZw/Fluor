/**** General **************************************************************
** Version:    v0.9.1
** Date:       2019-03-11
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include "central_controller.h"

#include <QGridLayout>
#include "laser_controller.h"
#include "toolbar_controller.h"
#include "fluor_controller.h"
#include <QStyleOption>
#include <QStyle>
#include <QPainter>
#include <QDebug>

namespace Central {

/*
Initializer: builds and connects central widget
*/
Controller::Controller(QWidget* parent) :
    QWidget(parent)
{
    // Set base properties
    this->setContentsMargins(0, 0, 0, 0);

    // Build layout
    QGridLayout* controller_layout = new QGridLayout(this);
    controller_layout->setRowMinimumHeight(0, 1);
    controller_layout->setRowStretch(0, 0);
    controller_layout->setRowStretch(1, 1);
    controller_layout->setColumnMinimumWidth(0, 80);
    controller_layout->setColumnMinimumWidth(1, 80);
    controller_layout->setColumnStretch(0, 0);
    controller_layout->setColumnStretch(1, 1);
    controller_layout->setContentsMargins(0, 0, 0, 0);
    controller_layout->setSpacing(6);

    // Add widgets
    Laser::Controller* controller_laser = new Laser::Controller(this);
    Bar::Controller* controller_toolbar = new Bar::Controller(this);
    Fluor::Controller* controller_fluor = new Fluor::Controller(this);

    controller_layout->addWidget(controller_laser, 0, 0, 1, 1);
    controller_layout->addWidget(controller_toolbar, 0, 1, 1, 1);
    controller_layout->addWidget(controller_fluor, 1, 0, 1, 1);

    // Connect signals and slots
    QObject::connect(this, &Central::Controller::unfocused, controller_fluor, &Fluor::Controller::unfocus);
    QObject::connect(this, &Central::Controller::reloadedData, controller_fluor, &Fluor::Controller::reloadData);
    QObject::connect(this, &Central::Controller::reloadedMaxSize, controller_fluor, &Fluor::Controller::reloadMaxSize);
    
    QObject::connect(this, &Central::Controller::unfocused, controller_laser, &Laser::Controller::unfocus);
    QObject::connect(this, &Central::Controller::reloadedMaxSize, controller_laser, &Laser::Controller::reloadMaxSize);

    //this->setStyleSheet("Central--Controller {background-color: blue;}");

}

/*
Reimplementation of paintEvent. This allows proper stylesheet management of QWidget inheriting widget
    :param event: paintevent
*/
void Controller::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QStyleOption style_option;
    style_option.initFrom(this);
    QPainter painter(this);
    this->style()->drawPrimitive(QStyle::PE_Widget, &style_option, &painter, this);
}

/*
Slot: unfocus the widget
*/
void Controller::unfocus(QEvent* event){
    emit this->unfocused(event);
}

/*
Slot: reloads the style of the widget (only necessary for non-CSS dependent style features)
*/
void Controller::reloadStyle(){
    emit this->reloadedStyle();
}

/*
Slot: reloads the data of the widget
*/
void Controller::reloadData(const DataFluorophores* data){
    emit this->reloadedData(data);
}

/*
Slot: reloads the max size of a (ListView) widget
*/
void Controller::reloadMaxSize(const QWidget* widget){
    emit this->reloadedMaxSize(widget);
}

} // Central namespace
