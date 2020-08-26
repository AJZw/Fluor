/**** General **************************************************************
** Version:    v0.9.2
** Date:       2019-03-11
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include "main_controller.h"
#include "central_controller.h"
#include "application.h"
#include <QApplication>
#include <QWindow>
#include <QEvent>
#include <QDebug>

namespace Main {

/*
Initiliaser - builds a QMainWindow widget and set the connections
*/
Controller::Controller(QWidget* parent) :
    QMainWindow(parent),
    window_screen(nullptr),
    window_width(600),
    window_height(300)
{
    // Set MainWindow properties
    this->setWindowTitle("Fluor");
    this->setGeometry(0, 0, this->window_width, this->window_height);
    this->setMinimumSize(this->window_width, this->window_height);
    this->setContentsMargins(8, 8, 8, 8);

    Central::Controller* controller_widget = new Central::Controller(this);
    this->setCentralWidget(controller_widget);

    this->installEventFilter(this);

    // Receive global events
    QObject::connect(static_cast<Application*>(QApplication::instance()), &Application::globalMouseButtonRelease, this, &Main::Controller::receiveGlobalEvent);
    
    // Connect signals and slots
    QObject::connect(this, &Main::Controller::sendGlobalEvent, controller_widget, &Central::Controller::receiveGlobalEvent);
    QObject::connect(this, &Main::Controller::resized, controller_widget, &Central::Controller::receiveGlobalSize);
    QObject::connect(this, &Main::Controller::moved, controller_widget, &Central::Controller::receiveGlobalSize);
    QObject::connect(this, &Main::Controller::sendData, controller_widget, &Central::Controller::receiveData);
    QObject::connect(this, &Main::Controller::sendInstrument, controller_widget, &Central::Controller::receiveInstrument);

    QObject::connect(controller_widget, &Central::Controller::sendCacheRequestUpdate, this, &Main::Controller::receiveCacheRequestUpdate);
    QObject::connect(controller_widget, &Central::Controller::sendCacheAdd, this, &Main::Controller::receiveCacheAdd);
    QObject::connect(controller_widget, &Central::Controller::sendCacheRemove, this, &Main::Controller::receiveCacheRemove);
    QObject::connect(controller_widget, &Central::Controller::sendLaser, this, &Main::Controller::receiveLaser);
    QObject::connect(this, &Main::Controller::sendCacheState, controller_widget, &Central::Controller::receiveCacheState);
    QObject::connect(this, &Main::Controller::sendCacheUpdate, controller_widget, &Central::Controller::receiveCacheUpdate);

    QObject::connect(controller_widget, &Central::Controller::sendToolbarStateChange, this, &Main::Controller::receiveToolbarStateChange);
    QObject::connect(this, &Main::Controller::sendToolbarStateUpdate, controller_widget, &Central::Controller::receiveToolbarStateUpdate);

    QObject::connect(controller_widget, &Central::Controller::sendGraphSelect, this, &Main::Controller::receiveGraphSelect);
    QObject::connect(this, &Main::Controller::sendGraphState, controller_widget, &Central::Controller::receiveGraphState);
}

/*
Getter for layout spacing property, as it has to return a QString it returns the value in pixels
*/
QString Controller::layoutMargins() const {
    return QString::number(this->contentsMargins().left(), 'f', 0);
}

/*
Receives layout scaling properties from the stylesheet
*/
void Controller::setLayoutMargins(QString layout_margins){
    int layout_margin_px = layout_margins.toInt();
    this->setContentsMargins(layout_margin_px, layout_margin_px, layout_margin_px, layout_margin_px);
}

/*
eventFilter - captures resize, move, and close event
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

/*
Shows the widget. Additionally connects the widget to screenChanged and logicalDotsPerInchChanged signals
*/
void Controller::show(){
    QMainWindow::show();
    
    QObject::connect(this->windowHandle(), &QWindow::screenChanged, this, &Main::Controller::receiveScreenChanged);
    this->window_screen = this->windowHandle()->screen();
    QObject::connect(this->window_screen, &QScreen::logicalDotsPerInchChanged, this, &Main::Controller::receiveDPIChanged);
}

/*
Hides the widget. Additionally disconnects the widget from screenChanged and logicalDotsPerInchChanged signals
*/
void Controller::hide(){
    QObject::disconnect(this->windowHandle(), &QWindow::screenChanged, this, &Main::Controller::receiveScreenChanged);
    QObject::disconnect(this->window_screen, &QScreen::logicalDotsPerInchChanged, this, &Main::Controller::receiveDPIChanged);
    this->window_screen = nullptr;

    QMainWindow::hide();
}

/*
Slot: receives screen change event of this window.
*/
void Controller::receiveScreenChanged(QScreen* screen){
    QObject::disconnect(this->window_screen, &QScreen::logicalDotsPerInchChanged, this, &Main::Controller::receiveDPIChanged);
    this->window_screen = screen;
    QObject::connect(this->window_screen, &QScreen::logicalDotsPerInchChanged, this, &Main::Controller::receiveDPIChanged);

    //qDebug() << "screenChanged > em:" << QFontMetrics(this->font(), this).width("M") << "px";
    emit this->screenChanged(this);
}

/*
Slot: receives screen DPI change events of this window
*/
void Controller::receiveDPIChanged(qreal dpi){
    Q_UNUSED(dpi);

    //qDebug() << "dpiChanged" << dpi << "> em:" << QFontMetrics(this->font(), this).width("M") << "px";
    emit this->screenDPIChanged(this);
}

/*
Slot: forwards the event to this->sendGlobalEvent();
*/
void Controller::receiveGlobalEvent(QEvent* event){
    emit this->sendGlobalEvent(event);
}

/*
Slot: forwards the new data signal
*/
void Controller::receiveData(const Data::FluorophoreReader& data){
    emit this->sendData(data);
}

/*
Slot: forwards the new instrument signal
*/
void Controller::receiveInstrument(const Data::Instrument& instrument){
    emit this->sendInstrument(instrument);
}

/*
Slot: forwards the request to the cache to update
*/
void Controller::receiveCacheRequestUpdate(){
    emit this->sendCacheRequestUpdate();
}

/*
Slot: forwards the cache add event
*/
void Controller::receiveCacheAdd(std::vector<Data::FluorophoreID>& flourophores){
    emit this->sendCacheAdd(flourophores);
}

/*
Slot: forwards the cache remove event
*/
void Controller::receiveCacheRemove(std::vector<Data::FluorophoreID>& flourophores){
    emit this->sendCacheRemove(flourophores);
}

/*
Slot: forwards the laser event
*/
void Controller::receiveLaser(int wavelenght){
    emit this->sendLaser(wavelenght);
}

/*
Slot: forwards the cache'ssynchronisation request
*/
void Controller::receiveCacheState(const std::vector<Cache::ID>& cache_state){
    emit this->sendCacheState(cache_state);
}

/*
Slot: forwards the cache's update request
*/
void Controller::receiveCacheUpdate(){
    emit this->sendCacheUpdate();
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

/*
Slot: receives and forwards graph select signal
    :param index: the selected graphs index
    :param state: the selections state
*/
void Controller::receiveGraphSelect(std::size_t index, bool state){
    emit this->sendGraphSelect(index, state);
}

/*
Slot: receives and forwards Graph Set signal
    :param number: the amount of graphs that should exist
*/
void Controller::receiveGraphState(std::vector<State::GraphState>& state){
    emit this->sendGraphState(state);
}

} // Main namespace
