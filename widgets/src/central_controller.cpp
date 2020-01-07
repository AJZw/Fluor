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
#include "graph_controller.h"

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
    Graph::Controller* controller_graph = new Graph::Controller(this);

    controller_layout->addWidget(controller_laser, 0, 0, 1, 1);
    controller_layout->addWidget(controller_toolbar, 0, 1, 1, 1);
    controller_layout->addWidget(controller_fluor, 1, 0, 1, 1);
    controller_layout->addWidget(controller_graph, 1, 1, 1, 1);

    // Connect signals and slots
    QObject::connect(this, &Central::Controller::sendGlobalEvent, controller_laser, &Laser::Controller::receiveGlobalEvent);
    QObject::connect(this, &Central::Controller::sendGlobalSize, controller_laser, &Laser::Controller::receiveGlobalSize);
    QObject::connect(this, &Central::Controller::sendInstrument, controller_laser, &Laser::Controller::receiveInstrument);
    
    QObject::connect(this, &Central::Controller::sendGlobalEvent, controller_fluor, &Fluor::Controller::receiveGlobalEvent);
    QObject::connect(this, &Central::Controller::sendGlobalSize, controller_fluor, &Fluor::Controller::receiveGlobalSize);
    QObject::connect(this, &Central::Controller::sendData, controller_fluor, &Fluor::Controller::receiveData);
    QObject::connect(this, &Central::Controller::sendCacheSync, controller_fluor, &Fluor::Controller::receiveCacheSync);
    QObject::connect(this, &Central::Controller::sendCacheUpdate, controller_fluor, &Fluor::Controller::receiveCacheUpdate);

    QObject::connect(controller_laser, &Laser::Controller::sendOutput, this, &Central::Controller::receiveLaser);

    QObject::connect(controller_fluor, &Fluor::Controller::sendCacheAdd, this, &Central::Controller::receiveCacheAdd);
    QObject::connect(controller_fluor, &Fluor::Controller::sendCacheRemove, this, &Central::Controller::receiveCacheRemove);
    QObject::connect(controller_fluor, &Fluor::Controller::sendCacheRequestUpdate, this, &Central::Controller::receiveCacheRequestUpdate);

    QObject::connect(controller_toolbar, &Bar::Controller::sendToolbarStateChange, this, &Central::Controller::receiveToolbarStateChange);
    QObject::connect(this, &Central::Controller::sendToolbarStateUpdate, controller_toolbar, &Bar::Controller::receiveToolbarStateUpdate);

    QObject::connect(this, &Central::Controller::sendCacheSync, controller_graph, &Graph::Controller::receiveCacheSync);
    QObject::connect(this, &Central::Controller::sendCacheUpdate, controller_graph, &Graph::Controller::receiveCacheUpdate);
    QObject::connect(controller_graph, &Graph::Controller::sendCacheRequestUpdate, this, &Central::Controller::receiveCacheRequestUpdate);
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
Getter for layout spacing property, as it has to return a QString it returns the value in pixels
*/
QString Controller::layoutSpacing() const {
    return QString::number(this->layout()->spacing(), 'f', 0);
}

/*
Receives layout scaling properties from the stylesheet
*/
void Controller::setLayoutSpacing(QString layout_spacing){
    this->layout()->setSpacing(layout_spacing.toInt());
    this->adjustSize();
}

/*
Slot: receives and forward global events
*/
void Controller::receiveGlobalEvent(QEvent* event){
    emit this->sendGlobalEvent(event);
}

/*
Slot: reloads the fluorophore data
*/
void Controller::receiveData(const Data::FluorophoreReader& data){
    emit this->sendData(data);
}

/*
Slot: reloads the data of the instrument
*/
void Controller::receiveInstrument(const Data::Instrument& instrument){
    emit this->sendInstrument(instrument);
}

/*
Slot: reloads the max size of a (ListView) widget
*/
void Controller::receiveGlobalSize(const QWidget* widget){
    emit this->sendGlobalSize(widget);
}

/*
Slot: forwards the request to the cache to update
*/
void Controller::receiveCacheRequestUpdate(){
    emit this->sendCacheRequestUpdate();
}

/*
Slot: receives and forwards the cache adding input
*/
void Controller::receiveCacheAdd(std::vector<Data::FluorophoreID>& fluorophores){
    emit this->sendCacheAdd(fluorophores);
}

/*
Slot: receives and forwards the cache remove input
*/
void Controller::receiveCacheRemove(std::vector<Data::FluorophoreID>& fluorophores){
    emit this->sendCacheRemove(fluorophores);
}

/*
Slot: receives and forwards the synchronisation request of the fluor buttons
*/
void Controller::receiveCacheSync(const std::vector<Cache::CacheID>& cache_state){
    emit this->sendCacheSync(cache_state);
}

/*
Slot: forwards the synchronisation request to the graph
*/
void Controller::receiveCacheUpdate(const std::vector<Cache::CacheID>& cache_state){
    emit this->sendCacheUpdate(cache_state);
}

/*
Slot: receives and forwards the laser wavelength input
*/
void Controller::receiveLaser(int wavelength){
    emit this->sendLaser(wavelength);
}

/*
Slot: receives and forwards Toolbar State changes
    :param type: the button that sends the change
    :param active: the active state
    :param enable: the enable state
*/
void Controller::receiveToolbarStateChange(Bar::ButtonType type, bool active, bool enable){
    emit this->sendToolbarStateChange(type, active, enable);
}

/*
Slot: receives and forwards Toolbar State update
    :param type: the button that should receive the update
    :param active: the active state
    :param enable: the enable state
*/
void Controller::receiveToolbarStateUpdate(Bar::ButtonType type, bool active, bool enable){
    emit this->sendToolbarStateUpdate(type, active, enable);
}

} // Central namespace
