/**** General **************************************************************
** Version:    v0.9.2
** Date:       2019-02-03
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include "laser_controller.h"

#include <QVBoxLayout>

#include "laser_buttons.h"
#include "laser_lineedit.h"
#include <QStyle>
#include <QStyleOption>
#include <QPainter>

namespace Laser {

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
    Laser::PushButton* widget_button = new Laser::PushButton(this);
    Laser::LineEdit* widget_lineedit = new Laser::LineEdit(this);
    widget_lineedit->hide();

    controller_layout->addWidget(widget_button, 0);
    controller_layout->addWidget(widget_lineedit, 0);

    // Connect internal signals and slots
    QObject::connect(widget_button, &Laser::PushButton::clicked, this, &Laser::Controller::clickedPushButton);
    QObject::connect(widget_lineedit, &Laser::LineEdit::finished, this, &Laser::Controller::finishedLineEdit);
    QObject::connect(this, &Laser::Controller::showPushButton, widget_button, &Laser::PushButton::showButton);
    QObject::connect(this, &Laser::Controller::hidePushButton, widget_button, &Laser::PushButton::hideButton);
    QObject::connect(this, &Laser::Controller::showLineEdit, widget_lineedit, &Laser::LineEdit::showButton);
    QObject::connect(this, &Laser::Controller::hideLineEdit, widget_lineedit, &Laser::LineEdit::hideButton);

    // Forwards the events to/from LineEdit
    QObject::connect(this, &Laser::Controller::sendGlobalEvent, widget_lineedit, &Laser::LineEdit::unfocus);
    QObject::connect(this, &Laser::Controller::sendInstrument, widget_lineedit, &Laser::LineEdit::reloadModel);
    QObject::connect(this, &Laser::Controller::sendGlobalSize, widget_lineedit, &Laser::LineEdit::reloadSize);
    QObject::connect(widget_lineedit, &Laser::LineEdit::output, this, &Laser::Controller::receiveOutput);

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
Slot: reload the max size of a (ListView) widget
*/
void Controller::receiveGlobalSize(const QWidget* widget){
    emit this->sendGlobalSize(widget);
}

/*
Slot: reloads the data of the instrument
*/
void Controller::receiveInstrument(const Data::Instrument& instrument){
    emit this->sendInstrument(instrument);
}

/*
Slot: receives and sends the output data
*/
void Controller::receiveOutput(int wavelength){
    emit this->sendOutput(wavelength);
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

} // Laser namespace
