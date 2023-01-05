/**** General **************************************************************
** Version:    v0.9.13
** Date:       2020-11-09
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2022 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include "main_controller.h"

#include <QApplication>
#include <QDebug>
#include <QEvent>
#include <QWindow>

#include "application.h"
#include "central_controller.h"
#include "main_menu_bar.h"

namespace Main {

/*
Initiliaser - builds a QMainWindow widget and set the connections
*/
Controller::Controller(QWidget* parent) : QMainWindow(parent), window_screen(nullptr), window_width(600), window_height(300) {
  // Set MainWindow properties
  this->setWindowTitle("Fluor");
  this->setGeometry(0, 0, this->window_width, this->window_height);
  this->setMinimumSize(this->window_width, this->window_height);
  this->setContentsMargins(0, 0, 0, 0);

  // Add menu
  Main::MenuBar* menu_bar = nullptr;
  if (QSysInfo().productType() == "osx") {
    // For macOS an application has only one menubar application wide,
    // this is simulated by not adding a parent to the menubar.
    // As this mainwindow is only made once this only dangles when the entire application is closed, so irrelevant
    menu_bar = new Main::MenuBar(nullptr);
  } else {
    menu_bar = new Main::MenuBar(this);
    this->setMenuBar(menu_bar);
  }

  // Add widgets
  Central::Controller* controller_widget = new Central::Controller(this);
  this->setCentralWidget(controller_widget);

  this->installEventFilter(this);

  // Receive global events
  QObject::connect(static_cast<Application*>(QApplication::instance()), &Application::globalEvent, this,
                   &Main::Controller::receiveGlobalEvent);

  // Connect signals and slots
  QObject::connect(this, &Main::Controller::sendInstruments, menu_bar, &Main::MenuBar::receiveInstruments);
  QObject::connect(this, &Main::Controller::sendStyles, menu_bar, &Main::MenuBar::receiveStyles);
  QObject::connect(this, &Main::Controller::sendMenuBarStateUpdate, menu_bar, &Main::MenuBar::receiveMenuBarStateUpdate);
  QObject::connect(menu_bar, &Main::MenuBar::sendMenuBarStateChange, this, &Main::Controller::receiveMenuBarStateChange);

  QObject::connect(this, &Main::Controller::sendGlobalEvent, controller_widget, &Central::Controller::receiveGlobalEvent);
  QObject::connect(this, &Main::Controller::resized, controller_widget, &Central::Controller::receiveGlobalSize);
  QObject::connect(this, &Main::Controller::moved, controller_widget, &Central::Controller::receiveGlobalSize);

  QObject::connect(this, &Main::Controller::sendInstrument, controller_widget, &Central::Controller::receiveInstrument);
  QObject::connect(this, &Main::Controller::sendFluorophores, controller_widget, &Central::Controller::receiveFluorophores);

  QObject::connect(controller_widget, &Central::Controller::sendCacheRequestUpdate, this, &Main::Controller::receiveCacheRequestUpdate);
  QObject::connect(controller_widget, &Central::Controller::sendCacheAdd, this, &Main::Controller::receiveCacheAdd);
  QObject::connect(controller_widget, &Central::Controller::sendCacheRemove, this, &Main::Controller::receiveCacheRemove);
  QObject::connect(controller_widget, &Central::Controller::sendLasers, this, &Main::Controller::receiveLasers);
  QObject::connect(this, &Main::Controller::sendCacheState, controller_widget, &Central::Controller::receiveCacheState);
  QObject::connect(this, &Main::Controller::sendCacheUpdate, controller_widget, &Central::Controller::receiveCacheUpdate);

  QObject::connect(controller_widget, &Central::Controller::sendToolbarStateChange, this, &Main::Controller::receiveToolbarStateChange);
  QObject::connect(this, &Main::Controller::sendToolbarStateUpdate, controller_widget, &Central::Controller::receiveToolbarStateUpdate);

  QObject::connect(controller_widget, &Central::Controller::sendGraphSelect, this, &Main::Controller::receiveGraphSelect);
  QObject::connect(this, &Main::Controller::sendGraphState, controller_widget, &Central::Controller::receiveGraphState);
}

/*
eventFilter - captures resize, move, and close event
  :param obj: source object
  :param event: the event
*/
bool Controller::eventFilter(QObject* obj, QEvent* event) {
  Q_UNUSED(obj);
  switch (event->type()) {
    case QEvent::Resize:
      emit this->resized(this);
      return false;
    case QEvent::Move:
      emit this->moved(this);
      return false;
    case QEvent::Close:
      emit this->closed(this);
      return false;
    default:
      break;
  }

  return QMainWindow::eventFilter(obj, event);
}

/*
Shows the widget. Additionally connects the widget to screenChanged and logicalDotsPerInchChanged signals
*/
void Controller::show() {
  QMainWindow::show();

  QObject::connect(this->windowHandle(), &QWindow::screenChanged, this, &Main::Controller::receiveScreenChanged);
  this->window_screen = this->windowHandle()->screen();
  QObject::connect(this->window_screen, &QScreen::logicalDotsPerInchChanged, this, &Main::Controller::receiveDPIChanged);
}

