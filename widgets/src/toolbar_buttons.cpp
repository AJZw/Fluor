/**** General **************************************************************
** Version:    v0.9.1
** Date:       2019-03-11
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2022 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include "toolbar_buttons.h"

#include <QAbstractButton>
#include <QDebug>
#include <QFont>
#include <QFontMetrics>
#include <QSize>
#include <QStyle>

namespace Bar {

/*
Constructor: builds a square pushbutton
  :param parent: parent widget
*/
SquarePushButton::SquarePushButton(QWidget* parent) : QPushButton(parent), tooltip_disabled(), tooltip_active(), tooltip_inactive() {
  QObject::connect(this, &QAbstractButton::clicked, this, &SquarePushButton::toggleActive);
  QObject::connect(this, &QAbstractButton::clicked, this, &SquarePushButton::click);
}

/*
Holds the recommended size for the widget, forcefully returns a square shape
*/
QSize SquarePushButton::sizeHint() const {
  QSize size = std::move(QPushButton::sizeHint());
  int height = size.height();
  int width = size.width();

  if (height >= width) {
    size.setWidth(height);
  } else {
    size.setHeight(width);
  }

  return size;
}

/*
Holds the minimum size of the PushButton, forcefully returns a square shape
*/
QSize SquarePushButton::minimumSizeHint() const {
  QSize size = std::move(QPushButton::minimumSizeHint());
  int height = size.height();
  int width = size.width();

  if (height >= width) {
    size.setWidth(height);
  } else {
    size.setHeight(width);
  }

  return size;
}

/*
Set tooltip based on tooltip_disabled/active/inactive strings
*/
void SquarePushButton::setToolTip() {
  if (!this->isEnabled()) {
    QWidget::setToolTip(this->tooltip_disabled);
  } else if (this->property("active").toBool()) {
    QWidget::setToolTip(this->tooltip_active);
  } else {
    QWidget::setToolTip(this->tooltip_inactive);
  }
}

/*
Slot: hides button
*/
void SquarePushButton::hideButton() { this->hide(); }

/*
Slot: shows button
*/
void SquarePushButton::showButton() { this->show(); }

/*
Slot: receives click signal, builds and fires clicked(active) signal
  :param checked: (unused) whether the button is checked. Required for signal-slot mechanism.
*/
void SquarePushButton::click(bool checked) {
  Q_UNUSED(checked);
  emit this->clicked(this->property("active").toBool());
}

/*
Slot: sets enable, also forces a style repolish
  :param enable: enable state
*/
void SquarePushButton::setEnabled(bool enable) {
  if (this->isEnabled() == enable) {
    return;
  }

  QWidget::setEnabled(enable);

  this->setToolTip();

  this->style()->unpolish(this);
  this->style()->polish(this);
}

/*
Slot: changes button from active to inactive, or visa versa
  :param checked: NOT USED
*/
void SquarePushButton::toggleActive(bool checked) {
  Q_UNUSED(checked);

  this->setActive(!this->property("active").toBool());
}

/*
Slot: set 'active' state of the button and changes icon
*/
void SquarePushButton::setActive(bool active) {
  if (this->property("active").toBool() == active) {
    return;
  }
  this->setProperty("active", active);

  this->setToolTip();

  this->style()->unpolish(this);
  this->style()->polish(this);
}

/*
Constructor: instantiates the IconPushButton
  :param parent: parent widget
*/
IconPushButton::IconPushButton(QWidget* parent) : SquarePushButton(parent) {
  this->setProperty("active", true);

  this->setIconSize(QSize(0, 0));
  this->setContentsMargins(0, 0, 0, 0);
}

/*
Getter for the icon scale; purely implemented for QPROPERTY's requirements
*/
QString IconPushButton::iconScale() const { return QString::number(this->iconSize().width(), 'f', 0); }

/*
Setter: receives stylesheet scaling input
*/
void IconPushButton::setIconScale(QString scale) {
  int size = scale.toInt();
  this->setIconSize(QSize(size, size));
}

/*
Constructor: LaserButton
  :param parent: parent widget
*/
LaserButton::LaserButton(QWidget* parent) : IconPushButton(parent) {
  this->tooltip_disabled = QString("No laser enabled");
  this->tooltip_active = QString("Toggle visibility laser (on)");
  this->tooltip_inactive = QString("Toggle visibilty laser (off)");
  this->setProperty("active", true);
  this->setEnabled(false);
}

/*
Constructor: ExcitationButton.
  :param parent: parent widget
*/
ExcitationButton::ExcitationButton(QWidget* parent) : IconPushButton(parent) {
  this->tooltip_active = QString("Toggle visibility excitation (on)");
  this->tooltip_inactive = QString("Toggle visibilty excitation (off)");
  this->setActive(false);
  this->setEnabled(true);
  this->setToolTip();
}

/*
Constructor: EmissionButton.
  :param parent: parent widget
*/
EmissionButton::EmissionButton(QWidget* parent) : IconPushButton(parent) {
  this->tooltip_active = QString("Toggle visibility emission (on)");
  this->tooltip_inactive = QString("Toggle visibilty emission (off)");
  this->setActive(true);
  this->setEnabled(true);
  this->setToolTip();
}

/*
Constructor: DetectorButton.
  :param parent: parent widget
*/
FilterButton::FilterButton(QWidget* parent) : IconPushButton(parent) {
  this->tooltip_disabled = QString("No filters enabled");
  this->tooltip_active = QString("Toggle visibility filters (on)");
  this->tooltip_inactive = QString("Toggle visibilty filters (off)");
  this->setActive(false);
  this->setEnabled(false);
  this->setToolTip();
}

/*
Constructor: 'plus'-button for adding a graph to graph::scrollarea
  :param parent: parent widget
*/
GraphAddButton::GraphAddButton(QWidget* parent) : IconPushButton(parent) {
  this->tooltip_disabled = QString("Reached maximum graph count");
  this->tooltip_active = QString("Add graph");
  this->setActive(true);
  this->setEnabled(true);
  this->setToolTip();
}

/*
Slot: setActive override, no need for style reloading in GraphAddButton
  :param active: ignored
*/
void GraphAddButton::setActive(bool active) {
  Q_UNUSED(active);
  return;
}

/*
Constructor: 'min'-button for removing a graph from the graph::scrollarea
*/
GraphRemoveButton::GraphRemoveButton(QWidget* parent) : IconPushButton(parent) {
  this->tooltip_disabled = QString("Reached minimum graph count");
  this->tooltip_active = QString("Remove graph");
  this->setActive(true);
  this->setEnabled(false);
  this->setToolTip();
}

/*
Slot: setActive override, no need for style reloading in GraphRemoveButton
  :param active: ignored
*/
void GraphRemoveButton::setActive(bool active) {
  Q_UNUSED(active);
  return;
}

/*
Constructor: Lasers-button for add and removing all lasers from a plot
*/
LaserLinesButton::LaserLinesButton(QWidget* parent) : IconPushButton(parent) {
  this->tooltip_disabled = QString("No instrument enabled");
  this->tooltip_active = QString("Expand laserlines");
  this->tooltip_inactive = QString("Reduce laserlines");
  this->setActive(true);
  this->setEnabled(true);
  this->setToolTip();
}

/*
Constructor: Text button for showing messages in the toolbar
  :param parent: parent widget
*/
Label::Label(QWidget* parent) : QLabel(parent) {
  this->setEnabled(false);
  this->setText("");
}

}  // namespace Bar
