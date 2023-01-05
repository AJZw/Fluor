/**** General **************************************************************
** Version:    v0.9.13
** Date:       2020-11-09
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2022 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

/**** LGPLv3 License *******************************************************
** central_controller.h is part of Fluor
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
** Controller of central widget
**
** :class: Central::Controller
** Controls the central_widget layout and connects the signals and slots for
** the laser menu, toolbar buttons, fluorophore menu, and graphs
**
***************************************************************************/

#ifndef CENTRAL_CONTROLLER_H
#define CENTRAL_CONTROLLER_H

#include <QPaintEvent>
#include <QWidget>
#include <vector>

#include "cache.h"
#include "data_fluorophores.h"
#include "data_instruments.h"
#include "state_gui.h"
#include "toolbar_controller.h"

namespace Central {

class Controller : public QWidget {
  Q_OBJECT
  Q_PROPERTY(QString layout_margins READ layoutMargins WRITE setLayoutMargins)
  Q_PROPERTY(QString layout_spacing READ layoutSpacing WRITE setLayoutSpacing)

 public:
  explicit Controller(QWidget* parent = nullptr);
  Controller(const Controller& obj) = delete;
  Controller& operator=(const Controller& obj) = delete;
  Controller(Controller&&) = delete;
  Controller& operator=(Controller&&) = delete;
  ~Controller() = default;

  void paintEvent(QPaintEvent* event);
  QString layoutMargins() const;
  void setLayoutMargins(QString layout_spacing);
  QString layoutSpacing() const;
  void setLayoutSpacing(QString layout_spacing);

 public slots:
  void receiveGlobalEvent(QEvent* event);
  void receiveFluorophores(const Data::FluorophoreReader& fluorophores);
  void receiveInstrument(const Data::Instrument& instrument);
  void receiveGlobalSize(const QWidget* widget = nullptr);

  void receiveCacheRequestUpdate();
  void receiveCacheAdd(std::vector<Data::FluorophoreID>& fluorophores);
  void receiveCacheRemove(std::vector<Data::FluorophoreID>& fluorophores);

  void receiveCacheState(const std::vector<Cache::ID>& cache_state);
  void receiveCacheUpdate();

  void receiveLasers(std::vector<Data::LaserID>& lasers);

  void receiveToolbarStateChange(Bar::ButtonType type, bool active, bool enable);
  void receiveToolbarStateUpdate(Bar::ButtonType type, bool active, bool enable);

  void receiveGraphSelect(std::size_t index, bool state);
  void receiveGraphState(std::vector<State::GraphState>& state);

 signals:
  void sendGlobalEvent(QEvent* event);
  void sendGlobalSize(const QWidget* widget = nullptr);
  void sendFluorophores(const Data::FluorophoreReader& fluorophores);
  void sendInstrument(const Data::Instrument& instrument);

  void sendCacheRequestUpdate();
  void sendCacheAdd(std::vector<Data::FluorophoreID>& fluorophores);
  void sendCacheRemove(std::vector<Data::FluorophoreID>& fluorophores);

  void sendCacheState(const std::vector<Cache::ID>& cache_state);
  void sendCacheUpdate();

  void sendLasers(std::vector<Data::LaserID>& lasers);

  void sendToolbarStateChange(Bar::ButtonType type, bool active, bool enable = true);
  void sendToolbarStateUpdate(Bar::ButtonType type, bool active, bool enable = true);

  void sendGraphSelect(std::size_t index, bool state);
  void sendGraphState(std::vector<State::GraphState>& state);
};

}  // namespace Central

#endif  // CENTRAL_CONTROLLER_H