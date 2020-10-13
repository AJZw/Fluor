/**** General **************************************************************
** Version:    v0.9.10
** Date:       2020-10-13
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include "general_widgets.h"

#include <QDebug>

namespace General {

const int CustomItemTypeRole(Qt::UserRole + 1000);
const int CustomItemRole(Qt::UserRole + 1001);

const int SeparatorItemType(QStandardItem::UserType + 1);

/*
Constructor of a ScrollBar
    :param parent: parent widget
*/
ScrollBar::ScrollBar(QWidget *parent) :
    QScrollBar(parent)
{}

/*
Upon hiding, emits the hiding() signal
    :param event: the hide event
*/
void ScrollBar::hideEvent(QHideEvent* event){
    emit this->hiding();
    QScrollBar::hideEvent(event);
}

/*
Upon showing, emits the showing signal
    :param event: the show event
*/
void ScrollBar::showEvent(QShowEvent* event){
    emit this->showing();
    QScrollBar::showEvent(event);
}

// ########################################################### //

/*
Constructor for a separator (---) item for QListViews
    :param parent: parent widget
*/
Separator::Separator(QWidget* parent): 
    QWidget(parent),
    pen("#FFFFFF"),
    height(6)
{
    this->setVisible(false);
}

/*
Getter and setter for the separator height
*/
void Separator::setSeparatorHeight(int height){
    this->height = height;
}
int Separator::separatorHeight() const {
    return this->height;
}

/*
Getter and setter for the separator line color
*/
void Separator::setSeparatorColor(QString color){
    this->pen.setColor(QColor(color));
}
QString Separator::separatorColor() const {
    return this->pen.color().name();
}

/*
Getter and setter for the separator line width
*/
void Separator::setSeparatorWidth(int width){
    this->pen.setWidth(width);
}
int Separator::separatorWidth() const {
    return this->pen.width();
}

void Separator::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const{
    Q_UNUSED(index);

    painter->fillRect(option.rect, option.backgroundBrush);
        
    painter->save();
    painter->setPen(this->pen);

    // Place line at center of drawing line
    qreal y_mid = static_cast<qreal>(option.rect.center().y());

    painter->drawLine(QPointF(option.rect.left(), y_mid), QPointF(option.rect.right(), y_mid));
    painter->restore();
}

/*
Implements the sizeHint function for proper drwawing by the CustomDelegate.
    :param option: the style options
    :param index: the index of the item to draw
*/
QSize Separator::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {   
    Q_UNUSED(index);
    QSize size = option.rect.size();
    int height = this->height;

    if(this->pen.width() > height){
        height = this->pen.width();
    }else if(((height - this->pen.width()) & 1) != 0){
        height -= 1;
    }

    size.setHeight(height);
    return size;
}

// ########################################################### //

/*
Separator QStandardItem. 
    :param parent: for proper stylesheet propagation the internal style widget needs to be parented to the window
*/
SeparatorItem::SeparatorItem(QWidget* parent) :
    QStandardItem(),
    style(parent)
{
    this->setData(SeparatorItemType, CustomItemTypeRole);
    this->setData(QVariant::fromValue<Separator*>(&this->style), CustomItemRole);
}

// ########################################################### //

/*
Constructor:
*/
CustomDelegate::CustomDelegate(QObject* parent) :
    QStyledItemDelegate(parent)
{}

/*
Reimplement the paint function to properly paint SeparatorItem's
    :param painter: the painter that is used for drawing
    :param option: the style options
    :param index: the index of the item to draw
*/
void CustomDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const{
    // Get pointer to object   
    if(index.data(CustomItemTypeRole).canConvert<int>()){
        int custom_item_type = qvariant_cast<int>(index.data(CustomItemTypeRole));

        if(custom_item_type == SeparatorItemType){
            //QStyledItemDelegate::paint(painter, option, index);
            Separator* item = qvariant_cast<Separator*>(index.data(CustomItemRole));
            item->paint(painter, option, index);
        }else{
            qDebug() << "CustomDelegate: unknown custom item type";
        }

        return;
    }else{
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }
}

/*
Reimplements the sizeHint function to properly paint SeparatorItem's
    :param option: the style options
    :param index: the index of the item to draw
*/
QSize CustomDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const{
    if(index.data(CustomItemTypeRole).canConvert<int>()){
        int custom_item_type = qvariant_cast<int>(index.data(CustomItemTypeRole));

        if(custom_item_type == SeparatorItemType){
            Separator* item = qvariant_cast<Separator*>(index.data(CustomItemRole));
            return item->sizeHint(option, index);
        }else{
            qDebug() << "CustomDelegate: unknown custom item type";
        }
        return QSize(option.rect.size());
    }else{
        return QStyledItemDelegate::sizeHint(option, index);
    }
}

} // general namespace
