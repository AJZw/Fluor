/**** General **************************************************************
** Version:    v0.9.5
** Date:       2019-09-01
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include "general_widgets.h"

namespace General {

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

} // general namespace
