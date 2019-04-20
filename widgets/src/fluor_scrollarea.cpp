/**** General **************************************************************
** Version:    v0.9.1
** Date:       2019-02-03
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include "fluor_scrollarea.h"
#include <QGridLayout>

namespace Fluor {

/*
Initializer: builds a FluorScrollArea
*/
ScrollArea::ScrollArea(QWidget* parent) :
    QScrollArea(parent)
{
    // Set Scrollarea properties
    this->setFocusPolicy(Qt::NoFocus); // Makes FluorLineEdit popup behave in a predictable way
    this->setWidgetResizable(true);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setFrameStyle(QFrame::NoFrame);

    // Setup widget and layout inside the scrollarea
    QWidget* _widget = new QWidget(this);

    QGridLayout* _layout = new QGridLayout(this);
    _layout->setColumnMinimumWidth(0, 80);
    _layout->setColumnMinimumWidth(1, 1);
    _layout->setColumnMinimumWidth(2, 1);
    _layout->setColumnStretch(0, 1);
    _layout->setRowStretch(99, 1);
    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->setHorizontalSpacing(3);
    _layout->setVerticalSpacing(6);

    _widget->setLayout(_layout);
    this->setWidget(_widget);
}

} // Fluor namespace

/*
def __init__(self, graph_layout, data, settings, tags_list, parent=None):

    # All sorting parameters
    self.sort_setting = self.settings.settings.value("USER/sort_option", self.settings.defaults.value("DEFAULT/sort_option", "Additive"))
    # sort_setting options: additive / additive_rev & alphabet / alphabet_rev & excitation / excitation_rev & emission / emission_rev
    self.sort_reversed = self.settings.settings.value("USER/sort_reversed", self.settings.defaults.value("DEFAULT/sort_reversed", False), type=bool)
    self.sort_presort = self.settings.settings.value("USER/sort_presort", self.settings.defaults.value("DEFAULT/sort_presort", True), type=bool)

    self.sort_build_id = 0          # New button 'id' -> used for additive(_rev) sorting
    self.button_list = []           # List with meta data for sorting
    self.button_lookup = {}         # Lookup dictionary for easy lookup

*/