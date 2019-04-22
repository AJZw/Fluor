/**** General **************************************************************
** Version:    v0.9.2
** Date:       2019-02-03
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include "fluor_controller.h"

#include <QVBoxLayout>

#include "fluor_buttons.h"
#include "fluor_lineedit.h"
#include "fluor_scrollarea.h"
#include <QStyle>
#include <QStyleOption>
#include <QPainter>
#include <QDebug>

namespace Fluor {

/*
Initializer: Builds and connects the Fluorophore menu
    :parent: parent widget
*/
Controller::Controller(QWidget* parent) :
    QWidget(parent)
{
    this->setContentsMargins(0, 0, 0, 0);

    // Set layout
    QVBoxLayout* controller_layout = new QVBoxLayout(this);
    controller_layout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(controller_layout);

    // Add buttons
    Fluor::PushButton* widget_button = new Fluor::PushButton(this);
    Fluor::LineEdit* widget_lineedit = new Fluor::LineEdit(this);
    Fluor::ScrollArea* widget_scrollarea = new Fluor::ScrollArea(this);
    widget_lineedit->hide();

    controller_layout->addWidget(widget_button, 0);
    controller_layout->addWidget(widget_lineedit, 0);
    controller_layout->addWidget(widget_scrollarea, 1);

    // Connect internal signals and slots
    // FluorPushButton and FluorLineEdit connections
    QObject::connect(widget_button, &Fluor::PushButton::clicked, this, &Fluor::Controller::clickedPushButton);
    QObject::connect(widget_lineedit, &Fluor::LineEdit::finished, this, &Fluor::Controller::finishedLineEdit);
    QObject::connect(this, &Fluor::Controller::showPushButton, widget_button, &Fluor::PushButton::showButton);
    QObject::connect(this, &Fluor::Controller::hidePushButton, widget_button, &Fluor::PushButton::hideButton);
    QObject::connect(this, &Fluor::Controller::showLineEdit, widget_lineedit, &Fluor::LineEdit::showButton);
    QObject::connect(this, &Fluor::Controller::hideLineEdit, widget_lineedit, &Fluor::LineEdit::hideButton);

    // Forwards the events to/from LineEdit
    QObject::connect(this, &Fluor::Controller::sendGlobalEvent, widget_lineedit, &Fluor::LineEdit::unfocus);
    QObject::connect(this, &Fluor::Controller::reloadedData, widget_lineedit, &Fluor::LineEdit::reloadCompleterModel);
    QObject::connect(this, &Fluor::Controller::reloadedGlobalSize, widget_lineedit, &Fluor::LineEdit::reloadSize);
    QObject::connect(widget_lineedit, &Fluor::LineEdit::output, this, &Fluor::Controller::output);
}

/*
Reimplementation of paintEvent. This allows proper stylesheet management of QWidget inheriting widget
    :param event: paintevent
*/
void Controller::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QStyleOption style_option;
    style_option.initFrom(this);
    QPainter painter(this);
    this->style()->drawPrimitive(QStyle::PE_Widget, &style_option, &painter, this);
}

/*
Slot: unfocus the widget
*/
void Controller::receiveGlobalEvent(QEvent* event){
    emit this->sendGlobalEvent(event);
}

/*
Slot: reload the data of the widget
*/
void Controller::reloadData(const DataFluorophores* data){
    emit this->reloadedData(data);
}

/*
Slot: reload the max size of a (ListView) widget
*/
void Controller::reloadGlobalSize(const QWidget* widget){
    emit this->reloadedGlobalSize(widget);
}

/*
Slot: slot for FluorPushButton::clicked, hides PushButton and show LineEdit
*/
void Controller::clickedPushButton(bool checked) {
    Q_UNUSED(checked);
    emit this->hidePushButton();
    emit this->showLineEdit();
}

/*
Slot: slot for FluorLineEdit::finished, hides LineEdit and shows Pushbutton
*/
void Controller::finishedLineEdit() {
    emit this->hideLineEdit();
    emit this->showPushButton();
}

} // Fluor namespace
