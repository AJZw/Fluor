/**** General **************************************************************
** Version:    v0.9.1
** Date:       2019-03-11
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2022 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

/**** LGPLv3 License *******************************************************
** toolbar_controller.h is part of Fluor
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
** Controller of the toolbar
**
** :class: Bar::Controller
** Controls the signal and slots of the toolbar widgets
**
***************************************************************************/

#ifndef TOOLBAR_CONTROLLER_H
#define TOOLBAR_CONTROLLER_H

#include <QPaintEvent>
#include <QWidget>

#include "global.h"

namespace Bar {

class Controller : public QWidget {
  Q_OBJECT
  Q_PROPERTY(QString layout_spacing READ layoutSpacing WRITE setLayoutSpacing)

 public:
  explicit Controller(QWidget* parent = nullptr);
  Controller(const Controller& obj) = delete;
  Controller& operator=(const Controller& obj) = delete;
  Controller(Controller&&) = delete;
  Controller& operator=(Controller&&) = delete;
  ~Controller() = default;

  void paintEvent(QPaintEvent* event);

  QString layoutSpacing() const;
  void setLayoutSpacing(QString layout_spacing);

  // global
 signals:
  void sendToolbarStateChange(Bar::ButtonType type, bool active, bool enable = true);

 public slots:
  void receiveToolbarStateUpdate(Bar::ButtonType type, bool active, bool enable);

  // local
 signals:
  void enableLaser(bool enable);
  void enableFilter(bool enable);
  void enableLaserLines(bool enable);

  void activateLaser(bool active);
  void activateExcitation(bool active);
  void activateEmission(bool active);
  void activateFilter(bool active);
  void activateGraphAdd(bool active);
  void activateGraphRemove(bool active);
  void activateLaserLines(bool active);

 private slots:
  void clickedLaser(bool active);
  void clickedExcitation(bool active);
  void clickedEmission(bool active);
  void clickedFilter(bool active);
  void clickedGraphAdd(bool active);
  void clickedGraphRemove(bool active);
  void clickedLaserLines(bool active);
};

}  // namespace Bar

#endif  // TOOLBAR_CONTROLLER_H