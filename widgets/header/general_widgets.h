/**** General **************************************************************
** Version:    v0.9.10
** Date:       2020-10-13
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
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

#include <QScrollBar>
#include <QStandarditem>
#include <QStyledItemDelegate>
#include <QPainter>
#include "global.h"

namespace General {

class ScrollBar : public QScrollBar {
    Q_OBJECT

    public:
        explicit ScrollBar(QWidget* parent=nullptr);
        ScrollBar(const ScrollBar &obj) = delete;
        ScrollBar& operator=(const ScrollBar &obj) = delete;
        ScrollBar(ScrollBar&&) = delete;
        ScrollBar& operator=(ScrollBar&&) = delete;
        virtual ~ScrollBar() = default;
    
    protected:
        void hideEvent(QHideEvent* );
        void showEvent(QShowEvent* );

    signals:
        void hiding();
        void showing();
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
        explicit Separator(QWidget* parent=nullptr);
        Separator(const Separator &obj) = default;
        Separator& operator=(const Separator &obj) = default;
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
        explicit SeparatorItem(QWidget* parent=nullptr);
        SeparatorItem(const SeparatorItem &obj) = default;
        SeparatorItem& operator=(const SeparatorItem &obj) = default;
        SeparatorItem(SeparatorItem&&) = default;
        SeparatorItem& operator=(SeparatorItem&&) = default;
        virtual ~SeparatorItem() = default;

    private:
        Separator style;
};

class CustomDelegate : public QStyledItemDelegate {
    Q_OBJECT

    public:
        explicit CustomDelegate(QObject* parent=nullptr);
        CustomDelegate(const CustomDelegate &obj) = delete;
        CustomDelegate& operator=(const CustomDelegate &obj) = delete;
        CustomDelegate(CustomDelegate&&) = delete;
        CustomDelegate& operator=(CustomDelegate&&) = delete;
        virtual ~CustomDelegate() = default;

    public:
        virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
        virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

} // general namespace

#endif // GENERAL_WIDGETS_H
