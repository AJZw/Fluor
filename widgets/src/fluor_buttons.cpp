/**** General **************************************************************
** Version:    v0.9.1
** Date:       2019-02-03
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2022 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include "fluor_buttons.h"

#include <QDebug>
#include <QGridLayout>
#include <QStyle>
#include <QVariant>

namespace Fluor {

/*
Constructor: builds a PushButton
*/
PushButton::PushButton(QWidget* parent) : QPushButton(parent) {
  this->setText("Fluorochromes");
  this->setToolTip("Add Fluorochromes");
}

/*
Slot: hides button
*/
void PushButton::hideButton() { this->hide(); }

/*
Slot: shows button
*/
void PushButton::showButton() { this->show(); }

// #################################################################### //

/*
Constructor: builds a Emission PushButton
*/
EmissionButton::EmissionButton(QWidget* parent)
    : QPushButton(parent), tooltip_active("Toggle emission (on)"), tooltip_inactive("Toggle emission (off)") {
  this->installEventFilter(this);

  this->setEnabled(true);
  this->setText("");

  this->setProperty("active", true);
  this->setToolTip();

  this->setProperty("select", false);

  QObject::connect(this, &QAbstractButton::clicked, this, &EmissionButton::toggleActive);
  QObject::connect(this, &QAbstractButton::clicked, this, &EmissionButton::click);
}

/*
Set tooltip based on tooltip_disabled/active/inactive strings
*/
void EmissionButton::setToolTip() {
  if (this->property("active").toBool()) {
    QWidget::setToolTip(this->tooltip_active);
  } else {
    QWidget::setToolTip(this->tooltip_inactive);
  }
}

/*
Captures and handles mouse events of this button. This is necessary for the selection handling
  :param obj: object that captured the event
  :param event: the event
*/
bool EmissionButton::eventFilter(QObject* obj, QEvent* event) {
  switch (event->type()) {
    case QEvent::HoverEnter:
      emit this->hoverEntered();
      break;
    case QEvent::HoverLeave:
      emit this->hoverLeaved();
      break;
    default:
      break;
  }
  return QPushButton::eventFilter(obj, event);
}

/*
Slot: receives click signal, builds and fires clicked(active) signal
  :param checked: (unused) whether the button is checked. Required for signal-slot mechanism.
*/
void EmissionButton::click(bool checked) {
  Q_UNUSED(checked);
  emit this->clicked(this->property("active").toBool());
}

/*
Slot: set 'active' state of the button
  :param active: the active state to set the button to
*/
void EmissionButton::setActive(bool active) {
  if (this->property("active").toBool() == active) {
    return;
  }
  this->setProperty("active", active);

  this->setToolTip();

  this->style()->unpolish(this);
  this->style()->polish(this);
}

/*
Slot: set 'select' state of the button
  :param select: the select state to set the button to
*/
void EmissionButton::setSelect(bool select) {
  if (this->property("select").toBool() == select) {
    return;
  }
  this->setProperty("select", select);

  this->style()->unpolish(this);
  this->style()->polish(this);

  if (this->property("select").toBool()) {
    emit this->selected();
  }
}

/*
Slot: changes button from active to inactive, or visa versa
  :param checked: NOT USED
*/
void EmissionButton::toggleActive(bool checked) {
  Q_UNUSED(checked);

  this->setActive(!this->property("active").toBool());
}

// ################################################################## //

/*
Constructor: builds a Excitation PushButton
*/
ExcitationButton::ExcitationButton(QWidget* parent)
    : QPushButton(parent), tooltip_active("Toggle excitation (on)"), tooltip_inactive("Toggle excitation (off)") {
  this->installEventFilter(this);
  this->setEnabled(true);
  this->setText("");

  this->setProperty("active", true);
  this->setToolTip();

  QObject::connect(this, &QAbstractButton::clicked, this, &ExcitationButton::toggleActive);
  QObject::connect(this, &QAbstractButton::clicked, this, &ExcitationButton::click);
}

/*
Set tooltip based on tooltip_disabled/active/inactive strings
*/
void ExcitationButton::setToolTip() {
  if (this->property("active").toBool()) {
    QWidget::setToolTip(this->tooltip_active);
  } else {
    QWidget::setToolTip(this->tooltip_inactive);
  }
}

/*
Captures and handles mouse events of this button. This is necessary for the selection handling
  :param obj: object that captured the event
  :param event: the event
*/
bool ExcitationButton::eventFilter(QObject* obj, QEvent* event) {
  switch (event->type()) {
    case QEvent::HoverEnter:
      emit this->hoverEntered();
      break;
    case QEvent::HoverLeave:
      emit this->hoverLeaved();
      break;
    default:
      break;
  }
  return QPushButton::eventFilter(obj, event);
}

/*
Slot: receives click signal, builds and fires clicked(active) signal
  :param checked: (unused) whether the button is checked. Required for signal-slot mechanism.
*/
void ExcitationButton::click(bool checked) {
  Q_UNUSED(checked);
  emit this->clicked(this->property("active").toBool());
}

/*
Slot: set 'active' state of the button
*/
void ExcitationButton::setActive(bool active) {
  if (this->property("active").toBool() == active) {
    return;
  }
  this->setProperty("active", active);

  this->setToolTip();

  this->style()->unpolish(this);
  this->style()->polish(this);
}

/*
Slot: changes button from active to inactive, or visa versa
  :param checked: NOT USED
*/
void ExcitationButton::toggleActive(bool checked) {
  Q_UNUSED(checked);

  this->setActive(!this->property("active").toBool());
}

// ################################################################## //

/*
Constructor: builds a Remove PushButton
*/
RemoveButton::RemoveButton(QWidget* parent)
    : QPushButton(parent), tooltip_active("Remove fluorophore"), tooltip_inactive("Remove fluorophore") {
  this->installEventFilter(this);

  this->setEnabled(true);
  this->setText("");

  this->setProperty("active", true);
  this->setToolTip();

  QObject::connect(this, &QAbstractButton::clicked, this, &RemoveButton::click);
}

/*
Slot: set 'active' state of the button
*/
void RemoveButton::setActive(bool active) {
  if (this->property("active").toBool() == active) {
    return;
  }
  this->setProperty("active", active);

  this->setToolTip();

  this->style()->unpolish(this);
  this->style()->polish(this);
}

/*
Set tooltip based on tooltip_disabled/active/inactive strings
*/
void RemoveButton::setToolTip() {
  if (this->property("active").toBool()) {
    QWidget::setToolTip(this->tooltip_active);
  } else {
    QWidget::setToolTip(this->tooltip_inactive);
  }
}

/*
Captures and handles mouse events of this button. This is necessary for the selection handling
  :param obj: object that captured the event
  :param event: the event
*/
bool RemoveButton::eventFilter(QObject* obj, QEvent* event) {
  switch (event->type()) {
    case QEvent::HoverEnter:
      emit this->hoverEntered();
      break;
    case QEvent::HoverLeave:
      emit this->hoverLeaved();
      break;
    default:
      break;
  }
  return QPushButton::eventFilter(obj, event);
}

/*
Slot: receives click signal, builds and fires clicked(active) signal
  :param checked: (unused) whether the button is checked. Required for signal-slot mechanism.
*/
void RemoveButton::click(bool checked) {
  Q_UNUSED(checked);
  emit this->clicked(this->property("active").toBool());
}

}  // namespace Fluor
