/**** General **************************************************************
** Version:    v0.9.2
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
    QObject::connect(this, &Central::Controller::sendGlobalEvent, controller_fluor, &Fluor::Controller::receiveGlobalEvent);
    QObject::connect(this, &Central::Controller::reloadedData, controller_fluor, &Fluor::Controller::reloadData);
    QObject::connect(this, &Central::Controller::reloadedGlobalSize, controller_fluor, &Fluor::Controller::reloadGlobalSize);
    
    QObject::connect(this, &Central::Controller::sendGlobalEvent, controller_laser, &Laser::Controller::receiveGlobalEvent);
    QObject::connect(this, &Central::Controller::reloadedGlobalSize, controller_laser, &Laser::Controller::reloadGlobalSize);

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
Slot: receives and forward global events
*/
void Controller::receiveGlobalEvent(QEvent* event){
    emit this->sendGlobalEvent(event);
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
void Controller::reloadGlobalSize(const QWidget* widget){
    emit this->reloadedGlobalSize(widget);
}

} // Central namespace
