/**** General **************************************************************
** Version:    v0.9.5
** Date:       2019-09-01
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
** The fluorophore menu lineedit
**
** :class: General::ScrollBar
** A QScrollBar that emits signals upon showing and hiding
**
***************************************************************************/

#ifndef GENERAL_WIDGETS_H
#define GENERAL_WIDGETS_H

#include <QScrollBar>

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

} // general namespace

#endif // GENERAL_WIDGETS_H
