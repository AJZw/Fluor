/**** General **************************************************************
** Version:    v0.9.1
** Date:       2019-02-03
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include "laser_buttons.h"

namespace Laser {

/*
Initializer: builds a PushButton
*/
PushButton::PushButton(QWidget* parent) :
    QPushButton(parent)
{
    this->setText("Lasers");
    this->setToolTip("Add Laser");
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

} // Laser namespace