/*
Hides the widget. Additionally disconnects the widget from screenChanged and logicalDotsPerInchChanged signals
*/
void Controller::hide() {
  QObject::disconnect(this->windowHandle(), &QWindow::screenChanged, this, &Main::Controller::receiveScreenChanged);
  QObject::disconnect(this->window_screen, &QScreen::logicalDotsPerInchChanged, this, &Main::Controller::receiveDPIChanged);
  this->window_screen = nullptr;

  QMainWindow::hide();
}

/*
Slot: receives screen change event of this window.
*/
void Controller::receiveScreenChanged(QScreen* screen) {
  QObject::disconnect(this->window_screen, &QScreen::logicalDotsPerInchChanged, this, &Main::Controller::receiveDPIChanged);
  this->window_screen = screen;
  QObject::connect(this->window_screen, &QScreen::logicalDotsPerInchChanged, this, &Main::Controller::receiveDPIChanged);

  // qDebug() << "screenChanged > em:" << QFontMetrics(this->font(), this).width("M") << "px";
  emit this->screenChanged(this);
}

/*
Slot: receives screen DPI change events of this window
*/
void Controller::receiveDPIChanged(qreal dpi) {
  Q_UNUSED(dpi);

  // qDebug() << "dpiChanged" << dpi << "> em:" << QFontMetrics(this->font(), this).width("M") << "px";
  emit this->screenDPIChanged(this);
}

/*
Slot: forwards the event to this->sendGlobalEvent();
*/
void Controller::receiveGlobalEvent(QEvent* event) { emit this->sendGlobalEvent(event); }

/*
Slot: forwards the new data signal
*/
void Controller::receiveFluorophores(const Data::FluorophoreReader& fluorophores) { emit this->sendFluorophores(fluorophores); }

/*
Slot: forwards the new instrument signal
*/
void Controller::receiveInstrument(const Data::Instrument& instrument) { emit this->sendInstrument(instrument); }

/*
Slot: forwards the instrument identifier list signal
*/
void Controller::receiveInstruments(const Data::InstrumentReader& instruments) { emit this->sendInstruments(instruments); }

/*
Slot: forward the style identifiers signal
*/
void Controller::receiveStyles(const std::vector<Data::StyleID>& styles) { emit this->sendStyles(styles); }

/*
Slot: receives menubar state updates (signals from the State::Program)
*/
void Controller::receiveMenuBarStateUpdate(Main::MenuBarAction action, const QVariant& id) {
  emit this->sendMenuBarStateUpdate(action, id);
}

/*
Slot: receives menubar state changes (signals from the Main::MenuBar)
*/
void Controller::receiveMenuBarStateChange(Main::MenuBarAction action, const QVariant& id) {
  emit this->sendMenuBarStateChange(action, id);
}

/*
Slot: forwards the request to the cache to update
*/
void Controller::receiveCacheRequestUpdate() { emit this->sendCacheRequestUpdate(); }

/*
Slot: forwards the cache add event
*/
void Controller::receiveCacheAdd(std::vector<Data::FluorophoreID>& flourophores) { emit this->sendCacheAdd(flourophores); }

/*
Slot: forwards the cache remove event
*/
void Controller::receiveCacheRemove(std::vector<Data::FluorophoreID>& flourophores) { emit this->sendCacheRemove(flourophores); }

/*
Slot: forwards the laser event
*/
void Controller::receiveLasers(std::vector<Data::LaserID>& lasers) { emit this->sendLasers(lasers); }

/*
Slot: forwards the cache'ssynchronisation request
*/
void Controller::receiveCacheState(const std::vector<Cache::ID>& cache_state) { emit this->sendCacheState(cache_state); }

/*
Slot: forwards the cache's update request
*/
void Controller::receiveCacheUpdate() { emit this->sendCacheUpdate(); }

/*
Slot: receives and forwards Toolbar State changes
  :param type: the button that sends the change
  :param active: the active state
  :param enable: the enable state
*/
void Controller::receiveToolbarStateChange(Bar::ButtonType type, bool active, bool enable) {
  emit this->sendToolbarStateChange(type, active, enable);
}

/*
Slot: receives and forwards Toolbar State update
  :param type: the button that should receive the update
  :param active: the active state
  :param enable: the enable state
*/
void Controller::receiveToolbarStateUpdate(Bar::ButtonType type, bool active, bool enable) {
  emit this->sendToolbarStateUpdate(type, active, enable);
}

/*
Slot: receives and forwards graph select signal
  :param index: the selected graphs index
  :param state: the selections state
*/
void Controller::receiveGraphSelect(std::size_t index, bool state) { emit this->sendGraphSelect(index, state); }

/*
Slot: receives and forwards Graph Set signal
  :param number: the amount of graphs that should exist
*/
void Controller::receiveGraphState(std::vector<State::GraphState>& state) { emit this->sendGraphState(state); }

}  // namespace Main
