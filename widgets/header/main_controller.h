/**** General **************************************************************
** Version:    v0.9.13
** Date:       2020-11-09
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2022 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

/**** LGPLv3 License *******************************************************
** main_controller.h is part of Fluor
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
** :class: Main::Controller
** Controls the signal and slots of the main widgets
**
***************************************************************************/

#ifndef MAIN_CONTROLLER_H
#define MAIN_CONTROLLER_H

#include <QHideEvent>
#include <QMainWindow>
#include <QScreen>
#include <QShowEvent>
#include <vector>

#include "cache.h"
#include "data_fluorophores.h"
#include "data_instruments.h"
#include "data_styles.h"
#include "global.h"
#include "state_gui.h"

namespace Main {

class Controller : public QMainWindow {
  Q_OBJECT

 public:
  explicit Controller(QWidget* parent = nullptr);
  Controller(const Controller& obj) = delete;
  Controller& operator=(const Controller& obj) = delete;
  Controller(Controller&&) = delete;
  Controller& operator=(Controller&&) = delete;
  ~Controller() = default;

 private:
  QString window_title;
  QScreen* window_screen;
  int window_width;
  int window_height;

  bool eventFilter(QObject* obj, QEvent* event);

 public slots:
  virtual void show();
  virtual void hide();

  void receiveScreenChanged(QScreen* screen);
  void receiveDPIChanged(qreal dpi);

  void receiveGlobalEvent(QEvent* event);
  void receiveFluorophores(const Data::FluorophoreReader& fluorophores);
  void receiveInstrument(const Data::Instrument& instrument);
  void receiveInstruments(const Data::InstrumentReader& instruments);
  void receiveStyles(const std::vector<Data::StyleID>& styles);

  void receiveCacheRequestUpdate();
  void receiveCacheAdd(std::vector<Data::FluorophoreID>& fluorophores);
  void receiveCacheRemove(std::vector<Data::FluorophoreID>& fluorophores);
  void receiveCacheState(const std::vector<Cache::ID>& cache_state);
  void receiveCacheUpdate();

  void receiveLasers(std::vector<Data::LaserID>& lasers);

  void receiveMenuBarStateChange(Main::MenuBarAction action, const QVariant& id = QString());
  void receiveMenuBarStateUpdate(Main::MenuBarAction action, const QVariant& id = QString());

  void receiveToolbarStateChange(Bar::ButtonType type, bool active, bool enable);
  void receiveToolbarStateUpdate(Bar::ButtonType type, bool active, bool enable);

  void receiveGraphSelect(std::size_t index, bool state);
  void receiveGraphState(std::vector<State::GraphState>& state);

 signals:
  void resized(const QWidget* widget);
  void moved(const QWidget* widget);
  void closed(const QWidget* widget);
  void screenChanged(QWidget* widget);
  void screenDPIChanged(QWidget* widget);

  void sendGlobalEvent(QEvent* event);
  void sendFluorophores(const Data::FluorophoreReader& fluorophores);
  void sendInstrument(const Data::Instrument& instrument);
  void sendInstruments(const Data::InstrumentReader& instruments);
  void sendStyles(const std::vector<Data::StyleID>& styles);

  void sendCacheRequestUpdate();
  void sendCacheAdd(std::vector<Data::FluorophoreID>& fluorophores);
  void sendCacheRemove(std::vector<Data::FluorophoreID>& fluorophores);
  void sendCacheState(const std::vector<Cache::ID>& cache_state);
  void sendCacheUpdate();

  void sendLasers(std::vector<Data::LaserID>& lasers);

  void sendMenuBarStateChange(Main::MenuBarAction action, const QVariant& id = QVariant());
  void sendMenuBarStateUpdate(Main::MenuBarAction action, const QVariant& id = QVariant());

  void sendToolbarStateChange(Bar::ButtonType type, bool active, bool enable = true);
  void sendToolbarStateUpdate(Bar::ButtonType type, bool active, bool enable = true);

  void sendGraphSelect(std::size_t index, bool state);
  void sendGraphState(std::vector<State::GraphState>& state);
};

}  // namespace Main

#endif  // MAIN_CONTROLLER_H