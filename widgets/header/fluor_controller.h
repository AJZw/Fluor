/**** General **************************************************************
** Version:    v0.9.13
** Date:       2020-11-09
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2022 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

/**** LGPLv3 License *******************************************************
** fluor_controller.h is part of Fluor
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
** The fluorophore menu controller
**
** :class: Fluor::Controller
** Builds and controlls the fluorophore menu. Combines a FLuorLineEdit with
** a QScrollWidget and the fluorophore buttons
** Internally, connects the signal and slots of internal events and forms an
** interface for external signals and slots
**
** :class: Fluor::ButtonsController
** Controller for emission, excitation visibility and remove button of a fluorophore
**
** :class: Fluor::ScrollController
** Controller for the scrollarea
**
***************************************************************************/

#ifndef FLUOR_CONTROLLER_H
#define FLUOR_CONTROLLER_H

#include <QPaintEvent>
#include <QScrollArea>
#include <QScrollBar>
#include <QStringList>
#include <QWidget>
#include <vector>

#include "cache.h"
#include "data_fluorophores.h"
#include "fluor_buttons.h"

namespace Fluor {

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

 public slots:
  void receiveGlobalEvent(QEvent* event);
  void receiveGlobalSize(const QWidget* widget = nullptr);
  void receiveFluorophores(const Data::FluorophoreReader& fluorophores);

  void receiveCacheRequestUpdate();
  void receiveCacheAdd(std::vector<Data::FluorophoreID>& fluorophores);
  void receiveCacheRemove(std::vector<Data::FluorophoreID>& fluorophores);
  void receiveCacheState(const std::vector<Cache::ID>& cache_state);
  void receiveCacheUpdate();

 private slots:
  void clickedPushButton(bool checked);
  void finishedLineEdit();

 signals:
  void sendGlobalEvent(QEvent* event);
  void sendGlobalSize(const QWidget* widget = nullptr);
  void sendFluorophores(const Data::FluorophoreReader& fluorophores);

  void showPushButton();
  void hidePushButton();
  void showLineEdit();
  void hideLineEdit();

  void sendCacheAdd(std::vector<Data::FluorophoreID>& fluorophores);
  void sendCacheRemove(std::vector<Data::FluorophoreID>& fluorophores);
  void sendCacheState(const std::vector<Cache::ID>& cache_state);
  void sendCacheUpdate();
  void sendCacheRequestUpdate();
};

class ButtonsController : public QWidget {
  Q_OBJECT
  Q_PROPERTY(QString layout_spacing READ layoutSpacing WRITE setLayoutSpacing)

 public:
  explicit ButtonsController(QWidget* parent = nullptr);
  ButtonsController(const ButtonsController& obj) = delete;
  ButtonsController& operator=(const ButtonsController& obj) = delete;
  ButtonsController(ButtonsController&&) = delete;
  ButtonsController& operator=(ButtonsController&&) = delete;
  ~ButtonsController() = default;

  void paintEvent(QPaintEvent* event);
  QString layoutSpacing() const;
  void setLayoutSpacing(QString layout_spacing);

  void syncButtons(const Cache::ID& cache_state);
  void updateButtons();

 private:
  Fluor::EmissionButton* widget_emission;
  Fluor::ExcitationButton* widget_excitation;
  Fluor::RemoveButton* widget_remove;

  QString id;
  QString name;
  Data::CacheSpectrum* data;

 signals:
  void requestUpdate();
  void sendRemove(std::vector<Data::FluorophoreID>& fluorophores);
  void sendSelected(QWidget* widget);

 public slots:
  void receiveEmissionClick(bool active);
  void receiveExcitationClick(bool active);
  void receiveRemoveClick();

 private slots:
  void hoverEntered();
  void hoverLeaved();
  void receiveSelected();
};

class ScrollController : public QScrollArea {
  Q_OBJECT
  Q_PROPERTY(QString layout_spacing READ layoutSpacing WRITE setLayoutSpacing)
  Q_PROPERTY(QString layout_margins_scroll_bar READ layoutMarginsScrollBar WRITE setLayoutMarginsScrollBar)

 public:
  explicit ScrollController(QWidget* parent = nullptr);
  ScrollController(const ScrollController& obj) = delete;
  ScrollController& operator=(const ScrollController& obj) = delete;
  ScrollController(ScrollController&&) = delete;
  ScrollController& operator=(ScrollController&&) = delete;
  virtual ~ScrollController() = default;

  QString layoutSpacing() const;
  void setLayoutSpacing(QString layout_spacing);
  QString layoutMarginsScrollBar() const;
  void setLayoutMarginsScrollBar(QString layout_spacing_scroll_bar);

 private:
  std::vector<ButtonsController*> button_widgets;
  int margin_scrollbar;

 signals:
  void sendCacheRequestUpdate();
  void sendRemove(std::vector<Data::FluorophoreID>& fluorophores);

 protected:
  virtual void resizeEvent(QResizeEvent* event) override;

 public slots:
  void hidingScrollBar();
  void showingScrollBar();

  void syncButtons(const std::vector<Cache::ID>& cache_state);
  void updateButtons();

  void receiveCacheRequestUpdate();
  void receiveRemove(std::vector<Data::FluorophoreID>& fluorophores);

 private slots:
  void receiveSelected(QWidget* widget);
};

}  // namespace Fluor

#endif  // FLUOR_CONTROLLER_H