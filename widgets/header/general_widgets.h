/**** General **************************************************************
** Version:    v0.9.13
** Date:       2020-11-09
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2022 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

/**** LGPLv3 License *******************************************************
** general_widgets.h is part of Fluor
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
** General widgets
**
** :class: General::ScrollBar
** A QScrollBar that emits signals upon showing and hiding
**
** As item-models use QVariants to store data, we make use of identifiers to
** know the type of data stored within the QVariant.
** CustomItemTypeRole - the item data role, stores the custom ItemType
** CustomItemRole - the custom item, stores a pointer to the custom item (example Separator)
** SeparatorItemType - the separator custom ItemType
**
** :class: General::Separator
** The styling and painting class to draw a separation line in a item-model
**
** :class: General::SeparatorItem
** Convenience QStandardItem wrapper of a separator styled item
**
** :class: General::CustomDelegate
** For the painting of custom items a custom delegate is required, this is one.
**
***************************************************************************/

#ifndef GENERAL_WIDGETS_H
#define GENERAL_WIDGETS_H

#include <QLabel>
#include <QPainter>
#include <QScreen>
#include <QScrollBar>
#include <QStandarditem>
#include <QStyledItemDelegate>
#include <QWindow>

#include "global.h"

namespace General {

class ScrollBar : public QScrollBar {
  Q_OBJECT

 public:
  explicit ScrollBar(QWidget* parent = nullptr);
  ScrollBar(const ScrollBar& obj) = delete;
  ScrollBar& operator=(const ScrollBar& obj) = delete;
  ScrollBar(ScrollBar&&) = delete;
  ScrollBar& operator=(ScrollBar&&) = delete;
  virtual ~ScrollBar() = default;

 protected:
  void hideEvent(QHideEvent* event);
  void showEvent(QShowEvent* event);

 signals:
  void hiding();
  void showing();
};

class StyledWidget : public QWidget {
  Q_OBJECT
  Q_PROPERTY(QString layout_margins READ layoutMargins WRITE setLayoutMargins)
  Q_PROPERTY(QString layout_spacing READ layoutSpacing WRITE setLayoutSpacing)

 public:
  explicit StyledWidget(QWidget* parent = nullptr);
  StyledWidget(const StyledWidget& obj) = delete;
  StyledWidget& operator=(const StyledWidget& obj) = delete;
  StyledWidget(StyledWidget&&) = delete;
  StyledWidget& operator=(StyledWidget&&) = delete;
  virtual ~StyledWidget() = default;

 private:
  QScreen* window_screen;

 public:
  void paintEvent(QPaintEvent* event);
  QString layoutMargins() const;
  void setLayoutMargins(QString layout_spacing);
  QString layoutSpacing() const;
  void setLayoutSpacing(QString layout_spacing);

 public slots:
  virtual void show();
  virtual void hide();

  void receiveScreenChanged(QScreen* screen);
  void receiveDPIChanged(qreal dpi);

 signals:
  void screenChanged(QWidget* widget);
  void screenDPIChanged(QWidget* widget);
};

class AboutIcon : public QLabel {
  Q_OBJECT
  Q_PROPERTY(QString scale READ iconScale WRITE setIconScale)

 public:
  explicit AboutIcon(QWidget* parent = nullptr);
  AboutIcon(const AboutIcon& obj) = delete;
  AboutIcon& operator=(const AboutIcon& obj) = delete;
  AboutIcon(AboutIcon&&) = delete;
  AboutIcon& operator=(AboutIcon&&) = delete;
  virtual ~AboutIcon() = default;

 private:
  QSize sizeHint() const;
  QSize minimumSizeHint() const;

  QString iconScale() const;
  void setIconScale(QString scale);
};

class AboutWindow : public StyledWidget {
  Q_OBJECT

 public:
  explicit AboutWindow(QWidget* parent = nullptr);
  AboutWindow(const AboutWindow& obj) = delete;
  AboutWindow& operator=(const AboutWindow& obj) = delete;
  AboutWindow(AboutWindow&&) = delete;
  AboutWindow& operator=(AboutWindow&&) = delete;
  virtual ~AboutWindow() = default;
};

extern const int CustomItemTypeRole;
extern const int CustomItemRole;
extern const int SeparatorItemType;

class Separator : public QWidget {
  Q_OBJECT
  Q_PROPERTY(int separator_height READ separatorHeight WRITE setSeparatorHeight)
  Q_PROPERTY(QString separator_color READ separatorColor WRITE setSeparatorColor)
  Q_PROPERTY(int separator_width READ separatorWidth WRITE setSeparatorWidth)

 public:
  explicit Separator(QWidget* parent = nullptr);
  Separator(const Separator& obj) = default;
  Separator& operator=(const Separator& obj) = default;
  Separator(Separator&&) = default;
  Separator& operator=(Separator&&) = default;
  virtual ~Separator() = default;

 private:
  QPen pen;
  int height;

 public:
  int separatorHeight() const;
  void setSeparatorHeight(int height);
  QString separatorColor() const;
  void setSeparatorColor(QString color);
  int separatorWidth() const;
  void setSeparatorWidth(int width);

  void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
  using QWidget::sizeHint;
  QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
};

class SeparatorItem : public QStandardItem {
 public:
  explicit SeparatorItem(QWidget* parent = nullptr);
  SeparatorItem(const SeparatorItem& obj) = default;
  SeparatorItem& operator=(const SeparatorItem& obj) = default;
  SeparatorItem(SeparatorItem&&) = default;
  SeparatorItem& operator=(SeparatorItem&&) = default;
  virtual ~SeparatorItem() = default;

 private:
  Separator style;
};

class CustomDelegate : public QStyledItemDelegate {
  Q_OBJECT

 public:
  explicit CustomDelegate(QObject* parent = nullptr);
  CustomDelegate(const CustomDelegate& obj) = delete;
  CustomDelegate& operator=(const CustomDelegate& obj) = delete;
  CustomDelegate(CustomDelegate&&) = delete;
  CustomDelegate& operator=(CustomDelegate&&) = delete;
  virtual ~CustomDelegate() = default;

 public:
  virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
  virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

}  // namespace General

#endif  // GENERAL_WIDGETS_H
