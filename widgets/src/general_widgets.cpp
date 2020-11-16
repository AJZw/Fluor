/**** General **************************************************************
** Version:    v0.9.13
** Date:       2020-11-09
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2020 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include "general_widgets.h"

#include <QGridLayout>
#include <QLabel>
#include <QApplication>
#include <QPixmap>
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
Constructor:
*/
StyledWidget::StyledWidget(QWidget* parent) :
    QWidget(parent),
    window_screen(nullptr)
{}

/*
Reimplementation of paintEvent. This allows proper stylesheet management of QWidget inheriting widget
    :param event: paintevent
*/
void StyledWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QStyleOption style_option;
    style_option.initFrom(this);
    QPainter painter(this);
    this->style()->drawPrimitive(QStyle::PE_Widget, &style_option, &painter, this);
}

/*
Getter for layout spacing property, as it has to return a QString it returns the value in pixels
*/
QString StyledWidget::layoutMargins() const {
    return QString::number(this->contentsMargins().left(), 'f', 0);
}

/*
Receives layout scaling properties from the stylesheet
*/
void StyledWidget::setLayoutMargins(QString layout_margins){
    int layout_margin_px = layout_margins.toInt();
    this->setContentsMargins(layout_margin_px, layout_margin_px, layout_margin_px, layout_margin_px);
}

/*
Getter for layout spacing property, as it has to return a QString it returns the value in pixels
*/
QString StyledWidget::layoutSpacing() const {
    return QString::number(this->layout()->spacing(), 'f', 0);
}

/*
Receives layout scaling properties from the stylesheet
*/
void StyledWidget::setLayoutSpacing(QString layout_spacing){
    this->layout()->setSpacing(layout_spacing.toInt());
    this->adjustSize();
}

/*
Shows the widget. Additionally connects the widget to screenChanged and logicalDotsPerInchChanged signals
*/
void StyledWidget::show(){
    QWidget::show();
    
    QObject::connect(this->windowHandle(), &QWindow::screenChanged, this, &General::AboutWindow::receiveScreenChanged);
    this->window_screen = this->windowHandle()->screen();
    QObject::connect(this->window_screen, &QScreen::logicalDotsPerInchChanged, this, &General::AboutWindow::receiveDPIChanged);
}

/*
Hides the widget. Additionally disconnects the widget from screenChanged and logicalDotsPerInchChanged signals
*/
void StyledWidget::hide(){
    QObject::disconnect(this->windowHandle(), &QWindow::screenChanged, this, &General::AboutWindow::receiveScreenChanged);
    QObject::disconnect(this->window_screen, &QScreen::logicalDotsPerInchChanged, this, &General::AboutWindow::receiveDPIChanged);
    this->window_screen = nullptr;

    QWidget::hide();
}

/*
Slot: receives screen change event of this window.
*/
void StyledWidget::receiveScreenChanged(QScreen* screen){
    QObject::disconnect(this->window_screen, &QScreen::logicalDotsPerInchChanged, this, &General::AboutWindow::receiveDPIChanged);
    this->window_screen = screen;
    QObject::connect(this->window_screen, &QScreen::logicalDotsPerInchChanged, this, &General::AboutWindow::receiveDPIChanged);

    //qDebug() << "screenChanged > em:" << QFontMetrics(this->font(), this).width("M") << "px";
    emit this->screenChanged(this);
}

/*
Slot: receives screen DPI change events of this window
*/
void StyledWidget::receiveDPIChanged(qreal dpi){
    Q_UNUSED(dpi);

    //qDebug() << "dpiChanged" << dpi << "> em:" << QFontMetrics(this->font(), this).width("M") << "px";
    emit this->screenDPIChanged(this);
}

// ########################################################### //

/*
Constructor: constructs a Square QLabel containing a QPixmap
*/
AboutIcon::AboutIcon(QWidget* parent) :
    QLabel(parent)
{
    this->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
}

/*
size hint, makes sure the widget is square
*/
QSize AboutIcon::sizeHint() const{
    QSize size = QLabel::sizeHint();
    int height = size.height();
    int width = size.width();

    if(height >= width){
        size.setWidth(height);
    }else{
        size.setHeight(width);
    }

    return size;
}

/*
minimum size hint, makes sure the widget is square
*/
QSize AboutIcon::minimumSizeHint() const{
    QSize size = QLabel::minimumSizeHint();
    int height = size.height();
    int width = size.width();

    if(height >= width){
        size.setWidth(height);
    }else{
        size.setHeight(width);
    }

    return size;
}

/*
Getter for the icon scale; purely implemented for QPROPERTY's requirements
*/
QString AboutIcon::iconScale() const {
    return QString::number(this->pixmap()->width(), 'f', 0);
}

/*
Setter: receives stylesheet scaling input
*/
void AboutIcon::setIconScale(QString scale){
    int size = scale.toInt();
    this->setPixmap(this->pixmap()->scaled(QSize(size, size), Qt::KeepAspectRatio));
}

// ########################################################### //

/*
Constructor for the about window
*/
AboutWindow::AboutWindow(QWidget* parent):
    StyledWidget(parent)
{
    this->setWindowTitle("About Fluor");
    this->setWindowFlags(this->windowFlags() ^ Qt::WindowMinMaxButtonsHint ^ Qt::MSWindowsFixedSizeDialogHint);

    // Set base properties
    this->setContentsMargins(8, 8, 8, 8);

    // Build layout
    QGridLayout* controller_layout = new QGridLayout(this);
    controller_layout->setRowMinimumHeight(0, 1);
    controller_layout->setRowStretch(0, 1);
    controller_layout->setRowStretch(1, 0);
    controller_layout->setRowStretch(2, 0);
    controller_layout->setRowStretch(3, 1);
    controller_layout->setColumnMinimumWidth(0, 80);
    controller_layout->setColumnMinimumWidth(1, 80);
    controller_layout->setColumnStretch(0, 1);
    controller_layout->setColumnStretch(1, 1);
    controller_layout->setColumnStretch(2, 1);
    controller_layout->setContentsMargins(0, 0, 0, 0);
    controller_layout->setSpacing(6);

    QApplication* q_app = static_cast<QApplication*>(QApplication::instance());

    // Set icon element
    AboutIcon* icon = new AboutIcon(this);

    // Set text elements
    QLabel* label_title = new QLabel("Fluor - Spectral Viewer", this);
    label_title->setObjectName("h1");
    QLabel* label_version = new QLabel(" " + q_app->applicationVersion() + " - Beta", this);
    QLabel* label_date = new QLabel(q_app->applicationName(), this);
    QLabel* label_copyright = new QLabel(" Copyright 2020 - AJ Zwijnenburg - all rights reserved", this);
    QLabel* label_qt = new QLabel(" This program uses the Qt library under the GNU Lesser General\n Public Library version 3. You can find a copy of this license at\n https://www.gnu.org/licenses/lgpl-3.0.en.html", this);
    label_qt->setObjectName("h4");

    controller_layout->addWidget(icon, 0, 0, 4, 1);
    controller_layout->addWidget(label_title, 0, 1, 1, 2, Qt::AlignLeft);
    controller_layout->addWidget(label_version, 1, 1, 1, 1, Qt::AlignLeft | Qt::AlignTop);
    controller_layout->addWidget(label_date, 1, 2, 1, 1, Qt::AlignLeft | Qt::AlignTop);
    controller_layout->addWidget(label_copyright, 2, 1, 1, 2, Qt::AlignLeft | Qt::AlignTop);
    controller_layout->addWidget(label_qt, 3, 1, 1, 2, Qt::AlignLeft | Qt::AlignTop);
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
