/**** General **************************************************************
** Version:    v0.9.1
** Date:       2019-02-03
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include "fluor_buttons.h"

#include <QGridLayout>
#include <QVariant>
#include <QStyle>
#include <QDebug>

namespace Fluor {

/*
Constructor: builds a PushButton
*/
PushButton::PushButton(QWidget* parent) :
    QPushButton(parent)
{
    this->setText("Fluorochromes");
    this->setToolTip("Add Fluorochromes");
}

/*
Slot: hides button
*/
void PushButton::hideButton(){
    this->hide();
}

/*
Slot: shows button
*/
void PushButton::showButton(){
    this->show();
}

// #################################################################### //

/*
Constructor: builds a Emission PushButton
*/
EmissionButton::EmissionButton(QWidget* parent) : 
    QPushButton(parent),
    tooltip_active("Toggle emission (on)"),
    tooltip_inactive("Toggle emission (off)")
{
    this->setEnabled(true);
    this->setText("");

    this->setProperty("active", true);
    this->setToolTip();

    QObject::connect(this, &QAbstractButton::clicked, this, &EmissionButton::toggleActive);
    QObject::connect(this, &QAbstractButton::clicked, this, &EmissionButton::click);
}

/*
Set tooltip based on tooltip_disabled/active/inactive strings
*/
void EmissionButton::setToolTip() {
    if(this->property("active").toBool()){
        QWidget::setToolTip(this->tooltip_active);
    }else{
        QWidget::setToolTip(this->tooltip_inactive);
    }
}

/*
Slot: receives click signal, builds and fires clicked(active) signal
    :param checked: (unused) whether the button is checked. Required for signal-slot mechanism.
*/
void EmissionButton::click(bool checked){
    Q_UNUSED(checked);
    emit this->clicked(this->property("active").toBool());
}


/*
Slot: set 'active' state of the button and changes icon`
    :param active: the active state to set the button to
*/
void EmissionButton::setActive(bool active){
    if(this->property("active").toBool() == active){
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
void EmissionButton::toggleActive(bool checked){
    Q_UNUSED(checked);

    this->setActive(!this->property("active").toBool());
}


// ################################################################## //

/*
Constructor: builds a Excitation PushButton
*/
ExcitationButton::ExcitationButton(QWidget* parent) : 
    QPushButton(parent),
    tooltip_active("Toggle excitation (on)"),
    tooltip_inactive("Toggle excitation (off)")
{
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
    if(this->property("active").toBool()){
        QWidget::setToolTip(this->tooltip_active);
    }else{
        QWidget::setToolTip(this->tooltip_inactive);
    }
}

/*
Slot: receives click signal, builds and fires clicked(active) signal
    :param checked: (unused) whether the button is checked. Required for signal-slot mechanism.
*/
void ExcitationButton::click(bool checked){
    Q_UNUSED(checked);
    emit this->clicked(this->property("active").toBool());
}

/*
Slot: set 'active' state of the button and changes icon
*/
void ExcitationButton::setActive(bool active){
    if(this->property("active").toBool() == active){
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
void ExcitationButton::toggleActive(bool checked){
    Q_UNUSED(checked);

    this->setActive(!this->property("active").toBool());
}


// ################################################################## //

/*
Constructor: builds a Remove PushButton
*/
RemoveButton::RemoveButton(QWidget* parent) : 
    QPushButton(parent),
    tooltip_active("Remove fluorophore"),
    tooltip_inactive("Remove fluorophore")
{
    this->setEnabled(true);
    this->setText("");

    this->setProperty("active", true);
    this->setToolTip();

    QObject::connect(this, &QAbstractButton::clicked, this, &RemoveButton::click);
}

/*
Slot: set 'active' state of the button and changes icon
*/
void RemoveButton::setActive(bool active){
    if(this->property("active").toBool() == active){
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
    if(this->property("active").toBool()){
        QWidget::setToolTip(this->tooltip_active);
    }else{
        QWidget::setToolTip(this->tooltip_inactive);
    }
}

/*
Slot: receives click signal, builds and fires clicked(active) signal
    :param checked: (unused) whether the button is checked. Required for signal-slot mechanism.
*/
void RemoveButton::click(bool checked){
    Q_UNUSED(checked);
    emit this->clicked(this->property("active").toBool());
}

} // Fluor namespace
