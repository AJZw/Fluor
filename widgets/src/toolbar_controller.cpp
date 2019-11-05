/**** General **************************************************************
** Version:    v0.9.1
** Date:       2019-03-11
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include "global.h"
#include "toolbar_controller.h"
#include "toolbar_buttons.h"
#include "application.h"
#include <QHBoxLayout>
#include <QStyleOption>
#include <QStyle>
#include <QPainter>

#include <QDebug>

namespace Bar {

/*
Initializer: builds and connects the widgets of the quick options bar
    :param parent: parent widget
*/
Controller::Controller(QWidget* parent) :
    QWidget(parent)
{
    // Set base properties
    this->setContentsMargins(0, 0, 0, 0);

    // Build layout
    QHBoxLayout* controller_layout = new QHBoxLayout(this);
    controller_layout->setContentsMargins(0, 0, 0, 0);
    controller_layout->setSpacing(6);

    this->setLayout(controller_layout);

    // Split layout into three pieces for centered toolbar button distribution
    QWidget* widget_left = new QWidget(this);
    QHBoxLayout* layout_left = new QHBoxLayout(widget_left);
    layout_left->setContentsMargins(0, 0, 0, 0);
    layout_left->setSpacing(6);

    QWidget* widget_mid = new QWidget(this);
    QHBoxLayout* layout_mid = new QHBoxLayout(widget_mid);
    layout_mid->setContentsMargins(0, 0, 0, 0);
    layout_mid->setSpacing(6);

    QWidget* widget_right = new QWidget(this);
    QHBoxLayout* layout_right = new QHBoxLayout(widget_right);
    layout_right->setContentsMargins(0, 0, 0, 0);
    layout_right->setSpacing(6);

    controller_layout->addWidget(widget_left);
    controller_layout->addWidget(widget_mid);
    controller_layout->addWidget(widget_right);

    // Add buttons
    Bar::LaserButton* widget_laser = new Bar::LaserButton(this);
    Bar::ExcitationButton* widget_excitation = new Bar::ExcitationButton(this);
    Bar::EmissionButton* widget_emission = new Bar::EmissionButton(this);
    Bar::DetectorButton* widget_detector = new Bar::DetectorButton(this);
    layout_left->addWidget(widget_laser);
    layout_left->addWidget(widget_excitation);
    layout_left->addWidget(widget_emission);
    layout_left->addWidget(widget_detector);
    layout_left->addStretch(1);

    Bar::GraphAddButton* widget_graph_add = new Bar::GraphAddButton(this);
    Bar::GraphRemoveButton* widget_graph_remove = new Bar::GraphRemoveButton(this);
    layout_mid->addStretch(1);
    layout_mid->addWidget(widget_graph_add);
    layout_mid->addWidget(widget_graph_remove);
    layout_mid->addStretch(1);

    //Bar::Label* widget_label = new Bar::Label(this);
    Bar::LasersButton* widget_lasers = new Bar::LasersButton(this);
    layout_right->addStretch(1);
    //layout_right->addWidget(widget_label);
    layout_right->addWidget(widget_lasers);

    // Connections
    QObject::connect(this, &Bar::Controller::enableLaser, widget_laser, &Bar::LaserButton::setEnabled);
    QObject::connect(this, &Bar::Controller::enableDetector, widget_detector, &Bar::DetectorButton::setEnabled);
    QObject::connect(this, &Bar::Controller::enableLasers, widget_lasers, &Bar::LasersButton::setEnabled);

    QObject::connect(widget_laser, &Bar::SquarePushButton::clicked, this, &Bar::Controller::clickedLaser);
    QObject::connect(widget_excitation, &Bar::SquarePushButton::clicked, this, &Bar::Controller::clickedExcitation);
    QObject::connect(widget_emission, &Bar::SquarePushButton::clicked, this, &Bar::Controller::clickedEmission);
    QObject::connect(widget_detector, &Bar::SquarePushButton::clicked, this, &Bar::Controller::clickedDetector);
    QObject::connect(widget_graph_add, &Bar::SquarePushButton::clicked, this, &Bar::Controller::clickedGraphAdd);
    QObject::connect(widget_graph_remove, &Bar::SquarePushButton::clicked, this, &Bar::Controller::clickedGraphRemove);
    QObject::connect(widget_lasers, &Bar::LasersButton::clicked, this, &Bar::Controller::clickedLasers);

    QObject::connect(this, &Bar::Controller::activateLaser, widget_laser, &Bar::SquarePushButton::setActive);
    QObject::connect(this, &Bar::Controller::activateExcitation, widget_excitation, &Bar::SquarePushButton::setActive);
    QObject::connect(this, &Bar::Controller::activateEmission, widget_emission, &Bar::SquarePushButton::setActive);
    QObject::connect(this, &Bar::Controller::activateDetector, widget_detector, &Bar::SquarePushButton::setActive);
    QObject::connect(this, &Bar::Controller::activateGraphAdd, widget_graph_add, &Bar::SquarePushButton::setActive);
    QObject::connect(this, &Bar::Controller::activateGraphRemove, widget_graph_remove, &Bar::SquarePushButton::setActive);
    QObject::connect(this, &Bar::Controller::activateLasers, widget_lasers, &Bar::LasersButton::setActive);
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
Getter for layout spacing property, as it has to return a QString it returns the value in pixels
*/
QString Controller::layoutSpacing() const {
    return QString::number(this->layout()->spacing(), 'f', 0);
}

/*
Receives layout scaling properties from the stylesheet
*/
void Controller::setLayoutSpacing(QString layout_spacing){
    this->layout()->setSpacing(layout_spacing.toInt());
}

/*
Slot: receives Toolbar state update
    :param type: which button to update the state of
    :param state: the value to update to state property to
    :param enable: the valeu to update the enable property to (if applicable) 
*/
void Controller::receiveToolbarStateUpdate(Bar::ButtonType type, bool active, bool enable){
    switch(type){
    case ButtonType::Laser:
        emit this->enableLaser(enable);
        emit this->activateLaser(active);
        return;
    case ButtonType::Excitation:
        emit this->activateExcitation(active);
        return;
    case ButtonType::Emission:
        emit this->activateEmission(active);
        return;
    case ButtonType::Detector:
        emit this->enableDetector(enable);
        emit this->activateDetector(active);
        return;
    case ButtonType::GraphAdd:
        emit this->activateGraphAdd(active);
        return;
    case ButtonType::GraphRemove:
        emit this->activateGraphRemove(active);
        return;
    case ButtonType::Lasers:
        emit this->enableLasers(enable);
        emit this->activateLasers(active);
        return;
    default:
        qWarning() << "Toolbar::Controller::receiveToolbarStateUpdate: unknown button";
    }
}

/*
Slot: handles widget_laser::clicked() signals
    :param active: whether the widget is active or non-active (after the click)
*/
void Controller::clickedLaser(bool active) {
    //emit this->setVisibilityLaser(active);
    emit this->sendToolbarStateChange(Bar::ButtonType::Laser, active, true);
}

/*
Slot: handles widget_excitation::clicked() signals
    :param active: whether the widget is active or non-active (after the click)
*/
void Controller::clickedExcitation(bool active) {
    //emit this->setVisibilityExcitation(active);
    emit this->sendToolbarStateChange(Bar::ButtonType::Excitation, active, true);
}

/*
Slot: handles widget_emission::clicked() signals
    :param active: whether the widget is active or non-active (after the click)
*/
void Controller::clickedEmission(bool active) {
    //emit this->setVisibilityEmission(active);
    emit this->sendToolbarStateChange(Bar::ButtonType::Emission, active, true);
}

/*
Slot: handles widget_detector::clicked() signals
    :param active: whether the widget is active or non-active (after the click)
*/
void Controller::clickedDetector(bool active){
    //emit this->setVisibilityDetector(active);
    emit this->sendToolbarStateChange(Bar::ButtonType::Detector, active, true);
}

/*
Slot: handles widget_graph_add::clicked() signals
    :param active: whether the widget is active or non-active (after the click)
*/
void Controller::clickedGraphAdd(bool active) {
    //emit this->addGraph(active);
    emit this->sendToolbarStateChange(Bar::ButtonType::GraphAdd, active, true);
}

/*
Slot: handles widget_graph_remove::clicked() signals
    :param active: whether the widget is active or non-active (after the click)
*/
void Controller::clickedGraphRemove(bool active) {
    //emit this->removeGraph(active);
    emit this->sendToolbarStateChange(Bar::ButtonType::GraphRemove, active, true);
}

/*
Slot: handles widget_laserse::clicked() signals
    :param active: whether the widget is active or non-active (after the click)
*/
void Controller::clickedLasers(bool active) {
    //emit this->expandLasers(active);
    emit this->sendToolbarStateChange(Bar::ButtonType::Lasers, active, true);
}

} // Bar namespace
